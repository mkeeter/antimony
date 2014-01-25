import math

from PySide import QtCore, QtGui

import colors
import base

class RepositionControl(base.NodeControl3D):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        r = Reposition(get_name('re'), x, y, z)
        return cls(canvas, r)

    def __init__(self, canvas, target):
        super(RepositionControl, self).__init__(canvas, target)

        self.draw_scale = 10

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','input','x','y','z','shape']

        self.show()
        self.raise_()

    def wireframe_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws this shape's bounding cube
            as a wireframe (stolen from Cube control).
        """
        shape = self._cache['shape']
        x0, y0, z0 = shape.xmin, shape.ymin, shape.zmin
        x1, y1, z1 = shape.xmax, shape.ymax, shape.zmax

        # Properly handle 2D shapes
        if math.isinf(z0):   z0 = 0
        if math.isinf(z1):   z1 = 0

        v = QtGui.QVector3D
        return self.draw_lines([
            [v(x0, y0, z0), v(x0, y0, z1), v(x0, y1, z1),
                v(x0, y1, z0), v(x0, y0, z0)],
            [v(x1, y0, z0), v(x1, y0, z1), v(x1, y1, z1),
                v(x1, y1, z0), v(x1, y0, z0)],
            [v(x0, y0, z0), v(x1, y0, z0)],
            [v(x0, y1, z0), v(x1, y1, z0)],
            [v(x0, y0, z1), v(x1, y0, z1)],
            [v(x0, y1, z1), v(x1, y1, z1)]], offset)

    def draw_wireframe(self, painter, mask=False):
        """ Draws this shape's bounding box as a dotted line.
        """
        self.set_pen(painter, mask, None, colors.green)
        p = painter.pen()
        p.setStyle(QtCore.Qt.DotLine)
        painter.setPen(p)
        painter.drawPath(self.wireframe_path(self.pos()))


    def draw_handle(self, painter, mask=False):
        self.set_brush(painter, mask, colors.green)
        pos = self.canvas.unit_to_pixel(self.position) - self.pos()

        if mask:                        d = 22
        elif self.drag_control.active:  d = 20
        else:                           d = 16

        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)
        self.set_pen(painter, mask, None, colors.green)
        painter.drawLine(pos.x(), pos.y() - 6, pos.x(), pos.y())
        painter.drawLine(pos.x(), pos.y(), pos.x() + 6, pos.y())
        painter.drawLine(pos.x(), pos.y(), pos.x() - 4, pos.y() + 4)


    def reposition(self):
        self.setGeometry(self.get_rect(self.wireframe_path, offset=15))
        self.make_masks()
        self.update()

    def make_masks(self):
        self.drag_control.mask = self.paint_mask(self.draw_handle)
        self.setMask(self.drag_control.mask |
                     self.paint_mask(self.draw_wireframe))

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.draw_wireframe(painter)
        self.draw_handle(painter)

from node.base import get_name
from node.reposition import Reposition

