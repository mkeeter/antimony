import math

from PySide import QtCore, QtGui

import colors
import base

class ScaleControl(base.NodeControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new scale modifier at the given point.
            position and scale should be given in units.
        """
        s = Scale(get_name('s'), x, y, z, 1, 1, 1)
        return cls(canvas, s)

    def __init__(self, canvas, target):
        """ Construct a circle control widget.
        """
        super(ScaleControl, self).__init__(canvas, target)

        self.drag_control = base.DragXYZ(self)
        self.scale_x = base.DragManager(self, self.drag_x)
        self.scale_y = base.DragManager(self, self.drag_y)
        self.scale_z = base.DragManager(self, self.drag_z)

        # Cached values (used if the node's values are invalid)
        self.position = QtGui.QVector3D()
        self.scale = QtGui.QVector3D()

        self.draw_scale = 25

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','input','x','y','z','sx','sy','sz','shape']

        self.show()
        self.raise_()

    def drag_x(self, v, p):
        if not self.node._sx.simple():  return
        self.node._sx.set_expr(str(float(self.node._sx.get_expr()) +
                                         v.x()/self.draw_scale))

    def drag_y(self, v, p):
        if not self.node._sy.simple():  return
        self.node._sy.set_expr(str(float(self.node._sy.get_expr()) +
                                         v.y()/self.draw_scale))

    def drag_z(self, v, p):
        if not self.node._sz.simple():  return
        self.node._sz.set_expr(str(float(self.node._sz.get_expr()) +
                                         v.z()/self.draw_scale))


    def make_masks(self):
        for n in ['x_handle','y_handle','z_handle','center','axes']:
            func = getattr(self, 'draw_' + n)
            painter = QtGui.QPainter()
            bitmap = QtGui.QBitmap(self.size())
            bitmap.clear()

            painter.begin(bitmap)
            func(painter, True)
            painter.end()

            setattr(self, n + '_mask', QtGui.QRegion(bitmap))
        self.scale_x.mask = self.x_handle_mask
        self.scale_y.mask = self.y_handle_mask
        self.scale_z.mask = self.z_handle_mask
        self.drag_control.mask = self.center_mask

        self.setMask(self.x_handle_mask |
                     self.y_handle_mask |
                     self.z_handle_mask |
                     self.center_mask   |
                     self.axes_mask)

    def _sync(self):
        v = QtGui.QVector3D(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y(),
                self.node.z if self.node._z.valid() else self.position.z())
        s = QtGui.QVector3D(
                self.node.sx if self.node._sx.valid() else self.scale.x(),
                self.node.sy if self.node._sy.valid() else self.scale.y(),
                self.node.sz if self.node._sz.valid() else self.scale.z())
        changed = (self.position != v) or (self.scale != s)

        self.position = v
        self.scale = s

        return changed

    def reposition(self):
        """ Repositions this widget and calls self.update
        """
        p = self.axes_path()
        p.connectPath(self.x_handle_path())
        p.connectPath(self.y_handle_path())
        p.connectPath(self.z_handle_path())

        rect = p.boundingRect().toRect()
        rect.setTop(rect.top() - 15)
        rect.setBottom(rect.bottom() + 15)
        rect.setLeft(rect.left() - 15)
        rect.setRight(rect.right() + 15)

        self.setGeometry(rect)

        self.make_masks()
        self.update()

    def x_handle_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws the x arrow.
        """
        s = self.scale.x() * self.draw_scale
        center = self.position + QtGui.QVector3D(s, 0, 0)
        points = [
            center + 0.1*QtGui.QVector3D(-s, s, 0),
            center,
            center + 0.1*QtGui.QVector3D(-s, -s, 0)]
        return self.draw_lines([points], offset)

    def y_handle_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws the x arrow.
        """
        s = self.scale.y() * self.draw_scale
        center = self.position + QtGui.QVector3D(0, s, 0)
        points = [
            center + 0.1*QtGui.QVector3D(-s, -s, 0),
            center,
            center + 0.1*QtGui.QVector3D(s, -s, 0)]
        return self.draw_lines([points], offset)


    def z_handle_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws the x arrow.
        """
        s = self.scale.z() * self.draw_scale
        center = self.position + QtGui.QVector3D(0, 0, s)
        points = [
            center + 0.1*QtGui.QVector3D(-s, 0, -s),
            center,
            center + 0.1*QtGui.QVector3D(s, 0, -s)]
        return self.draw_lines([points], offset)

    def axes_path(self, offset=QtCore.QPoint()):
        lines = [
                [self.position, self.position +
                        self.draw_scale*QtGui.QVector3D(0, 0, self.scale.z())],
                [self.position, self.position +
                        self.draw_scale*QtGui.QVector3D(self.scale.x(), 0, 0)],
                [self.position, self.position +
                        self.draw_scale*QtGui.QVector3D(0, self.scale.y(), 0)]]
        return self.draw_lines(lines, offset)

    def draw_x_handle(self, painter, mask=False):
        self.set_line(painter, mask, self.scale_x, colors.orange)
        painter.drawPath(self.x_handle_path(self.pos()))

    def draw_y_handle(self, painter, mask=False):
        self.set_line(painter, mask, self.scale_y, colors.orange)
        painter.drawPath(self.y_handle_path(self.pos()))

    def draw_z_handle(self, painter, mask=False):
        self.set_line(painter, mask, self.scale_z, colors.orange)
        painter.drawPath(self.z_handle_path(self.pos()))


    def draw_axes(self, painter, mask=False):
        self.set_line(painter, mask, None, colors.orange)
        painter.drawPath(self.axes_path(self.pos()))


    def draw_center(self, painter, mask=False):
        """ Draws a dot at the center of this scale object.
        """
        pos = self.canvas.unit_to_pixel(self.position) - self.pos()
        x, y = pos.x(), pos.y()

        self.set_brush(painter, mask, colors.orange)

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 16

        painter.drawEllipse(x - d/2, y - d/2, d, d)


    def paintEvent(self, paintEvent):
        """ Paints this widget's lines.
        """
        painter = QtGui.QPainter(self)
        self.draw_axes(painter)
        self.draw_center(painter)
        self.draw_x_handle(painter)
        self.draw_y_handle(painter)
        self.draw_z_handle(painter)

from node.scale import Scale
from node.base import get_name
