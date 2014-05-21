import math

from PySide import QtCore, QtGui

from sb.controls.control import Control, DummyControl
from sb.controls.multiline import MultiLineControl

class _RadiusControl(MultiLineControl):
    @property
    def pos(self):
        return self.parentObject().pos

    def __init__(self, canvas, node, parent):
        super(_RadiusControl, self).__init__(canvas, node, parent)

    def _lines(self):
        pos = self.parentObject().pos
        r = self._node.object_datums['r']._value
        n = 100
        return [[pos + r * QtGui.QVector3D(math.cos(i*2*math.pi/n),
                                           math.sin(i*2*math.pi/n), 0)
                for i in range(n+1)]]

    def update_center(self):
        self.parentObject().update_center()

    def drag(self, p, d):
        if self._node.object_datums['r'].simple():
            self._node.object_datums['r'] += QtGui.QVector3D.dotProduct(
                    (p - self.pos).normalized(), d)


class _CircleCenter(Control):
    @property
    def pos(self):
        return self.parentObject().pos

    def boundingRect(self):
        return self.bounding_box([self.pos])

    def paint(self, painter, options, widget):
        self.set_default_pen(painter)
        self.set_default_brush(painter)
        painter.drawPath(self.shape())

    def shape(self):
        pt = self.transform_points([self.pos])[0]
        path = QtGui.QPainterPath()
        path.addEllipse(pt.x() - 5, pt.y() - 5, 10, 10)
        return path

    def drag(self, p, d):
        for a in 'xy':
            if self._node.object_datums[a].simple():
                self._node.object_datums[a] += getattr(d, a)()

    def update_center(self):
        self.parentObject().update_center()


################################################################################

class CircleControl(DummyControl):

    def __init__(self, canvas, node):
        super(CircleControl, self).__init__(canvas, node)
        self._center = _CircleCenter(canvas, node, self)
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



