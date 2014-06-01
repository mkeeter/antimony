from PySide import QtCore, QtGui

from sb.controls.control import Control

class Point3DControl(Control):

    def __init__(self, canvas, node):
        super().__init__(canvas, node)
        self.watch_datums('x','y','z')

    @property
    def pos(self):
        return QtGui.QVector3D(
                self._cache['x'], self._cache['y'], self._cache['z'])

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

    def drag(self, c, d):
        self._node.get_datum('x').increment(d.x())
        self._node.get_datum('y').increment(d.y())
        self._node.get_datum('z').increment(d.z())

    def center_pos(self):
        pt = self.transform_points([self.pos])[0]
        return pt.toPoint()

################################################################################

class Point2DControl(Control):
    @property
    def pos(self):
        return QtGui.QVector3D(
                self._node.datums['x']._value,
                self._node.datums['y']._value, 0)

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

    def drag(self, c, d):
        for a in 'xy':
            if self._node.datums[a].simple():
                self._node.datums[a] += getattr(d, a)()

    def center_pos(self):
        pt = self.transform_points([self.pos])[0]
        return pt.toPoint()


