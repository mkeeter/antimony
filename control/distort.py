import math

from PySide import QtCore, QtGui

import colors
import base

class DistortControl(base.NodeControl):
    def __init__(self, canvas, target):
        super(DistortControl, self).__init__(canvas, target)

        self.drag_control = base.DragXYZ(self)
        self.radius_drag = base.DragManager(self, self.drag_radius)

        self.position = QtGui.QVector3D()
        self.r = 0

        self.editor_datums = ['name','input','x','y','z','r','shape']

        self.sync()
        self.make_masks()

        self.show()
        self.raise_()


    def _sync(self):
        """ Move and scale this control to the appropriate position.
            Use self.position and self.r if eval fails.
        """
        p = QtGui.QVector3D(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y(),
                self.node.z if self.node._z.valid() else self.position.y())

        r = self.node.r if self.node._r.valid() else self.r

        # Figure out if these fundamental values have changed
        changed = (self.position != p) or (self.r != r)

        # Cache these values
        self.position = p
        self.r = r

        return changed


    def reposition(self):
        self.setGeometry(self.get_rect(self.wireframe_path, offset=15))
        self.make_masks()
        self.update()


    def make_masks(self):
        self.drag_control.mask = self.paint_mask(self.draw_center)
        self.radius_drag.mask = self.paint_mask(self.draw_handles)
        self.setMask(self.drag_control.mask |
                     self.radius_drag.mask |
                     self.paint_mask(self.draw_wireframe))


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
        """ Draws paths for the main axes.
        """
        lines = [[self.position - QtGui.QVector3D(self.r, 0, 0),
                  self.position + QtGui.QVector3D(self.r, 0, 0)],
                 [self.position - QtGui.QVector3D(0, self.r, 0),
                  self.position + QtGui.QVector3D(0, self.r, 0)],
                 [self.position - QtGui.QVector3D(0, 0, self.r),
                  self.position + QtGui.QVector3D(0, 0, self.r)]]
        return self.draw_lines(lines, offset)


    def draw_handles(self, painter, mask=False):
        """ Draws circular handles at the ends of all the axes.
        """
        self.set_brush(painter, mask, colors.orange)

        if mask:                                                    d = 14
        elif self.radius_drag.hover or self.radius_drag.drag:       d = 12
        else:                                                       d = 10

        pts = [self.position - QtGui.QVector3D(self.r, 0, 0),
               self.position + QtGui.QVector3D(self.r, 0, 0),
               self.position - QtGui.QVector3D(0, self.r, 0),
               self.position + QtGui.QVector3D(0, self.r, 0),
               self.position - QtGui.QVector3D(0, 0, self.r),
               self.position + QtGui.QVector3D(0, 0, self.r)]

        for pt in pts:
            p = self.canvas.unit_to_pixel(pt) - self.pos()
            painter.drawEllipse(p.x() - d/2, p.y() - d/2, d, d)



    def draw_wireframe(self, painter, mask=False):
        self.set_pen(painter, mask, None, colors.orange)
        painter.drawPath(self.wireframe_path(self.pos()))

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.draw_wireframe(painter)
        self.draw_handles(painter)
        self.draw_center(painter)

################################################################################

class AttractControl(DistortControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        a = Attract(get_name('attract'), x, y, z, scale)
        return cls(canvas, a)


class RepelControl(DistortControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        r = Repel(get_name('repel'), x, y, z, scale)
        return cls(canvas, r)

from node.distort import Attract, Repel
from node.base import get_name
