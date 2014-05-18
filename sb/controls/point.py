from PySide import QtCore, QtGui

from sb.controls.control import Control

class Point3DControl(Control):
    def __init__(self, node, canvas, x, y, z):
        super(Point3DControl, self).__init__(canvas, node)
        self.x = x
        self.y = y
        self.z = z

    def boundingRect(self):
        return self.bounding_box([
            QtGui.QVector3D(self.x._value, self.y._value, self.z._value)])

    def paint(self, painter, options, widget):
        if self._hover:
            painter.setPen(QtGui.QPen(QtCore.Qt.white, 2))
        painter.setBrush(QtGui.QBrush(QtCore.Qt.red))
        painter.drawPath(self.shape())

    def shape(self):
        pt = self.transform_points([
            QtGui.QVector3D(self.x._value, self.y._value, self.z._value)])[0]
        path = QtGui.QPainterPath()
        path.addEllipse(pt.x() - 5, pt.y() - 5, 10, 10)
        return path

