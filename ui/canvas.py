import math

from PySide import QtCore, QtGui

from control import colors
import button

class Canvas(QtGui.QWidget):
    def __init__(self):
        super(Canvas, self).__init__()
        self.setFocusPolicy(QtCore.Qt.ClickFocus)
        self.setMouseTracking(True)

        self.center = QtGui.QVector3D(0, 0, 0)
        self.yaw   = 0
        self.pitch = 0
        self.scale = 0.01

        self.dragging = False
        self.mouse_pos = QtCore.QPointF(self.width()/2, self.height()/2)

        self.render_tasks = {}

        self.show()

    def drag_vector(self, start, end):
        """ Returns the drag vector from start to end (as a QVector3D)
            Input arguments should be in pixel coordinates.
        """
        return self.pixel_to_unit(end) - self.pixel_to_unit(start)

    def keyPressEvent(self, event):
        """ Rotates the camera around with arrow keys.
        """
        if event.key() == QtCore.Qt.Key_Up:
            self.pitch = max(-math.pi, self.pitch - math.pi/32)
        elif event.key() == QtCore.Qt.Key_Down:
            self.pitch = min(0, self.pitch + math.pi/32)
        elif event.key() == QtCore.Qt.Key_Left:
            self.yaw += math.pi/32
        elif event.key() == QtCore.Qt.Key_Right:
            self.yaw -= math.pi/32
        self.sync_all_children()
        self.update()

    def contextMenuEvent(self, event):
        self.openMenuAt(event.pos())

    def openMenuAt(self, point):
        menu = QtGui.QMenu()
        items = [("Triangle", TriangleControl),
                 ("Circle", CircleControl),
                 ("Point (2D)", PointControl),
                 ("Point (3D)", Point3DControl), None,
                 ("Union", UnionControl),
                 ("Intersection", IntersectionControl),
                 ("Cutout", CutoutControl), None,
                 ("Get bounds", GetBoundsControl),
                 ("Set bounds", SetBoundsControl)]

        actions = {}
        for i in items:
            if i is None:   menu.addSeparator()
            else:           actions[menu.addAction(i[0])] = i[1].new

        # Open up the menu at the given point and get a constructor back.
        constructor = actions.get(menu.exec_(self.mapToGlobal(point)),
                                  lambda *args: None)

        # Figure out constructor parameters (from cursor position)
        point = self.mapFromGlobal(QtGui.QCursor.pos())
        pos = self.pixel_to_unit(point)
        x, y, z  = pos.x(), pos.y(), pos.z()
        scale = self.pixel_to_unit(x=point.x() + 50) - x

        # Call the constructor, making a control
        ctrl = constructor(self, x, y, z, scale)

        # Start dragging this control if possible.
        drag_managers = [getattr(ctrl, d) for d in dir(ctrl)
                         if isinstance(getattr(ctrl, d), DragManager)]
        # Priority XYZ dragging, then XY, then other
        xyz = [d for d in drag_managers if isinstance(d, DragXYZ)]
        xy  = [d for d in drag_managers if isinstance(d, DragXY)]
        if xyz:             dm = xyz[0]
        elif xy:            dm = xy[0]
        elif drag_managers: dm = drag_managers[0]
        else:               dm = None

        if dm:
            dm.drag = True
            dm.mouse_pos = point
            ctrl.grabMouse()


    def mousePressEvent(self, event):
        """ Starts dragging if the left button is pressed.
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.mouse_pos = event.pos()
            self.dragging = True


    def mouseMoveEvent(self, event):
        """ Tracks mouse position and drags the canvas center around.
        """
        p = event.pos()
        if self.dragging:
            delta = self.pixel_to_unit(p) - self.pixel_to_unit(self.mouse_pos)
            self.drag(-delta.x(), -delta.y(), -delta.z())
        self.mouse_pos = p


    def wheelEvent(self, event):
        """ Zooms in or out based on mouse wheel spinning.
        """
        pos = self.pixel_to_unit(self.mouse_pos)
        factor = 1.001 if event.delta() > 0 else 1/1.001
        for d in range(abs(event.delta())):
            self.scale *= factor
        new_pos = self.pixel_to_unit(self.mouse_pos)
        self.center += pos - new_pos
        self.sync_all_children()
        self.update()


    def mouseReleaseEvent(self, event):
        """ Stops dragging if the left button is released.
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.dragging = False


    def drag(self, dx, dy, dz):
        """ Drags the center of canvas around by the given delta
            (in unit coordinates)
        """
        self.center += QtGui.QVector3D(dx, dy, dz)
        self.update()
        self.sync_all_children()


    def paintEvent(self, paintEvent):
        """ Paints rendered expressions and the canvas axes.
        """
        # Start expressions rendering (asynchronously)
        # (not strictly part of the paint process, but I'm putting it here
        #  so that it gets called whenever anything changes)
        pix_per_unit = self.unit_to_pixel(1) - self.unit_to_pixel(0)
        self.render_expressions(pix_per_unit)

        painter = QtGui.QPainter(self)
        painter.setBackground(QtGui.QColor(0, 0, 0))
        painter.eraseRect(self.rect())

        # Draw expression images
        self.draw_expressions(painter)

        # Draw a pair of axes
        center = self.unit_to_pixel(0, 0, 0)
        x = self.unit_to_pixel(0.2/self.scale, 0, 0)
        y = self.unit_to_pixel(0, 0.2/self.scale, 0)
        z = self.unit_to_pixel(0, 0, 0.2/self.scale)
        painter.setPen(QtGui.QPen(QtGui.QColor(*colors.red), 2))
        painter.drawLine(center, x)
        painter.setPen(QtGui.QPen(QtGui.QColor(*colors.green), 2))
        painter.drawLine(center, y)
        painter.setPen(QtGui.QPen(QtGui.QColor(*colors.blue), 2))
        painter.drawLine(center, z)


    def sync_all_children(self):
        """ Calls sync on all children that have that function.
        """
        for c in self.findChildren(QtGui.QWidget):
            if hasattr(c, 'sync'):  c.sync()


    def resizeEvent(self, event):
        """ On resize, refresh oneself and sync children positions.
        """
        self.update()
        self.sync_all_children()
        for b in self.findChildren(button.Button):
            b.position(self.height())


    def pixel_matrix(self):
        """ Defines a matrix that maps the OpenGL standard cube
            (-1, -1, -1), (1, 1, 1) into the screen's coordinates.
        """
        M = QtGui.QMatrix4x4()
        M.translate(self.width()/2, self.height()/2)
        M.scale(min(self.width(), self.height()) / 2)
        M.scale(1, -1)
        return M


    def transform_matrix(self):
        """ Returns a matrix that converts coordinates into the OpenGL
            bounding box.
        """

        # Remember that these operations are applied back-asswards.
        M = QtGui.QMatrix4x4()
        M.scale(self.scale)
        M.rotate(math.degrees(self.pitch), QtGui.QVector3D(1, 0, 0))
        M.rotate(math.degrees(self.yaw), QtGui.QVector3D(0, 0, 1))
        M.translate(-self.center)
        return M

    def projection_matrix(self):
        """ Convert the OpenGL bounding box into screen coordinates.
        """
        pass

    def unit_to_pixel(self, x=None, y=None, z=None):
        """ Converts an x,y position in mm into a pixel coordinate.
            Takes in either a three-argument coordinate, a QVector3D,
            or a QPointF; returns a QPoint.
        """
        if isinstance(x, QtGui.QVector3D): x, y, z = x.x(), x.y(), x.z()
        elif isinstance(x, QtCore.QPointF): x, y, z = x.x(), x.y(), 0

        v = QtGui.QVector3D(x if x else 0, y if y else 0, z if z else 0)
        M = self.pixel_matrix() * self.transform_matrix()
        v_ = M * v

        out = []
        if x is not None:   out.append(v_.x())
        if y is not None:   out.append(v_.y())
        if z is not None:   out.append(v_.z())

        if len(out) == 1:   return out[0]
        else:               return QtCore.QPoint(v_.x(), v_.y())


    def pixel_to_unit(self, x=None, y=None):
        """ Converts a pixel location into an x,y coordinate.
        """
        if isinstance(x, QtCore.QPoint):    x, y = x.x(), x.y()

        v = QtGui.QVector3D(x if x else 0, y if y else 0, 0)
        M = (self.transform_matrix().inverted()[0] *
             self.pixel_matrix().inverted()[0])
        v_ = M * v

        out = []
        if x is not None and y is not None: return v_
        elif x is not None: return v_.x()
        elif y is not None: return v_.y()


    def get_bounding_rect(self, expression):
        """ For a given expression, finds a bounding rectangle
            (to draw that expression's image).
        """
        c1 = self.unit_to_pixel(expression.xmin, expression.ymin)
        xmin, ymax = c1.x(), c1.y()
        c2 = self.unit_to_pixel(expression.xmax, expression.ymax)
        xmax, ymin = c2.x(), c2.y()
        return QtCore.QRect(xmin, ymin, xmax-xmin, ymax-ymin)


    def find_input(self, pos):
        """ Hunts through all Editor panels to find one with
            a connection.Input control at the given position, returning
            None otherwise.
        """
        for c in self.findChildren(Editor):
            i = c.find_input(pos)
            if i is not None:   return i
        return None


    def render_expressions(self, pix_per_unit):
        """ Starts render tasks for all new expressions that don't already
            have render tasks.
        """
        found = self.find_expressions()
        if found:
            # Find datums and expressions that need rendering.
            datums, expressions = zip(*found)
        else:
            datums, expressions = [], []

        # Remove all but the most recent image for render tasks
        # with datums that are present, or all images for render
        # tasks without a currently active datum
        to_delete = []
        for k in self.render_tasks:
            delete = k not in datums
            while (len(self.render_tasks[k]) > (0 if delete else 1) and
                   self.render_tasks[k][0].join()):
                self.render_tasks[k] = self.render_tasks[k][1:]
            if not self.render_tasks[k]:
                to_delete.append(k)
        for k in to_delete:
            del(self.render_tasks[k])

        # Check if the last render task is useful; otherwise
        # start a new one at the back of the list
        for d, e in zip(datums, expressions):
            if (d in self.render_tasks and
                self.render_tasks[d][-1].expression == e and
                self.render_tasks[d][-1].resolution == pix_per_unit):
                continue
            else:
                self.render_tasks[d] = (
                        self.render_tasks.get(d, []) +
                        [RenderTask(e, pix_per_unit, self.update)])


    def draw_expressions(self, painter):
        """ Paints all rendered expressions (i.e. RenderTasks with a qimage
            member variable.
        """
        comp = painter.compositionMode()
        painter.setCompositionMode(QtGui.QPainter.CompositionMode_Lighten)
        for tasks in self.render_tasks.itervalues():
            for t in tasks[::-1]:
                if t.qimage:
                    t.qimage.save("image.png")
                    painter.drawImage(self.get_bounding_rect(t.expression),
                                      t.qimage, t.qimage.rect())
                    break
        painter.setCompositionMode(comp)


    def find_expressions(self):
        """ Searches for expressions to render (i.e. expressions
            which are valid, have xy bounds, and don't have connections)
            Returns a list of (datum, expression) tuples.
        """
        expressions = []
        for c in self.findChildren(NodeControl):
            for datum_name, datum in c.node.datums:
                if (isinstance(datum, ExpressionFunctionDatum) and
                        not datum.connections() and datum.valid()):
                    e = datum.value()
                    if e.has_xy_bounds():
                        expressions.append((datum,e))
        return expressions

################################################################################

from control.base import NodeControl, DragManager, DragXY, DragXYZ
from fab.expression import Expression

from node.datum import ExpressionFunctionDatum

from control.point import PointControl, Point3DControl
from control.circle import CircleControl
from control.triangle import TriangleControl
from control.csg import UnionControl, IntersectionControl, CutoutControl
from control.bounds import GetBoundsControl, SetBoundsControl

from ui.editor import Editor
from ui.render import RenderTask
