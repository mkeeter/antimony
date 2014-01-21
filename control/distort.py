import math

from PySide import QtCore, QtGui

import colors
import base

class DistortControl(base.NodeControl):
    def __init__(self, canvas, target):
        super(DistortControl, self).__init__(canvas, target)

        self.drag_control = base.DragXYZ(self)
        self.radius_drag = base.DragManager(self, self.drag_radius)

        self.editor_datums = ['name','input','x','y','z','r','shape']

        self.sync()
        self.make_masks()

        self.show()
        self.raise_()

    @property
    def position(self):
        return QtGui.QVector3D(self._cache['x'],
                               self._cache['y'],
                               self._cache['z'])

    @property
    def r(self):    return self._cache['r']


    def reposition(self):
        self.setGeometry(self.get_rect(self.wireframe_path, offset=15))
        self.make_masks()
        self.update()


    def make_masks(self):
        self.drag_control.mask = self.paint_mask(self.draw_center)
        self.radius_drag.mask = self.paint_mask(self.draw_wireframe)
        self.setMask(self.drag_control.mask |
                     self.radius_drag.mask |
                     self.paint_mask(self.draw_wireframe) |
                     self.paint_mask(self.draw_arrows))


    def drag_radius(self, v, p):
        """ Drags the ring to expand or contract it.
            p is the drag position.
            v is the drag vector.
        """
        if not self.node._r.simple():   return

        p -= self.position
        p = p.normalized()

        dr = QtGui.QVector3D.dotProduct(p, v)
        self.node._r.set_expr(str(float(self.node._r.get_expr()) + dr))

    def draw_center(self, painter, mask=False):
        """ Draws a circle at the center of the widget.
        """
        p = self.canvas.unit_to_pixel(self.position) - self.pos()

        self.set_brush(painter, mask, colors.orange)

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 16

        painter.drawEllipse(p.x() - d/2, p.y() - d/2, d, d)


    def wireframe_path(self, offset=QtCore.QPoint()):
        """ Draws a circle, projected to be facing the camera.
        """
        m = QtGui.QMatrix4x4()
        m.rotate(-math.degrees(self.canvas.yaw), QtGui.QVector3D(0, 0, 1))
        m.rotate(-math.degrees(self.canvas.pitch), QtGui.QVector3D(1, 0, 0))
        lines = [m * QtGui.QVector3D(math.cos(i/32.*math.pi*2)*self.r,
                                     math.sin(i/32.*math.pi*2)*self.r, 0)
                + self.position
                 for i in range(33)]
        return self.draw_lines([lines], offset)


    def draw_wireframe(self, painter, mask=False):
        self.set_pen(painter, mask, self.radius_drag, colors.orange)
        painter.drawPath(self.wireframe_path(self.pos()))


    def draw_arrows(self, painter, mask=False):
        self.set_pen(painter, mask, None, colors.orange)
        painter.drawPath(self.arrow_path(self.pos()))


    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.draw_wireframe(painter)
        self.draw_arrows(painter)
        self.draw_center(painter)


################################################################################

class AttractControl(DistortControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        a = Attract(get_name('attract'), x, y, z, scale)
        return cls(canvas, a)

    def arrow_path(self, offset=QtCore.QPoint()):
        path = QtGui.QPainterPath()
        r = self.r / 2.
        s = self.r / 6.
        v = QtGui.QVector3D
        pts = [
                [v(self.r, 0, 0), v(r, 0, 0)],
                [v(r+s, s, 0), v(r, 0, 0), v(r+s, -s, 0)],
                [v(0, self.r, 0), v(0, r, 0)],
                [v(s, r+s, 0), v(0, r, 0), v(-s, r+s, 0)],
                [v(0, 0, self.r), v(0, 0, r)],
                [v(s, 0, r+s), v(0, 0, r), v(-s, 0, r+s)]]
        for arrow in pts:
            path.addPath(
                self.draw_lines([[self.position - p for p in arrow]], offset))
            path.addPath(
                self.draw_lines([[self.position + p for p in arrow]], offset))

        return path




class RepelControl(DistortControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        r = Repel(get_name('repel'), x, y, z, scale)
        return cls(canvas, r)

    def arrow_path(self, offset=QtCore.QPoint()):
        path = QtGui.QPainterPath()
        r = self.r / 1.3
        s = self.r / 6.
        v = QtGui.QVector3D
        pts = [
                [v(0, 0, 0), v(r, 0, 0)],
                [v(r-s, s, 0), v(r, 0, 0), v(r-s, -s, 0)],
                [v(0, 0, 0), v(0, r, 0)],
                [v(s, r-s, 0), v(0, r, 0), v(-s, r-s, 0)],
                [v(0, 0, 0), v(0, 0, r)],
                [v(s, 0, r-s), v(0, 0, r), v(-s, 0, r-s)]]
        for arrow in pts:
            path.addPath(
                self.draw_lines([[self.position - p for p in arrow]], offset))
            path.addPath(
                self.draw_lines([[self.position + p for p in arrow]], offset))

        return path

from node.distort import Attract, Repel
from node.base import get_name
