from PySide import QtCore, QtGui

from sb.controls.control import Control

class Point3DControl(Control):
    @property
    def pos(self):
        return QtGui.QVector3D(
                self._node.object_datums['x']._value,
                self._node.object_datums['y']._value,
                self._node.object_datums['z']._value)

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
        for a in 'xyz':
            if self._node.object_datums[a].simple():
                self._node.object_datums[a] += getattr(d, a)()

    def update_center(self):
        """ Recalculates viewport coordinates where the node viewer should be
            positioned, then emits center_changed with that position.
        """
        pt = self.transform_points([self.pos])[0]
        self.center_changed.emit(QtCore.QPoint(pt.x(), pt.y()))

################################################################################

class Point2DControl(Control):
    @property
    def pos(self):
        return QtGui.QVector3D(
                self._node.object_datums['x']._value,
                self._node.object_datums['y']._value, 0)

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
            if self._node.object_datums[a].simple():
                self._node.object_datums[a] += getattr(d, a)()

    def update_center(self):
        """ Recalculates viewport coordinates where the node viewer should be
            positioned, then emits center_changed with that position.
        """
        pt = self.transform_points([self.pos])[0]
        self.center_changed.emit(QtCore.QPoint(pt.x(), pt.y()))

