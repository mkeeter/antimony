import math

from PySide import QtCore, QtGui

import colors
import base

class ImageControl(base.NodeControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new ImageControl object at the given position.
            image should be a QRegion object.
        """
        filename, filetype = QtGui.QFileDialog.getOpenFileName(
                self.window, "Open", '', "*.png|*.jpg")
        if not filename:    return

        e = Expression()
        for r in image.rects():
            e |= rectangle(r.left(), r.right(), r.bottom(), r.top())
        i = ImageNode(get_name('img'), x, y, image)

        return cls(canvas, i)


    def __init__(self, canvas, target):
        super(ImageControl, self).__init__(canvas, target)

        self.drag_control = base.DragXY(self)
        self.position = QtCore.QPointF()

        self.sync()
        self.editor_datums = ['name','x','y','shape']

        self.size = QtCore.QSize(target.xmax, target.ymax)

        self.show()
        self.raise_()


    def _sync(self):
        p = QtCore.QPointF(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y())
        size = QtCore.QSize(target.xmax, target.ymax)

        changed = (p != self.position) or (size != self.size)

        # Cache these values
        self.position = p
        self.size = size

        return changed

    def reposition(self):
        self.move(self.position)
        self.setFixedSize(self.size)

    def paintEvent(self, painter):
        painter = QtGui.QPainter(self)
        painter.setPen(QtGui.QColor(*colors.blue))
        if self.drag_control.drag or self.drag_control.hover:
            painter.setBrush(QtGui.QColor(*(colors.blue + (150,))))
        else:
            painter.setBrush(QtGui.QColor(*(colors.blue + (100,))))
        painter.drawRect(self.rect())


from node.base import get_name
from node.image import ImageNode

from fab.shapes import rectangle
from fab.expression import Expression

