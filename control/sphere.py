import math

from PySide import QtCore, QtGui

import colors
import base

class SphereControl(base.NodeControl3D):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new sphere at the given point.
        """
        s = Sphere(get_name('s'), x, y, z, scale)
        return cls(canvas, s)

    def __init__(self, canvas, target):
        """ Constructs a sphere control widget.
        """
        super(SphereControl, self).__init__(canvas, target)

        self.drag_control = base.DragXYZ(self)
        self.radius_drag = base.DragManager(self, self.drag_radius)

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','x','y','z','r','shape']

        self.show()
        self.raise_()

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

    def make_masks(self):
        self.drag_control.mask = self.paint_mask(self.draw_center)
        self.radius_drag.mask = self.paint_mask(self.draw_wireframe)
        self.setMask(self.drag_control.mask |
                     self.radius_drag.mask)


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

    @property
    def r(self):    return self._cache['r']


    def reposition(self):
        # Check whether any render information has changed.
        self.setGeometry(self.get_rect(self.wireframe_path))

        self.make_masks()
        self.update()

    def draw_center(self, painter, mask=False):
        """ Draws a circle at the center of the widget.
        """
        width, height = self.width(), self.height()

        self.set_brush(painter, mask, colors.grey)

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 16

        painter.drawEllipse((width - d) / 2, (height - d) / 2, d, d)
        d = 8
        painter.drawEllipse((width - d) / 2, (height - d) / 2, d, d)


    def draw_wireframe(self, painter, mask=False):
        """ Draw the wireframe outline of a sphere.
        """
        if self.r <= 0:     return
        path = self.wireframe_path(self.pos())

        if mask:    r = 6
        else:       r = 2

        self.set_pen(painter, mask, self.radius_drag, colors.grey)

        painter.drawPath(path)

    def paintEvent(self, event):
        """ Draw the sphere's center and wireframe.
        """
        painter = QtGui.QPainter(self)
        self.draw_center(painter)
        self.draw_wireframe(painter)

from node.base import get_name
from node.sphere import Sphere
