import math

from PySide import QtCore, QtGui

import colors
import base

class ExtrudeZControl(base.NodeControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new z extrusion modifier at the given point.
        """
        e = ExtrudeZ(get_name('z'), x, y, 0, scale*2)
        return cls(canvas, e)

    def __init__(self, canvas, target):
        """ Constructs a z extrusion control widget
        """
        super(ExtrudeZControl, self).__init__(canvas, target)

        self.drag_control = base.DragXYZ(self)
        self.drag_top_control = base.DragManager(self, self.drag_top)

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','input','z','top','shape']

        self.show()
        self.raise_()

    def drag_top(self, v, p):
        """ Drags the top of the extrusion around.
        """
        if not self.node._top.simple(): return
        self.node._top.set_expr(str(float(self.node._top.get_expr()) +
                                    v.z()))

    @property
    def position(self):
        return QtGui.QVector3D(self._cache['x'],
                               self._cache['y'],
                               self._cache['z'])

    @property
    def top(self):  return self._cache['top']


    def base_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws a base for the extrusion
            and a line pointing upwards to the top.
        """
        scale = max(
                0.08 / self.canvas.scale,
                (self.top - self.position.z())/10.)

        lines = [[
                self.position + QtGui.QVector3D(-scale, 0, 0),
                self.position,
                self.position + QtGui.QVector3D(scale, 0, 0)
                ],[
                self.position + QtGui.QVector3D(0, -scale, 0),
                self.position,
                self.position + QtGui.QVector3D(0, scale, 0)
                ],[
                self.position,
                QtGui.QVector3D(self.position.x(), self.position.y(), self.top)
        ]]
        return self.draw_lines(lines, offset)


    def top_path(self, offset=QtCore.QPoint()):
        """ Draws a pointy arrow at the top of the extrude shape.
        """
        scale = max(
                0.06 / self.canvas.scale,
                (self.top - self.position.z()) / 15.)

        center = QtGui.QVector3D(
                self.position.x(), self.position.y(), self.top)
        line = [
                center + QtGui.QVector3D(-scale, 0, -scale),
                center,
                center + QtGui.QVector3D(scale, 0, -scale)]
        return self.draw_lines([line], offset)

    def full_path(self):
        p = self.base_path()
        p.connectPath(self.top_path())
        return p

    def reposition(self):
        """ Repositions this widget and calls self.update
        """
        self.setGeometry(self.get_rect(self.full_path, offset=15))
        self.make_masks()
        self.update()

    def draw_base(self, painter, mask=False):
        painter.setBrush(QtGui.QBrush())
        if mask:
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 8))
        elif self.drag_control.hover or self.drag_control.drag:
            painter.setPen(QtGui.QPen(QtGui.QColor(
                *colors.highlight(colors.orange)), 4))
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.orange), 4))
        painter.drawPath(self.base_path(self.pos()))

    def draw_top(self, painter, mask=False):
        painter.setBrush(QtGui.QBrush())
        if mask:
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 8))
        elif self.drag_top_control.hover or self.drag_top_control.drag:
            painter.setPen(QtGui.QPen(QtGui.QColor(
                *colors.highlight(colors.orange)), 4))
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.orange), 4))
        painter.drawPath(self.top_path(self.pos()))

    def make_masks(self):
        self.drag_control.mask = self.paint_mask(self.draw_base)
        self.drag_top_control.mask = self.paint_mask(self.draw_top)

        self.setMask(self.drag_control.mask | self.drag_top_control.mask)


    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        self.draw_base(painter)
        self.draw_top(painter)

from node.extrude import ExtrudeZ
from node.base import get_name
