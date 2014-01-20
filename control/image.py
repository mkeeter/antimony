import math

from PySide import QtCore, QtGui

import colors
import base

class ImageControl(base.NodeControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new ImageControl object at the given position.
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
        self.drag_scale_control = base.DragManager(self, self.drag_scale)
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
            scale = self.node.scale
        else:
            size = self.imgsize
            scale = self.imgscale

        changed = (p != self.position) or (size != self.imgsize)

        # Cache these values
        self.position = p
        self.imgscale = scale
        self.imgsize = size

        return changed

    def reposition(self):
        self.setGeometry(self.get_rect(self.outline_path, offset=15))
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
        self.set_pen(painter, mask, None, colors.grey)
        p = painter.pen()
        p.setStyle(QtCore.Qt.DotLine)
        painter.setPen(p)
        painter.drawPath(self.outline_path(self.pos()))

    def draw_base(self, painter, mask=False):
        self.set_brush(painter, mask, colors.grey)
        pt = self.canvas.unit_to_pixel(self.position) - self.pos()

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 16

        painter.drawEllipse(pt.x() - d/2, pt.y() - d/2, d, d)

    def draw_scale(self, painter, mask=False):
        self.set_brush(painter, mask, colors.grey)
        pt = self.canvas.unit_to_pixel(
                self.position.x() + self.imgsize.width(),
                self.position.y() + self.imgsize.height()) - self.pos()

        if mask:
            d = 20
        elif self.drag_scale_control.hover or self.drag_scale_control.drag:
            d = 18
        else:
            d = 14

        painter.drawEllipse(pt.x() - d/2, pt.y() - d/2, d, d)

        s = d/(2*math.sqrt(2)) - 2
        painter.drawLine(pt.x() - s, pt.y() + s, pt.x() + s, pt.y() - s)

    def drag_scale(self, v, p):
        if not self.node._scale.simple():   return

        w = self.imgsize.width() / self.imgscale
        h = self.imgsize.height() / self.imgscale

        self.node._scale.set_expr(str(max(1 / min(w,h), min(
            (self.imgsize.width() + v.x()) / w,
            (self.imgsize.height() + v.y()) / h))))

    def make_mask(self):
        """ Make the wireframe mask image.
        """
        self.drag_control.mask = self.paint_mask(self.draw_base)
        self.drag_scale_control.mask = self.paint_mask(self.draw_scale)
        self.setMask(self.drag_control.mask |
                     self.drag_scale_control.mask |
                     self.paint_mask(self.draw_outline))

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.draw_outline(painter)
        self.draw_base(painter)
        self.draw_scale(painter)


from node.base import get_name
from node.image import ImageNode

from fab.shapes import rectangle
from fab.expression import Expression

