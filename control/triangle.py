from PySide import QtCore, QtGui

import base

class TriangleControl(base.NodeControl):
    def __init__(self, canvas, target):
        self.position = QtCore.QPointF()
        self.points = [
            ChildPointControl(canvas, t, self) for t in
            [target.a, target.b, target.c]]
        super(TriangleControl, self).__init__(canvas, target)


    def raise_(self):
        """ Overload raise_ so that points stay above triangle lines.
        """
        super(TriangleControl, self).raise_()
        for p in self.points:   p.raise_()


    def make_mask(self):
        """ Make the triangular mask image.
        """
        painter = QtGui.QPainter()
        bitmap = QtGui.QBitmap(self.size())
        bitmap.clear()

        painter.begin(bitmap)
        self.draw(painter, mask=True)
        painter.end()

        self.setMask(bitmap)


    def mouseDoubleClickEvent(self, event):
        """ On double click, open the central editor
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.open_editor()

    def delete(self, trigger=None):
        """ Overloaded delete (that can be invoked from a child)
            Deletes self and all ChildPointControl objects.
        """
        for p in self.points:
            if p != trigger:
                p.delete()
        super(TriangleControl, self).delete()

    def sync(self):
        for pt in self.points:   pt.sync()

        xmin = min(pt.position.x() for pt in self.points)
        xmax = max(pt.position.x() for pt in self.points)

        ymin = min(pt.position.y() for pt in self.points)
        ymax = max(pt.position.y() for pt in self.points)

        i = self.canvas.mm_to_pixel(x=xmin) - 5
        j = self.canvas.mm_to_pixel(y=ymax) - 5
        di = self.canvas.mm_to_pixel(x=xmax) - i + 10
        dj = self.canvas.mm_to_pixel(y=ymin) - j + 10

        changed = (self.position != QtCore.QPointF(self.node.x, self.node.y) or
                   di != self.width() or dj != self.height() or
                   self.pos() != QtCore.QPoint(i, j))

        self.move(i, j)
        self.resize(di, dj)

        # Cache position here
        self.position = QtCore.QPointF(self.node.x, self.node.y)

        if changed:
            self.make_mask()
            self.update()
        if self.editor:     self.editor.sync()

    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        self.draw(painter)

    def draw(self, painter, mask=False):
        coords = [
                QtCore.QPoint(
                    *self.canvas.mm_to_pixel(pt.position.x(),
                                             pt.position.y())) - self.pos()
                for pt in self.points]

        lines = [QtCore.QLine(coords[0], coords[1]),
                 QtCore.QLine(coords[1], coords[2]),
                 QtCore.QLine(coords[2], coords[0])]

        if mask:
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 4))
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(200, 200, 200), 4))
        painter.drawLines(lines)

from point import ChildPointControl
