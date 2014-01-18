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
                canvas, "Open", '', "Images (*.png *.jpg *.gif)")
        if not filename:    return

        # Import an image and convert it into a QRegion
        img = QtGui.QImage(filename)
        bmp = QtGui.QRegion(QtGui.QPixmap(img.createHeuristicMask()))

        e = Expression(None)
        for r in bmp.rects():
            e |= rectangle(r.left(), r.right() + 1.01, r.bottom(), r.top() + 1.01)
        i = ImageNode(get_name('img'), x, y, e, img.width(), img.height())

        return cls(canvas, i)


    def __init__(self, canvas, target):
        super(ImageControl, self).__init__(canvas, target)

        self.drag_control = base.DragXY(self)
        self.position = QtCore.QPointF()

        self.sync()
        self.editor_datums = ['name','x','y','shape']

        self.size = QtCore.QSize()

        self.show()
        self.raise_()


    def _sync(self):
        p = QtCore.QPointF(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y())

        if self.node._shape.valid():
            size = QtCore.QSize(self.node.shape.xmax, self.node.shape.ymax)
        else:
            size = self.size

        changed = (p != self.position) or (size != self.size)

        # Cache these values
        self.position = p
        self.size = size

        return changed

    def reposition(self):
        self.move(self.canvas.unit_to_pixel(self.position))
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

