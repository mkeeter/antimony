from PySide import QtCore, QtGui

from sb.controls.control import DummyControl
from sb.controls.point import Point2DControl
from sb.controls.multiline import MultiLineControl

class _Rectangle(MultiLineControl):
    def _lines(self):
        x0 = self._node.object_datums['x0']._value
        x1 = self._node.object_datums['x1']._value
        y0 = self._node.object_datums['y0']._value
        y1 = self._node.object_datums['y1']._value
        return [[QtGui.QVector3D(x0, y0, 0), QtGui.QVector3D(x0, y1, 0),
                 QtGui.QVector3D(x1, y1, 0), QtGui.QVector3D(x1, y0, 0),
                 QtGui.QVector3D(x0, y0, 0)]]

    def drag(self, c, d):
        for i in '01':
            for a in 'xy':
                x = '%s%s' % (a, i)
                if self._node.object_datums[x].simple():
                    self._node.object_datums[x] += getattr(d, a)()

class _RectanglePtControl(Point2DControl):
    def __init__(self, canvas, node, parent, num):
        super(_RectanglePtControl, self).__init__(canvas, node, parent)
        self.num = num

    @property
    def pos(self):
        return QtGui.QVector3D(
                self._node.object_datums['x%i' % self.num]._value,
                self._node.object_datums['y%i' % self.num]._value, 0)

    def drag(self, c, d):
        for a in 'xy':
            x = '%s%i' % (a, self.num)
            if self._node.object_datums[x].simple():
                self._node.object_datums[x] += getattr(d, a)()

class RectangleControl(DummyControl):
    def __init__(self, canvas, node):
        super(RectangleControl, self).__init__(canvas, node)
        self.rect =  _Rectangle(canvas, node, self)
        self.lower_left  = _RectanglePtControl(canvas, node, self, 0)
        self.upper_right = _RectanglePtControl(canvas, node, self, 1)

    def update_center(self):
        pt = self.transform_points([self.lower_left.pos])[0]
        self.center_changed.emit(QtCore.QPoint(pt.x(), pt.y()))

    def drag(self, p, d):
        self.rect.drag(p, d)
