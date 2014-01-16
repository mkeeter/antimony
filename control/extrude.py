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

        self.position = QtGui.QVector3D()
        self.top = 0

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

    def _sync(self):
        v = QtGui.QVector3D(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y(),
                self.node.z if self.node._z.valid() else self.position.z())
        top = self.node.top if self.node._top.valid() else self.top

        changed = (top != self.top) or (v != self.position)

        self.position = v
        self.top = top

        return changed

    def base_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws a base for the extrusion
            and a line pointing upwards to the top.
        """
        scale = max(0.08 / self.canvas.scale,
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
        center = QtGui.QVector3D(
                self.position.x(), self.position.y(), self.top)
        scale = max(0.06 / self.canvas.scale,
                (self.top - self.position.z()) / 15.)
        line = [
                center + QtGui.QVector3D(-scale, 0, -scale),
                center,
                center + QtGui.QVector3D(scale, 0, -scale)]
        return self.draw_lines([line], offset)

    def reposition(self):
        """ Repositions this widget and calls self.update
        """
        p = self.base_path()
        p.connectPath(self.top_path())

        rect = p.boundingRect().toRect()
        rect.setTop(rect.top() - 15)
        rect.setBottom(rect.bottom() + 15)
        rect.setLeft(rect.left() - 15)
        rect.setRight(rect.right() + 15)

        self.setGeometry(rect)

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
        for n in 'base','top':
            func = getattr(self, 'draw_'+n)
            painter = QtGui.QPainter()
            bitmap = QtGui.QBitmap(self.size())
            bitmap.clear()

            painter.begin(bitmap)
            func(painter, True)
            painter.end()

            setattr(self, n+'_mask', QtGui.QRegion(bitmap))

        self.drag_control.mask = self.base_mask
        self.drag_top_control.mask = self.top_mask

        self.setMask(self.base_mask | self.top_mask)


    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        self.draw_base(painter)
        self.draw_top(painter)

from node.extrude import ExtrudeZ
from node.base import get_name
