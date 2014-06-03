import math

from PySide import QtCore, QtGui

from sb.controls.control import DummyControl
from sb.controls.point import Point2DControl
from sb.controls.multiline import MultiLineControl

class _RadiusControl(MultiLineControl):
    def __init__(self, canvas, node, parent):
        super().__init__(canvas, node, parent)
        self.watch_datums('x','y','r')

    @property
    def pos(self):
        return self.parentObject().pos

    def _lines(self):
        pos = self.parentObject().pos
        r = self._cache['r']
        n = 100
        return [[pos + r * QtGui.QVector3D(math.cos(i*2*math.pi/n),
                                           math.sin(i*2*math.pi/n), 0)
                for i in range(n+1)]]

    def drag(self, p, d):
        self._node.get_datum('r').increment(
                QtGui.QVector3D.dotProduct((p - self.pos).normalized(), d))

################################################################################

class CircleControl(DummyControl):

    def __init__(self, canvas, node):
        super().__init__(canvas, node)
        self._center = Point2DControl(canvas, node, self)
        self._radius = _RadiusControl(canvas, node, self)

    @property
    def pos(self):
        return self._center.pos

    def center_pos(self):
        pt = self.transform_points([self.pos])[0]
        return pt.toPoint()

    def drag(self, p, d):
        self._center.drag(p, d)



