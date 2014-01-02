from PySide import QtCore, QtGui

import base
import colors

class TriangleControl(base.DraggableNodeControl):

    @classmethod
    def new(cls, canvas, x, y, scale):
        """ Construct a new triangle at the given location.
            Location should be specified in units.
        """
        a = Point(get_name('a'), x - scale, y - scale)
        b = Point(get_name('b'), x, y + scale)
        c = Point(get_name('c'), x + scale, y - scale)
        tri = Triangle('t', a, b, c)

        for p in [a, b, c]:     ChildPointControl(canvas, p, tri)
        tri_ctrl = cls(canvas, tri)


    def __init__(self, canvas, target):
        """ Construct the triangle control widget, creating
            helper ChildPointControl widgets for each of the triangle's
            vertices.
        """
        self.position = QtCore.QPointF()
        self.point_nodes = [target.a, target.b, target.c]
        super(TriangleControl, self).__init__(canvas, target)

        self.editor_datums = ['name','shape']

        self.sync()
        self.make_mask()

        self.show()
        self.raise_()


    def hit(self, pos):
        return self.mask.contains(pos)


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

        painter.begin(bitmap)
        self.draw_center(painter, mask=True)
        self.mask = QtGui.QRegion(bitmap)
        self.draw_lines(painter, mask=True)
        painter.end()

        self.setMask(bitmap)


    def drag(self, dx, dy):
        for pt in self.point_nodes:  pt.control.drag(dx, dy)


    def mouseDoubleClickEvent(self, event):
        """ On double click, open the central editor
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.open_editor()

    def delete(self, trigger=None):
        """ Overloaded delete (that can be invoked from a child)
            Deletes self and all ChildPointControl objects.
        """
        for p in self.point_nodes:
            if p.control != trigger:
                p.control.delete()
        super(TriangleControl, self).delete()

    def sync(self):
        for pt in self.point_nodes:   pt.control.sync()

        xmin = min(pt.control.position.x() for pt in self.point_nodes)
        xmax = max(pt.control.position.x() for pt in self.point_nodes)

        ymin = min(pt.control.position.y() for pt in self.point_nodes)
        ymax = max(pt.control.position.y() for pt in self.point_nodes)

        x = sum(pt.control.position.x() for pt in self.point_nodes) / 3.0
        y = sum(pt.control.position.y() for pt in self.point_nodes) / 3.0

        i = self.canvas.mm_to_pixel(x=xmin) - 5
        j = self.canvas.mm_to_pixel(y=ymax) - 5
        di = self.canvas.mm_to_pixel(x=xmax) - i + 10
        dj = self.canvas.mm_to_pixel(y=ymin) - j + 10

        changed = (self.position != QtCore.QPointF(x, y) or
                   di != self.width() or dj != self.height() or
                   self.pos() != QtCore.QPoint(i, j))

        self.move(i, j)
        self.resize(di, dj)

        # Cache position here
        self.position = QtCore.QPointF(x, y)

        if changed:
            self.make_mask()
            self.update()
        if self.editor:     self.editor.sync()

    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        self.draw(painter)

    def draw_lines(self, painter, mask=False):
        coords = [
                QtCore.QPoint(
                    *self.canvas.mm_to_pixel(pt.control.position.x(),
                                             pt.control.position.y())) - self.pos()
                for pt in self.point_nodes]

        lines = [QtCore.QLine(coords[0], coords[1]),
                 QtCore.QLine(coords[1], coords[2]),
                 QtCore.QLine(coords[2], coords[0])]

        if mask:
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 4))
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(200, 200, 200), 4))
        painter.drawLines(lines)


    def draw_center(self, painter, mask=False):
        x, y = self.canvas.mm_to_pixel(self.position.x(), self.position.y())
        x -= self.pos().x()
        y -= self.pos().y()

        if mask:
            painter.setBrush(QtGui.QBrush(QtCore.Qt.color1))
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 2))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(*colors.light_grey)))
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.dark_grey), 2))

        if mask:            d = 22
        elif self.hovering: d = 20
        else:               d = 16

        painter.drawEllipse(x - d/2, y - d/2, d, d)
        if mask:    return

        painter.setPen(QtGui.QPen(QtGui.QColor(*colors.dark_grey), 0))
        lines = [QtCore.QLine(x-4, y+2, x, y-4),
                 QtCore.QLine(x, y-4, x+4, y+2),
                 QtCore.QLine(x+4, y+2, x-4, y+2)]
        painter.drawLines(lines)


    def draw(self, painter, mask=False):
        self.draw_lines(painter, mask)
        self.draw_center(painter, mask)


from node.point import Point
from node.triangle import Triangle
from node.base import get_name

from point import ChildPointControl
