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
        bmp = QtGui.QRegion(QtGui.QPixmap(img.createMaskFromColor(0xffffffff)))

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
        self.editor_datums = ['name','x','y','scale','shape']

        self.imgsize = QtCore.QSize()

        self.show()
        self.raise_()


    def _sync(self):
        p = QtCore.QPointF(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y())

        if self.node._scale.valid():
            size = QtCore.QSize(
                    self.node.w * self.node.scale if self.node._w.valid()
                                else self.imgsize.width(),
                    self.node.h * self.node.scale if self.node._h.valid()
                                else self.imgsize.height())
        else:
            size = self.imgsize

        changed = (p != self.position) or (size != self.imgsize)

        # Cache these values
        self.position = p
        self.imgsize = size

        return changed

    def reposition(self):
        rect = self.outline_path().boundingRect().toRect()
        rect.setTop(rect.top() - 5)
        rect.setBottom(rect.bottom() + 5)
        rect.setLeft(rect.left() - 5)
        rect.setRight(rect.right() + 5)

        self.setGeometry(rect)
        self.make_mask()
        self.update()

    def outline_path(self, offset=QtCore.QPoint()):
        """ Draws a path that outlines this image.
        """
        coords = [
                QtGui.QVector3D(self.position.x(), self.position.y(), 0),
                QtGui.QVector3D(self.position.x() + self.imgsize.width(),
                                self.position.y(), 0),
                QtGui.QVector3D(self.position.x() + self.imgsize.width(),
                                self.position.y() + self.imgsize.height(), 0),
                QtGui.QVector3D(self.position.x(),
                                self.position.y() + self.imgsize.height(), 0),
                QtGui.QVector3D(self.position.x(), self.position.y(), 0)]
        return self.draw_lines([coords], offset)


    def draw_outline(self, painter, mask=False):
        """ Draws this image's 2D boundary on the floor.
        """
        if mask:
            painter.setPen(QtCore.Qt.color1)
        else:
            painter.setPen(QtGui.QColor(*colors.green))

        if mask:
            painter.setBrush(QtCore.Qt.color1)
        elif self.drag_control.drag or self.drag_control.hover:
            painter.setBrush(QtGui.QColor(*(colors.green + (100,))))
        else:
            painter.setBrush(QtGui.QColor(*(colors.green + (50,))))
        painter.drawPath(self.outline_path(self.pos()))

    def make_mask(self):
        """ Make the wireframe mask image.
        """
        self.drag_control.mask = self.paint_mask(self.draw_outline)
        self.setMask(self.drag_control.mask)

    def paintEvent(self, event):
        self.draw_outline(QtGui.QPainter(self))


from node.base import get_name
from node.image import ImageNode

from fab.shapes import rectangle
from fab.expression import Expression

