from PySide import QtCore, QtGui

from sb.controls.control import Control

class Point3DControl(Control):
    def __init__(self, node, canvas):
        super(Point3DControl, self).__init__(canvas, node)
        self.setFlags(QtGui.QGraphicsItem.ItemIsMovable |
                      QtGui.QGraphicsItem.ItemIsSelectable)

    def boundingRect(self):
        return self.bounding_box([self.pos])

    def paint(self, painter, options, widget):
        if self._hover:
            painter.setPen(QtGui.QPen(QtCore.Qt.white, 2))
        if self.isSelected():
            painter.setBrush(QtGui.QBrush(QtCore.Qt.green))
        else:
            painter.setBrush(QtGui.QBrush(QtCore.Qt.red))
        painter.drawPath(self.shape())

    def shape(self):
        pt = self.transform_points([self.pos])[0]
        path = QtGui.QPainterPath()
        path.addEllipse(pt.x() - 5, pt.y() - 5, 10, 10)
        return path

    @property
    def pos(self):
        return QtGui.QVector3D(
                self._node.object_datums['x']._value,
                self._node.object_datums['y']._value,
                self._node.object_datums['z']._value)

    def drag(self, d):
        for a in 'xyz':
            if self._node.object_datums[a].simple():
                self._node.object_datums[a] += getattr(d, a)()

    def update_center(self):
        """ Recalculates viewport coordinates where the node viewer should be
            positioned, then emits center_changed with that position.
        """
        pt = self.transform_points([self.pos])[0]
        self.center_changed.emit(QtCore.QPoint(pt.x(), pt.y()))

