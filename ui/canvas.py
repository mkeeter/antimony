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

        self.render_tasks = []

        self.scatter_points(2)
        self.make_circle()
        self.show()

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.mouse_pos = event.pos()
            self.dragging = True

    def mouseMoveEvent(self, event):
        p = event.pos()
        if self.dragging:
            delta = p - self.mouse_pos
            self.drag(-delta.x() / self.scale, delta.y() / self.scale)
        self.mouse_pos = p

    def wheelEvent(self, event):
        pos = self.pixel_to_mm(self.mouse_pos.x(), self.mouse_pos.y())
        factor = 1.001 if event.delta() > 0 else 1/1.001
        for d in range(abs(event.delta())):
            self.scale *= factor
        new_pos = self.pixel_to_mm(self.mouse_pos.x(), self.mouse_pos.y())
        self.center += QtCore.QPointF(*pos) - QtCore.QPointF(*new_pos)
        self.sync_all_children()
        self.update()

    def mouseReleaseEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.dragging = False

    def drag(self, dx, dy):
        self.center += QtCore.QPointF(dx, dy)
        self.update()
        self.sync_all_children()

    def paintEvent(self, paintEvent):

        # Start expressions rendering (asynchronously)
        # (not strictly part of the paint process, but I'm putting it here
        #  so that it gets called whenever anything changes)
        self.render_expressions(10)

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
        for c in self.findChildren(QtGui.QWidget):
            if hasattr(c, 'sync'):  c.sync()

    def resizeEvent(self, event):
        self.update()
        self.sync_all_children()

    def scatter_points(self, n):
        for i in range(n):
            pt = Point('p%i' % i, random.uniform(-10, 10), random.uniform(-10, 10))
            ctrl = PointControl(self, pt)
            e = Editor(ctrl)
            ctrl.editor = e
            ctrl.raise_()

    def make_circle(self):
        c = Circle('c', 1, 1, 4)
        ctrl = CircleControl(self, c)
        e = Editor(ctrl)
        ctrl.editor = e
        ctrl.raise_()

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
        new_tasks = []
        old_expressions = [r.expression for r in self.render_tasks]
        for e in self.find_expressions():
            try:
                i = old_expressions.index(e)
            except ValueError:
                new_tasks.append(RenderTask(e, pix_per_unit, self.update))
            else:
                # Attempt to join this task
                self.render_tasks[i].join()
                # We want to keep it around, because it has made more
                # progress rendering this expression than a new task.
                new_tasks.append(self.render_tasks[i])
                # Remove the useful tasks from the list of old tasks
                self.render_tasks = (
                        self.render_tasks[:i] + self.render_tasks[i+1:])

        # Attempt to halt all remaining (non-useful) threads.
        # If we can't join the thread, then keep it around to avoid
        # leaking threads.
        for t in self.render_tasks:
            if not t.join():    new_tasks.append(t)
        self.render_tasks = new_tasks
        print self.render_tasks


    def draw_expressions(self, painter):
        for p in self.render_tasks:
            if not p.qimage:    continue
            painter.drawImage(self.get_bounding_rect(p.expression),
                              p.qimage, p.qimage.rect())


    def find_expressions(self):
        """ Searches for expressions to render (i.e. expressions
            which are valid and have xy bounds).
        """
        expressions = []
        for c in self.findChildren(NodeControl):
            for t, d in c.node.datums:
                if d.type == Expression and d.valid():
                    e = d.value()
                    if e.has_xy_bounds():
                        expressions.append(e)
        return expressions

################################################################################

from control.base import NodeControl
from fab.expression import Expression

from node.point import Point
from control.point import PointControl

from node.circle import Circle
from control.circle import CircleControl

from ui.editor import Editor
from ui.render import RenderTask
