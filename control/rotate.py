import math

from PySide import QtCore, QtGui

import colors
import base

class RotateZControl(base.NodeControl3D):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        r = RotateZ(get_name('rot'), x, y, z)
        return cls(canvas, r)

    def __init__(self, canvas, target):
        """ Constructs a rotate control widget.
        """
        super(RotateZControl, self).__init__(canvas, target)

        self.drag_control = base.DragXYZ(self)
        self.drag_angle_control = base.DragManager(self, self.drag_angle)

        self.draw_scale = 25

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','input','x','y','angle','shape']


        self.show()
        self.raise_()

    def drag_angle(self, v, p):
        """ Drags the angular handle.
        """
        if not self.node._angle.simple():   return
        self.node._angle.set_expr(str(math.degrees(
            math.atan2(p.y() - self.position.y(), p.x() - self.position.x()))))


    def make_masks(self):
        self.drag_control.mask = self.paint_mask(self.draw_center)
        self.drag_angle_control.mask = self.paint_mask(self.draw_handle)
        self.setMask(self.drag_control.mask |
                     self.drag_angle_control.mask |
                     self.paint_mask(self.draw_axes))

    @property
    def angle(self):
        return self._cache['angle']

    def reposition(self):
        self.setGeometry(self.get_rect(self.axes_path, offset=15))
        self.make_masks()
        self.update()

    def axes_path(self, offset=QtCore.QPoint()):
        pos = self.position
        lines = [pos + QtGui.QVector3D(self.draw_scale, 0, 0),
                 pos,
                 pos + self.draw_scale * QtGui.QVector3D(
                     math.cos(math.radians(self.angle)),
                     math.sin(math.radians(self.angle)),
                     0)]

        arc = []
        a = self.angle if self.angle > 0 else self.angle + 360
        count = int(math.ceil(a / 10))
        step = a / count
        for i in range(count+1):
            arc.append(QtGui.QVector3D(math.cos(math.radians(i*step)),
                                       math.sin(math.radians(i*step)),
                                       0) * self.draw_scale/4 + pos)
        return self.draw_lines([lines, arc], offset)

    def draw_axes(self, painter, mask=False):
        self.set_pen(painter, mask, None, colors.green)
        painter.drawPath(self.axes_path(self.pos()))


    def draw_center(self, painter, mask=False):
        """ Draws a circular handle at this node's position.
        """
        pos = self.canvas.unit_to_pixel(self.position) - self.pos()

        self.set_brush(painter, mask, colors.green)

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 16

        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)


    def draw_handle(self, painter, mask=False):
        """ Draws a small handle at this node's outside.
        """
        pos = self.canvas.unit_to_pixel(self.position + self.draw_scale *
                QtGui.QVector3D(
                     math.cos(math.radians(self.angle)),
                     math.sin(math.radians(self.angle)),
                     0)) - self.pos()
        self.set_brush(painter, mask, colors.green)

        if mask:
            d = 14
        elif self.drag_angle_control.hover or self.drag_angle_control.drag:
            d = 12
        else:
            d = 10

        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.draw_axes(painter)
        self.draw_center(painter)
        self.draw_handle(painter)

from node.base import get_name
from node.rotate import RotateZ
