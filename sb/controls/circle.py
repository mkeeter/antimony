import math

from PySide import QtCore, QtGui

from sb.controls.control import DummyControl
from sb.controls.point import Point2DControl
from sb.controls.multiline import MultiLineControl

class _RadiusControl(MultiLineControl):
    @property
    def pos(self):
        return self.parentObject().pos

    def _lines(self):
        pos = self.parentObject().pos
        r = self._node.object_datums['r']._value
        n = 100
        return [[pos + r * QtGui.QVector3D(math.cos(i*2*math.pi/n),
                                           math.sin(i*2*math.pi/n), 0)
                for i in range(n+1)]]

    def drag(self, p, d):
        if self._node.object_datums['r'].simple():
            self._node.object_datums['r'] += QtGui.QVector3D.dotProduct(
                    (p - self.pos).normalized(), d)

################################################################################

class CircleControl(DummyControl):

    def __init__(self, canvas, node):
        super(CircleControl, self).__init__(canvas, node)
        self._center = Point2DControl(canvas, node, self)
        self._radius = _RadiusControl(canvas, node, self)

    @property
    def pos(self):
        return QtGui.QVector3D(
                self._node.object_datums['x']._value,
                self._node.object_datums['y']._value,
                0)

    def update_center(self):
        """ Recalculates viewport coordinates where the node viewer should be
            positioned, then emits center_changed with that position.
        """
        pt = self.transform_points([self.pos])[0]
        self.center_changed.emit(QtCore.QPoint(pt.x(), pt.y()))

    def drag(self, p, d):
        self._center.drag(p, d)



