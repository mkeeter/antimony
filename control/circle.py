import math

from PySide import QtCore, QtGui

import colors
import base

class CircleControl(base.NodeControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new circle at the given point.
            position and scale should be given in units.
        """
        c = Circle(get_name('c'), x, y, scale)
        return cls(canvas, c)

    def __init__(self, canvas, target):
        """ Construct a circle control widget.
        """
        super(CircleControl, self).__init__(canvas, target)

        self.drag_control = base.DragXY(self)
        self.ring_drag_control = base.DragManager(self, self.drag_ring)

        # Cached values (used if the node's values are invalid)
        self.position = QtCore.QPointF()
        self.r = 0

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','x','y','r','shape']

        self.show()
        self.raise_()


    def ring_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws the ring
            (properly projected and transformed).
        """
        center = QtGui.QVector3D(self.position)
        points = [center + QtGui.QVector3D(math.cos(i/32.*math.pi*2)*self.r,
                                           math.sin(i/32.*math.pi*2)*self.r, 0)
                  for i in range(33)]
        return self.draw_lines([points], offset)


    def get_input_pos(self):
        return self.pos() + QtCore.QPoint(3, self.height()/2)
    def get_output_pos(self):
        return self.pos() + QtCore.QPoint(self.width() - 3, self.height()/2)

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
        self.node._r.set_expr(str(float(self.node._r.get_expr()) + dr))


    def make_masks(self):
        """ Render various masks and assign them to the DragManagers.
            (then set the global widget mask)
        """
        self.drag_control.mask = self.paint_mask(self.draw_center)
        self.ring_drag_control.mask = self.paint_mask(self.draw_ring)
        self.setMask(self.drag_control.mask | self.ring_drag_control.mask)


    def _sync(self):
        """ Move and scale this control to the appropriate position.
            Use self.position and self.r if eval fails.
        """
        p = QtCore.QPointF(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y())

        r = self.node.r if self.node._r.valid() else self.r

        # Figure out if these fundamental values have changed
        changed = (self.position != p) or (self.r != r)

        # Cache these values
        self.position = p
        self.r = r

        return changed

    def reposition(self):
        """ Repositions the node and calls self.update
        """
        # Get bounding box from painter path
        rect = self.ring_path().boundingRect().toRect()
        rect.setTop(rect.top() - 5)
        rect.setBottom(rect.bottom() + 5)
        rect.setLeft(rect.left() - 5)
        rect.setRight(rect.right() + 5)

        self.setGeometry(rect)
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



    def draw_ring(self, painter, mask=False):
        """ Draws the ring around the widget.
        """
        if self.r <= 0:     return
        path = self.ring_path(self.pos())

        if mask:    r = 6
        else:       r = 2

        self.set_pen(painter, mask, self.ring_drag_control, colors.grey)

        painter.drawPath(path)


    def paintEvent(self, painter):
        painter = QtGui.QPainter(self)
        self.draw_center(painter)
        self.draw_ring(painter)

from node.base import get_name
from node.circle import Circle
