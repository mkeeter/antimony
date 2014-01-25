import math

from PySide import QtCore, QtGui

import colors
import base

class CylinderControl(base.NodeControl3D):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new cylinder at the given point.
        """
        c = Cylinder(get_name('cyl'), x, y, z, scale, scale)
        return cls(canvas, c)

    def __init__(self, canvas, target):
        """ Create a cylinder control widget.
        """
        super(CylinderControl, self).__init__(canvas, target)

        self.drag_ring_control = base.DragManager(self, self.drag_ring)
        self.drag_height_control = base.DragManager(self, self.drag_height)

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','x','y','z','r','h','shape']

        self.show()
        self.raise_()

    @property
    def height(self):   return self._cache['h']

    @property
    def r(self):
        return self._cache['r']


    def ring_path(self, offset=QtCore.QPoint()):
        """ Draws a pair of rings at the cylinder's top and bottom.
        """
        center = self.position
        points = [center + QtGui.QVector3D(math.cos(i/32.*math.pi*2)*self.r,
                                           math.sin(i/32.*math.pi*2)*self.r, 0)
                  for i in range(33)]
        lines = [
                points,
                [p + QtGui.QVector3D(0, 0, self.height) for p in points]]
        return self.draw_lines(lines, offset)


    def arrow_path(self, offset=QtCore.QPoint()):
        center = self.position
        top = center + QtGui.QVector3D(0, 0, self.height)
        h = min(max(self.height, 15), 35)/8.
        lines = [[center, top],
                 [top + QtGui.QVector3D(-h, 0, -h),
                  top,
                  top + QtGui.QVector3D(h, 0, -h)]]
        return self.draw_lines(lines, offset)


    def draw_arrow(self, painter, mask=False):
        self.set_pen(painter, mask, None, colors.grey)
        painter.drawPath(self.arrow_path(self.pos()))


    def drag_ring(self, v, p):
        """ Drags the ring to expand or contract it.
            p is the drag position.
            v is the drag vector.
        """
        if not self.node._r.simple():   return

        p -= QtGui.QVector3D(self.position)
        p.setZ(0)
        p = p.normalized()

        v.setZ(0)
        dr = QtGui.QVector3D.dotProduct(p, v)
        self.node._r += dr


    def drag_height(self, v, p):
        if not self.node._h.simple():   return
        self.node._h += v.z()


    def make_masks(self):
        """ Render various masks and assign them to the DragManagers.
            (then set the global widget mask)
        """
        self.drag_control.mask = self.paint_mask(self.draw_center)
        self.drag_ring_control.mask = self.paint_mask(self.draw_ring)
        self.drag_height_control.mask = self.paint_mask(self.draw_height)
        self.setMask(self.drag_control.mask |
                     self.drag_ring_control.mask |
                     self.drag_height_control.mask |
                     self.paint_mask(self.draw_arrow))

    def reposition(self):
        """ Repositions the node and calls self.update
        """
        # Get bounding box from painter path
        self.setGeometry(self.get_rect(self.ring_path, offset=15))
        self.make_masks()
        self.update()


    def draw_center(self, painter, mask=False):
        """ Draws a circular point at the center of this widget.
        """
        pos = self.canvas.unit_to_pixel(self.position) - self.pos()

        self.set_brush(painter, mask, colors.grey)

        if mask:                        d = 22
        elif self.drag_control.active:  d = 20
        else:                           d = 16

        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)
        d = 8

        painter.drawEllipse(pos.x() - d/2, pos.y() - d/4 + 2, d, d/2)
        painter.drawEllipse(pos.x() - d/2, pos.y() - d/4 - 2, d, d/2)


    def draw_height(self, painter, mask=False):
        pos = (self.canvas.unit_to_pixel(self.position +
                                         QtGui.QVector3D(0, 0, self.height))
               - self.pos())

        self.set_brush(painter, mask, colors.grey)

        if mask:                                d = 14
        elif self.drag_height_control.active:   d = 12
        else:                                   d = 10

        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)


    def draw_ring(self, painter, mask=False):
        """ Draws the ring around the widget.
        """
        if self.r <= 0:     return
        path = self.ring_path(self.pos())

        self.set_pen(painter, mask, self.drag_ring_control, colors.grey)

        painter.drawPath(path)


    def paintEvent(self, painter):
        painter = QtGui.QPainter(self)
        self.draw_ring(painter)
        self.draw_arrow(painter)
        self.draw_center(painter)
        self.draw_height(painter)

from node.base import get_name
from node.cylinder import Cylinder
