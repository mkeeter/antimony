from PySide import QtCore, QtGui

from sb.controls.control import Control, DummyControl
from sb.controls.multiline import MultiLineControl

class _Rectangle(MultiLineControl):

    def __init__(self, canvas, node, parent):
        super().__init__(canvas, node, parent)
        self.watch_datums('x0','y0','x1','y1')

    def _lines(self):
        x0 = self._cache['x0']
        x1 = self._cache['x1']
        y0 = self._cache['y0']
        y1 = self._cache['y1']
        return [[QtGui.QVector3D(x0, y0, 0), QtGui.QVector3D(x0, y1, 0),
                 QtGui.QVector3D(x1, y1, 0), QtGui.QVector3D(x1, y0, 0),
                 QtGui.QVector3D(x0, y0, 0)]]

    def drag(self, c, d):
        self._node.get_datum('x0').increment(d.x())
        self._node.get_datum('x1').increment(d.x())
        self._node.get_datum('y0').increment(d.y())
        self._node.get_datum('y1').increment(d.y())

class _RectanglePtControl(Control):
    def __init__(self, canvas, node, parent, num):
        super().__init__(canvas, node, parent)
        self.watch_datums('x%i' % num, 'y%i' % num)
        self.num = num

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

    @property
    def pos(self):
        return QtGui.QVector3D(
                self._cache['x%i' % self.num],
                self._cache['y%i' % self.num], 0)

    def drag(self, c, d):
        self._node.get_datum('x%i' % self.num).increment(d.x())
        self._node.get_datum('y%i' % self.num).increment(d.y())

################################################################################

class RectangleControl(DummyControl):
    def __init__(self, canvas, node):
        super(RectangleControl, self).__init__(canvas, node)
        self.rect =  _Rectangle(canvas, node, self)
        self.lower_left  = _RectanglePtControl(canvas, node, self, 0)
        self.upper_right = _RectanglePtControl(canvas, node, self, 1)

    def center_pos(self):
        pt = self.transform_points([self.lower_left.pos])[0]
        return pt.toPoint()

    def drag(self, p, d):
        self.rect.drag(p, d)
