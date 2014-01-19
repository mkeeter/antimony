import math

from PySide import QtCore, QtGui

import colors
import base

class TextControl(base.NodeControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        t = Text(get_name('txt'), x, y, scale, 'hello')
        return cls(canvas, t)

    def __init__(self, canvas, target):
        super(TextControl, self).__init__(canvas, target)

        self.drag_control = base.DragXY(self)
        self.drag_scale_control = base.DragManager(self, self.drag_scale)

        self.bbox = QtCore.QRectF()
        self.scale = 0

        self.editor_datums = ['name','x','y','scale','txt','shape']

        self.sync()
        self.show()
        self.raise_()


    def editor_position(self):
        return self.canvas.unit_to_pixel(self.bbox.left(), self.bbox.top())

    def _sync(self):
        if self.node._shape.valid() and self.node.shape.has_xy_bounds():
            s = self.node.shape
            bbox = QtCore.QRectF(s.xmin, s.ymin,
                                 s.xmax - s.xmin, s.ymax - s.ymin)
        else:
            bbox = self.bbox

        s = self.node.scale if self.node._scale.valid() else self.scale

        changed = (bbox != self.bbox) or (s != self.scale)

        self.bbox = bbox
        self.scale = s

        return changed


    def outline_path(self, offset=QtCore.QPoint()):
        """ Rectangular path tracing out bounding box of text.
        """
        coords = [
                QtGui.QVector3D(self.bbox.left(), self.bbox.bottom(), 0),
                QtGui.QVector3D(self.bbox.right(), self.bbox.bottom(), 0),
                QtGui.QVector3D(self.bbox.right(), self.bbox.top(), 0),
                QtGui.QVector3D(self.bbox.left(), self.bbox.top(), 0),
                QtGui.QVector3D(self.bbox.left(), self.bbox.bottom(), 0)]
        return self.draw_lines([coords], offset)


    def reposition(self):
        self.setGeometry(self.get_rect(self.outline_path, offset=15))
        self.make_mask()
        self.update()


    def draw_outline(self, painter, mask=False):
        """ Draws this text block's 2D boundary on the floor.
        """
        self.set_pen(painter, mask, None, colors.green)
        painter.drawPath(self.outline_path(self.pos()))


    def draw_base(self, painter, mask=False):
        """ Draws a simple circle at the lower-left of this text.
        """
        self.set_brush(painter, mask, colors.green)
        pt = self.canvas.unit_to_pixel(self.bbox.left(),
                                       self.bbox.top()) - self.pos()

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 16

        painter.drawEllipse(pt.x() - d/2, pt.y() - d/2, d, d)


    def draw_scale(self, painter, mask=False):
        """ Draws a slightly smaller, slashed circle at the top right of
            the text's bounding box.
        """
        self.set_brush(painter, mask, colors.green)
        pt = self.canvas.unit_to_pixel(self.bbox.right(),
                                       self.bbox.bottom()) - self.pos()

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
        """ Modifies the text scale.
        """
        if not self.node._scale.simple():   return

        w = self.bbox.width() / self.scale
        h = self.bbox.height() / self.scale

        self.node._scale.set_expr(str(max(1 / min(w,h), min(
            (self.bbox.width() + v.x()) / w,
            (self.bbox.height() + v.y()) / h))))


    def make_mask(self):
        """ Make the wireframe mask image and overall widget mask.
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

from node.text import Text
from node.base import get_name
