from PySide import QtCore, QtGui

from sb.controls.control import Control

class Point3DControl(Control):
    drag_x = QtCore.Signal(float)
    drag_y = QtCore.Signal(float)
    drag_z = QtCore.Signal(float)

    def __init__(self, canvas, node):
        super().__init__(canvas, node)
        for d in node.datums:
            d.changed.connect(self.update_cache)
            self._cache[d.name] = d._value
        self.drag_x.connect(node.get_datum('x').increment)
        self.drag_y.connect(node.get_datum('y').increment)
        self.drag_z.connect(node.get_datum('z').increment)

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
        self.drag_x.emit(d.x())
        self.drag_y.emit(d.y())
        self.drag_z.emit(d.z())

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


