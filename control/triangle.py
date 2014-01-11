from PySide import QtCore, QtGui

import base
import colors

class TriangleControl(base.NodeControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Construct a new triangle at the given location.
            Location should be specified in units.
        """
        a = Point(get_name('a'), x - scale, y - scale)
        b = Point(get_name('b'), x, y + scale)
        c = Point(get_name('c'), x + scale, y - scale)
        tri = Triangle('t', a, b, c)

        for p in [a, b, c]:     PointControl(canvas, p)
        return cls(canvas, tri)


    def __init__(self, canvas, target):
        """ Construct the triangle control widget
        """
        self.position = QtCore.QPointF()
        self.point_nodes = [target.a, target.b, target.c]
        super(TriangleControl, self).__init__(canvas, target)

        self.drag_control = base.DragManager(self, self.drag)
        self.editor_datums = ['name','shape']

        self.sync()
        self.make_mask()

        self.show()
        self.raise_()



    def raise_(self):
        """ Overload raise_ so that points stay above triangle lines.
        """
        super(TriangleControl, self).raise_()
        for p in self.point_nodes:   p.control.raise_()


    def make_mask(self):
        """ Make the triangular mask image.
        """
        painter = QtGui.QPainter()
        bitmap = QtGui.QBitmap(self.size())
        bitmap.clear()

        # We'll store the center as the clickable mask region
        painter.begin(bitmap)
        self.draw_center(painter, mask=True)
        painter.end()
        self.drag_control.mask = QtGui.QRegion(bitmap)


    def drag(self, v, p):
        for pt in self.point_nodes:
            pt.control.drag_control.dragXY(v, None)


    def _sync(self):
        for pt in self.point_nodes:   pt.control.sync()

        # Get bounding box from painter path
        rect = self.triangle_path().boundingRect().toRect()
        rect.setTop(rect.top() - 5)
        rect.setBottom(rect.bottom() + 5)
        rect.setLeft(rect.left() - 5)
        rect.setRight(rect.right() + 5)

        # Place x and y coordinates at center of triangle
        x = sum(pt.control.position.x() for pt in self.point_nodes) / 3.0
        y = sum(pt.control.position.y() for pt in self.point_nodes) / 3.0

        changed = (self.position != QtCore.QPointF(x, y) or
                   self.geometry() != rect)

        self.setGeometry(rect)

        # Cache position here
        self.position = QtCore.QPointF(x, y)

        if changed:
            self.make_mask()
            self.update()


    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        self.draw(painter)


    def triangle_path(self, offset=QtCore.QPoint()):
        """ Returns a QPainterPath that draws the triangle.
        """
        coords = [QtGui.QVector3D(pt.control.position)
                  for pt in self.point_nodes]
        coords.append(coords[0])
        return self.draw_lines([coords], offset)


    def draw_triangle(self, painter, mask=False):
        """ Draws the triangle on the given painter.
        """
        if mask:
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 4))
            painter.setBrush(QtGui.QBrush())
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(200, 200, 200), 4))
        painter.drawPath(self.triangle_path(self.pos()))


    def draw_center(self, painter, mask=False):
        pos = self.canvas.unit_to_pixel(self.position) - self.pos()
        x, y = pos.x(), pos.y()

        if mask:
            painter.setBrush(QtGui.QBrush(QtCore.Qt.color1))
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 2))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(*colors.light_grey)))
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.dark_grey), 2))

        if mask:
            d = 22
        elif self.drag_control.hover or self.drag_control.drag:
            d = 20
        else:
            d = 16

        painter.drawEllipse(x - d/2, y - d/2, d, d)
        if mask:    return

        painter.setPen(QtGui.QPen(QtGui.QColor(*colors.dark_grey), 0))
        lines = [QtCore.QLine(x-4, y+2, x, y-4),
                 QtCore.QLine(x, y-4, x+4, y+2),
                 QtCore.QLine(x+4, y+2, x-4, y+2)]
        painter.drawLines(lines)


    def draw(self, painter, mask=False):
        self.draw_triangle(painter, mask)
        self.draw_center(painter, mask)


from node.point import Point
from node.triangle import Triangle
from node.base import get_name

from point import PointControl
