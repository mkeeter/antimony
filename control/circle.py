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

        self.drag_pt = False
        self.drag_r  = False

        self.hover_pt = False
        self.hover_r  = False

        # Cached values (used if the node's values are invalid)
        self.position = QtCore.QPointF()
        self.r = 0

        self.sync()
        self.make_masks()

        self.editor_datums = ['name','x','y','r','shape']

        self.show()
        self.raise_()

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.RightButton:
            self.delete()
        elif event.button() == QtCore.Qt.LeftButton:
            self.mouse_pos = self.mapToParent(event.pos())
            if self.center_mask.contains(event.pos()):
                self.drag_pt = True
            else:
                self.drag_r = True

    def mouseMoveEvent(self, event):
        p = self.mapToParent(event.pos())
        if self.drag_pt or self.drag_r:
            v = self.canvas.drag_vector(self.mouse_pos, p)
            if self.drag_pt:
                self.drag_center(v)
            elif self.drag_r:
                pos = self.canvas.pixel_to_unit(p)
                self.drag_ring(pos, v)
        elif not self.hover_pt and self.center_mask.contains(event.pos()):
            self.hover_pt = True
            self.update()
        elif not self.hover_r and self.ring_mask.contains(event.pos()):
            self.hover_r = True
            self.update()
        self.mouse_pos = p

    def drag_center(self, v):
        """ Drag this node by attempting to change its x and y coordinates
            dx and dy should be floating-point values.
        """
        if self.node._x.simple():
            self.node._x.set_expr(str(float(self.node._x.get_expr()) + v.x()))
        if self.node._y.simple():
            self.node._y.set_expr(str(float(self.node._y.get_expr()) + v.y()))

    def ring_path(self, offset=QtCore.QPoint()):
        """ Returns a painter path that draws the ring
            (properly projected and transformed).
        """
        center = QtGui.QVector3D(self.position)
        points = [center + QtGui.QVector3D(math.cos(i/32.*math.pi*2)*self.r,
                                           math.sin(i/32.*math.pi*2)*self.r, 0)
                  for i in range(33)]
        return self.draw_lines([points], offset)


    def drag_ring(self, p, v):
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


    def mouseDoubleClickEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.open_editor()

    def mouseReleaseEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.drag_pt = False
            self.drag_r = False

    def leaveEvent(self, event):
        self.hover_pt = False
        self.hover_r = False
        self.update()

    def make_masks(self):
        for n in ['center','ring']:
            func = getattr(self, 'draw_' + n)
            painter = QtGui.QPainter()
            bitmap = QtGui.QBitmap(self.size())
            bitmap.clear()

            painter.begin(bitmap)
            func(painter, True)
            painter.end()

            setattr(self, n + '_mask', QtGui.QRegion(bitmap))

        self.setMask(self.center_mask.united(self.ring_mask))


    def sync(self):
        """ Move and scale this control to the appropriate position.
            Use self.position and self.r if eval fails.
        """
        try:    x = self.node.x
        except: x = self.position.x()

        try:    y = self.node.y
        except: y = self.position.y()

        try:    r = self.node.r
        except: r = self.r

        # Figure out if these fundamental values have changed
        changed = self.position != QtCore.QPointF(x, y) or self.r != r

        # Cache these values
        self.position = QtCore.QPointF(x, y)
        self.r = r

        # Get bounding box from painter path
        rect = self.ring_path().boundingRect().toRect()
        rect.setTop(rect.top() - 5)
        rect.setBottom(rect.bottom() + 5)
        rect.setLeft(rect.left() - 5)
        rect.setRight(rect.right() + 5)

        # Check whether any render information has changed.
        changed |= self.geometry() != rect
        self.setGeometry(rect)

        if changed:
            self.make_masks()
            self.update()

        super(CircleControl, self).sync()


    def draw_center(self, painter, mask=False):
        """ Draws a circle at the center of the widget.
        """
        width, height = self.width(), self.height()

        if mask:
            painter.setBrush(QtGui.QBrush(QtCore.Qt.color1))
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 2))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(*colors.light_grey)))
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.dark_grey), 2))

        if mask:                                d = 22
        elif self.hover_pt or self.drag_pt:     d = 20
        else:                                   d = 14

        painter.drawEllipse((width - d) / 2, (height - d) / 2, d, d)


    def draw_ring(self, painter, mask=False):
        """ Draws the ring around the widget.
        """
        if self.r <= 0:     return
        path = self.ring_path(self.pos())

        if mask:                            r = 6
        elif self.hover_r or self.drag_r:   r = 4
        else:                               r = 2

        if mask:
            painter.setBrush(QtGui.QBrush())
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, r))
        else:
            painter.setBrush(QtGui.QBrush())
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.light_grey), r))

        painter.drawPath(path)


    def paintEvent(self, painter):
        painter = QtGui.QPainter(self)
        self.draw_center(painter)
        self.draw_ring(painter)

from node.base import get_name
from node.circle import Circle
