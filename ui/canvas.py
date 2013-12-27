import random

from PySide import QtCore, QtGui

class Canvas(QtGui.QWidget):
    def __init__(self):
        super(Canvas, self).__init__()
        self.setMouseTracking(True)
        self.setGeometry(0, 900/4, 1440/2, 900/2)
        self.setWindowTitle("Antimony")

        self.center = QtCore.QPointF(0, 0)
        self.scale = 10.0 # scale is measured in pixels/mm

        self.dragging = False
        self.mouse_pos = QtCore.QPointF(self.width()/2, self.height()/2)

        self.render_tasks = {}

        self.show()


    def contextMenuEvent(self, event):
        point = event.pos()
        x, y = self.pixel_to_mm(point.x(), point.y())
        scale = 50/self.scale

        menu = QtGui.QMenu()
        tri = menu.addAction("Triangle")
        cir = menu.addAction("Circle")
        pt  = menu.addAction("Point")

        point = self.mapToGlobal(point)
        selected = menu.exec_(point)
        if selected == tri:
            TriangleControl.new(self, x, y, scale)
        elif selected == cir:
            CircleControl.new(self, x, y, scale)
        elif selected == pt:
            PointControl.new(self, x, y, scale)


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
            delta = p - self.mouse_pos
            self.drag(-delta.x() / self.scale, delta.y() / self.scale)
        self.mouse_pos = p


    def wheelEvent(self, event):
        """ Zooms in or out based on mouse wheel spinning.
        """
        pos = self.pixel_to_mm(self.mouse_pos.x(), self.mouse_pos.y())
        factor = 1.001 if event.delta() > 0 else 1/1.001
        for d in range(abs(event.delta())):
            self.scale *= factor
        new_pos = self.pixel_to_mm(self.mouse_pos.x(), self.mouse_pos.y())
        self.center += QtCore.QPointF(*pos) - QtCore.QPointF(*new_pos)
        self.sync_all_children()
        self.update()


    def mouseReleaseEvent(self, event):
        """ Stops dragging if the left button is released.
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.dragging = False


    def drag(self, dx, dy):
        """ Drags the center of canvas around by the given delta
            (in unit coordinates)
        """
        self.center += QtCore.QPointF(dx, dy)
        self.update()
        self.sync_all_children()


    def paintEvent(self, paintEvent):
        """ Paints rendered expressions and the canvas axes.
        """
        # Start expressions rendering (asynchronously)
        # (not strictly part of the paint process, but I'm putting it here
        #  so that it gets called whenever anything changes)
        self.render_expressions(self.scale)

        painter = QtGui.QPainter(self)
        painter.setBackground(QtGui.QColor(20, 20, 20))
        painter.eraseRect(self.rect())

        # Draw expression images
        self.draw_expressions(painter)

        # Draw a pair of axes
        center = self.mm_to_pixel(0, 0)
        painter.setPen(QtGui.QPen(QtGui.QColor(255, 0, 0), 2))
        painter.drawLine(center[0], center[1], center[0] + 80, center[1])
        painter.setPen(QtGui.QPen(QtGui.QColor(0, 255, 0), 2))
        painter.drawLine(center[0], center[1], center[0], center[1] - 80)


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



    def mm_to_pixel(self, x=None, y=None):
        """ Converts an x,y position in mm into a pixel coordinate.
        """
        if x is not None:
            x = int((x - self.center.x()) * self.scale + self.size().width()/2)
        if y is not None:
            y = int((self.center.y() - y) * self.scale + self.size().height()/2)

        if x is not None and y is not None:     return x, y
        elif x is not None:                     return x
        elif y is not None:                     return y


    def pixel_to_mm(self, x=None, y=None):
        """ Converts a pixel location into an x,y coordinate.
        """
        if x is not None:
            x =  (x - self.width()/2) / self.scale + self.center.x()
        if y is not None:
            y = -((y - self.height()/2) / self.scale - self.center.y())
        if x is not None and y is not None:     return x, y
        elif x is not None:                     return x
        elif y is not None:                     return y


    def get_bounding_rect(self, expression):
        """ For a given expression, finds a bounding rectangle
            (to draw that expression's image).
        """
        xmin, ymax = self.mm_to_pixel(expression.xmin, expression.ymin)
        xmax, ymin = self.mm_to_pixel(expression.xmax, expression.ymax)
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
        comp = painter.compositionMode()
        painter.setCompositionMode(QtGui.QPainter.CompositionMode_Lighten)
        for tasks in self.render_tasks.itervalues():
            for t in tasks[::-1]:
                if t.qimage:
                    painter.drawImage(self.get_bounding_rect(t.expression),
                                      t.qimage, t.qimage.rect())
                    break
        painter.setCompositionMode(comp)


    def find_expressions(self):
        """ Searches for expressions to render (i.e. expressions
            which are valid and have xy bounds).
            Returns a list of (datum, expression) tuples.
        """
        expressions = []
        for c in self.findChildren(NodeControl):
            for t, d in c.node.datums:
                if d.type == Expression and d.valid():
                    e = d.value()
                    if e.has_xy_bounds():
                        expressions.append((d,e))
        return expressions

################################################################################

from control.base import NodeControl
from fab.expression import Expression

from node.point import Point
from control.point import PointControl

from node.circle import Circle
from control.circle import CircleControl

from node.triangle import Triangle
from control.triangle import TriangleControl

from ui.editor import Editor
from ui.render import RenderTask
