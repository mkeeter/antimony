from PySide import QtCore, QtGui

import base
import colors

class CubeControl(base.NodeControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new cube at the given location.
        """
        a = Point3D(get_name('a'), x - scale, y - scale, z - scale)
        b = Point3D(get_name('b'), x + scale, y + scale, z + scale)
        cube = Cube('c', a, b)

        for p in [a, b]:    Point3DControl(canvas, p)
        return cls(canvas, cube)

    def __init__(self, canvas, target):
        """ Construct the cube control widget.
        """
        super(CubeControl, self).__init__(canvas, target)

        self.drag_control = base.DragManager(self, self.drag)
        self.editor_datums = ['name', 'shape']

        self.corners = [None, None]

        self.sync()
        self.make_mask()

        self.show()
        self.raise_()

    def raise_(self):
        """ Overload raise so that point stay above lines.
        """
        super(CubeControl, self).raise_()
        self.node.a.control.raise_()
        self.node.b.control.raise_()

    def make_mask(self):
        """ Make the cube's mask and assign it to painters.
        """
        painter = QtGui.QPainter()
        bitmap = QtGui.QBitmap(self.size())
        bitmap.clear()

        painter.begin(bitmap)
        self.draw_center(painter, mask=True)
        painter.end()
        self.drag_control.mask = QtGui.QRegion(bitmap)

    def drag(self, v, p):
        """ Call the drag methods of all child points.
        """
        self.node.a.control.drag_control.dragXYZ(v, p)
        self.node.b.control.drag_control.dragXYZ(v, p)

    def wireframe_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws this cube as a wireframe.
        """
        corner1 = self.node.a.control.position
        corner2 = self.node.b.control.position
        x0, y0, z0 = corner1.x(), corner1.y(), corner1.z()
        x1, y1, z1 = corner2.x(), corner2.y(), corner2.z()
        v = QtGui.QVector3D
        return self.draw_lines([
            [v(x0, y0, z0), v(x0, y0, z1), v(x0, y1, z1),
                v(x0, y1, z0), v(x0, y0, z0)],
            [v(x1, y0, z0), v(x1, y0, z1), v(x1, y1, z1),
                v(x1, y1, z0), v(x1, y0, z0)],
            [v(x0, y0, z0), v(x1, y0, z0)],
            [v(x0, y1, z0), v(x1, y1, z0)],
            [v(x0, y0, z1), v(x1, y0, z1)],
            [v(x0, y1, z1), v(x1, y1, z1)]], offset)

    def _sync(self):
        """ Updates children nodes, then updates self.
        """
        # Use any to avoid short-circuiting
        changed = any([self.node.a.control.sync(),
                       self.node.b.control.sync()])
        corners = [self.node.a.control.position, self.node.b.control.position]
        changed |= (corners != self.corners)
        return changed


    def reposition(self):
        """ Repositions this node and calls self.update
        """
        rect = self.wireframe_path().boundingRect().toRect()
        rect.setTop(rect.top() - 5)
        rect.setBottom(rect.bottom() + 5)
        rect.setLeft(rect.left() - 5)
        rect.setRight(rect.right() + 5)

        self.setGeometry(rect)

        self.make_mask()
        self.update()

    def paintEvent(self, paintEvent):
        """ Paints this widget when necessary.
        """
        painter = QtGui.QPainter(self)
        self.draw_wireframe(painter)
        self.draw_center(painter)

    def draw_wireframe(self, painter):
        """ Draws the wireframe for this cube.
        """
        painter.setBrush(QtGui.QBrush())
        painter.setPen(QtGui.QPen(QtGui.QColor(*colors.light_grey), 2))
        p = self.wireframe_path(self.pos())
        painter.drawPath(p)


    def draw_center(self, painter, mask=False):
        """ Draws a dot at the center of this cube.
        """
        pos = (self.canvas.unit_to_pixel(
                    (self.node.a.control.position +
                     self.node.b.control.position)/2)
                - self.pos())
        x, y = pos.x(), pos.y()
        if mask:
            painter.setBrush(QtGui.QBrush(QtCore.Qt.color1))
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 2))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(*colors.light_grey)))
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.dark_grey), 2))

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 16

        painter.drawEllipse(x - d/2, y - d/2, d, d)
        lines = [QtCore.QLine(x-3, y-3, x+3, y-3),
                 QtCore.QLine(x+3, y-3, x+3, y+3),
                 QtCore.QLine(x+3, y+3, x-3, y+3),
                 QtCore.QLine(x-3, y+3, x-3, y-3)]
        painter.drawLines(lines)


    def editor_position(self):
        """ Returns a QPoint defining the position at which we should
            open up the editor.
        """
        return self.canvas.unit_to_pixel(
                    (self.node.a.control.position +
                     self.node.b.control.position)/2)

from node.point import Point3D
from control.point import Point3DControl
from node.base import get_name
from node.cube import Cube
