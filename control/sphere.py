import math

from PySide import QtCore, QtGui

import colors
import base

class SphereControl(base.NodeControl):
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

        self.position = QtGui.QVector3D()
        self.r = 0

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
        p.setZ(0)
        p = p.normalized()

        v.setZ(0)
        dr = QtGui.QVector3D.dotProduct(p, v)
        self.node._r.set_expr(str(float(self.node._r.get_expr()) + dr))

    def make_masks(self):
        for n in ['center', 'wireframe']:
            func = getattr(self, 'draw_' + n)
            painter = QtGui.QPainter()
            bitmap = QtGui.QBitmap(self.size())
            bitmap.clear()

            painter.begin(bitmap)
            func(painter, True)
            painter.end()

            setattr(self, n+'_mask', QtGui.QRegion(bitmap))
        self.drag_control.mask = self.center_mask
        self.radius_drag.mask = self.wireframe_mask


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

    def _sync(self):

        try:    x = self.node.x
        except: x = self.position.x()

        try:    y = self.node.y
        except: y = self.position.y()

        try:    z = self.node.z
        except: z = self.position.z()

        try:    r = self.node.r
        except: r = self.r

        # Figure out if these fundamental values have changed
        changed = self.position != QtGui.QVector3D(x, y, z) or self.r != r

        # Cache these values
        self.position = QtGui.QVector3D(x, y, z)
        self.r = r

        rect = self.wireframe_path().boundingRect().toRect()
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

        if mask:                                                    d = 22
        elif self.drag_control.hover or self.drag_control.drag:     d = 20
        else:                                                       d = 14

        painter.drawEllipse((width - d) / 2, (height - d) / 2, d, d)

    def draw_wireframe(self, painter, mask=False):
        """ Draw the wireframe outline of a sphere.
        """
        if self.r <= 0:     return
        path = self.wireframe_path(self.pos())

        if mask:                                                r = 6
        elif self.radius_drag.hover or self.radius_drag.drag:   r = 4
        else:                                                   r = 2

        if mask:
            painter.setBrush(QtGui.QBrush())
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, r))
        else:
            painter.setBrush(QtGui.QBrush())
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.light_grey), r))

        painter.drawPath(path)

    def paintEvent(self, event):
        """ Draw the sphere's center and wireframe.
        """
        painter = QtGui.QPainter(self)
        self.draw_center(painter)
        self.draw_wireframe(painter)

from node.base import get_name
from node.sphere import Sphere
