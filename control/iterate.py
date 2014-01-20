import math

from PySide import QtCore, QtGui

import colors
import base

class Array2DControl(base.NodeControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        a = Array2D(get_name('array'), x, y, 1, 1, scale, scale)
        return cls(canvas, a)

    def __init__(self, canvas, target):
        super(Array2DControl, self).__init__(canvas, target)

        self.drag_control = base.DragXY(self)
        self.drag_spacing_control = base.DragManager(self, self.drag_spacing)

        self.position = QtCore.QPointF()
        self.dx = 0
        self.dy = 0
        self.i = 1
        self.j = 1

        self.dx_plus_hover = False
        self.dy_plus_hover = False
        self.dx_minus_hover = False
        self.dy_minus_hover = False

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','input','x','y','i','j','dx','dy','shape']

        self.show()
        self.raise_()

    def drag_spacing(self, v, p):
        """ Drag one of the array nodes to increase or decrease spacing.
        """
        if not self.node._dx.simple() or not self.node._dy.simple():  return
        self.node._dx.set_expr(str(float(self.node._dx.get_expr()) + v.x()))
        self.node._dy.set_expr(str(float(self.node._dy.get_expr()) + v.y()))

    def make_masks(self):
        """ Make masks for dragging the base, dragging the array,
            and pressing any of the buttons.
        """
        self.drag_spacing_control.mask = self.paint_mask(self.draw_array)
        self.drag_control.mask = self.paint_mask(self.draw_center)

        self.dx_plus_mask = self.paint_mask(self.draw_dx_plus)
        self.dy_plus_mask = self.paint_mask(self.draw_dy_plus)
        self.dx_minus_mask = self.paint_mask(self.draw_dx_minus)
        self.dy_minus_mask = self.paint_mask(self.draw_dy_minus)

        self.setMask(self.drag_spacing_control.mask |
                     self.drag_control.mask |
                     self.dx_plus_mask |
                     self.dx_minus_mask |
                     self.dy_plus_mask |
                     self.dy_minus_mask)

    def _sync(self):
        v = QtCore.QPointF(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y())
        dx = self.node.dx if self.node._dx.valid() else self.dx
        dy = self.node.dy if self.node._dy.valid() else self.dy
        i = self.node.i if self.node._i.valid() else self.i
        j = self.node.j if self.node._j.valid() else self.j

        changed = ((v != self.position) or (dx != self.dx) or (dy != self.dy)
                    or (i != self.i) or (j != self.j))

        self.position = v
        self.dx, self.dy = dx, dy
        self.i, self.j = i, j

        return changed


    def reposition(self):
        """ Repositions this widget by calculating its 2D bounding
            box (checking each of the nodes and the +/- buttons).
        """
        pts = [(self.position.x(), self.position.y()),
               (self.position.x() + self.dx*self.i, self.position.y()),
               (self.position.x() + self.dx*self.i,
                   self.position.y() + self.dy*self.j),
               (self.position.x(), self.position.y() + self.dy*self.j)]
        pix = [self.canvas.unit_to_pixel(*pt) for pt in pts]
        pix += [pix[0] + QtCore.QPoint(-22, 0),
                pix[0] + QtCore.QPoint(22, 0),
                pix[0] + QtCore.QPoint(0, -22),
                pix[0] + QtCore.QPoint(0, 22)]
        xmin, xmax = min(p.x() for p in pix), max(p.x() for p in pix)
        ymin, ymax = min(p.y() for p in pix), max(p.y() for p in pix)

        self.setGeometry(xmin - 12, ymin - 12,
                         xmax - xmin + 24, ymax - ymin + 24)

        self.make_masks()
        self.update()


    def draw_center(self, painter, mask=False):
        """ Draws a dot at the center of this object.
        """
        pos = self.canvas.unit_to_pixel(self.position) - self.pos()
        x, y = pos.x(), pos.y()

        self.set_brush(painter, mask, colors.green)

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 16

        painter.drawEllipse(x - d/2, y - d/2, d, d)

    def mouseMoveEvent(self, event):
        """ When the mouse moves, update the plus and minus button
            hover states.
        """
        self.dx_plus_hover = self.dx_plus_mask.contains(event.pos())
        self.dx_minus_hover = self.dx_minus_mask.contains(event.pos())
        self.dy_plus_hover = self.dy_plus_mask.contains(event.pos())
        self.dy_minus_hover = self.dy_minus_mask.contains(event.pos())

        if self.drag_control.hover or self.drag_control.drag:
            self.dx_plus_hover = self.dx_minus_hover = False
            self.dy_plus_hover = self.dy_minus_hover = False
        self.update()

    def mousePressEvent(self, event):
        pass

    def mouseReleaseEvent(self, event):
        """ Increment and decrement array counts.
        """
        if event.button() == QtCore.Qt.LeftButton:
            if self.dx_plus_hover:
                try:
                    i = self.node.i
                    self.node._i.set_expr(str(i + 1))
                except: pass
            elif self.dx_minus_hover:
                try:
                    i = self.node.i
                    if i > 0:   self.node._i.set_expr(str(i - 1))
                except: pass
            elif self.dy_plus_hover:
                try:
                    j = self.node.j
                    self.node._j.set_expr(str(j + 1))
                except: pass
            elif self.dx_minus_hover:
                try:
                    j = self.node.j
                    if j > 0:   self.node._j.set_expr(str(j - 1))
                except: pass
            self.sync()

    def leaveEvent(self, event):
        """ When the mouse leaves, set all hover states to False.
        """
        self.dx_plus_hover = self.dx_minus_hover = False
        self.dy_plus_hover = self.dy_minus_hover = False
        self.update()


    def draw_array(self, painter, mask=False):
        """ Draws an array of small points.
        """
        self.set_brush(painter, mask, colors.green)

        if mask:                                    d = 14
        elif (self.drag_spacing_control.hover or
              self.drag_spacing_control.drag):      d = 12
        else:                                       d = 10

        for i in range(self.i):
            for j in range(self.j):
                if i == j == 0: continue
                pos = self.canvas.unit_to_pixel(self.position +
                        QtCore.QPointF(i*self.dx, j*self.dy)) - self.pos()

                painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)


    def draw_dx_plus(self, painter, mask=False):
        self.set_brush(painter, mask, colors.green)

        if mask:                    d = 22
        elif self.dx_plus_hover:    d = 20
        else:                       d = 16

        pos = (self.canvas.unit_to_pixel(self.position) - self.pos() +
                QtCore.QPoint(22, 0))
        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)
        self.set_pen(painter, mask, None, colors.green)
        painter.drawLine(pos.x() - 4, pos.y(),
                         pos.x() + 4, pos.y())
        painter.drawLine(pos.x(), pos.y() - 4,
                         pos.x(), pos.y() + 4)


    def draw_dx_minus(self, painter, mask=False):
        self.set_brush(painter, mask, colors.green)

        if mask:                    d = 22
        elif self.dx_minus_hover:   d = 20
        else:                       d = 16

        pos = (self.canvas.unit_to_pixel(self.position) - self.pos() -
                QtCore.QPoint(22, 0))
        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)
        self.set_pen(painter, mask, None, colors.green)
        painter.drawLine(pos.x() - 4, pos.y(),
                         pos.x() + 4, pos.y())

    def draw_dy_plus(self, painter, mask=False):
        self.set_brush(painter, mask, colors.green)

        if mask:                    d = 22
        elif self.dy_plus_hover:    d = 20
        else:                       d = 16

        pos = (self.canvas.unit_to_pixel(self.position) - self.pos() -
                QtCore.QPoint(0, 22))
        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)
        self.set_pen(painter, mask, None, colors.green)
        painter.drawLine(pos.x() - 4, pos.y(),
                         pos.x() + 4, pos.y())
        painter.drawLine(pos.x(), pos.y() - 4,
                         pos.x(), pos.y() + 4)


    def draw_dy_minus(self, painter, mask=False):
        self.set_brush(painter, mask, colors.green)

        if mask:                    d = 22
        elif self.dy_minus_hover:   d = 20
        else:                       d = 16

        pos = (self.canvas.unit_to_pixel(self.position) - self.pos() +
                QtCore.QPoint(0, 22))
        painter.drawEllipse(pos.x() - d/2, pos.y() - d/2, d, d)
        self.set_pen(painter, mask, None, colors.green)
        painter.drawLine(pos.x() - 4, pos.y(),
                         pos.x() + 4, pos.y())



    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.draw_array(painter)
        self.draw_center(painter)
        self.draw_dx_plus(painter)
        self.draw_dx_minus(painter)
        self.draw_dy_plus(painter)
        self.draw_dy_minus(painter)

from node.base import get_name
from node.iterate import Array2D
