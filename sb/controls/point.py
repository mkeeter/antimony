from PySide import QtCore, QtGui

from sb.controls.control import Control

class Point3DControl(Control):
    def __init__(self, canvas, x, y, z):
        super(Point3DControl, self).__init__(canvas)
        self.x = x
        self.y = y
        self.z = z

    def boundingRect(self):
        return self.bounding_box([
            QtGui.QVector3D(self.x._value, self.y._value, self.z._value)])

    def paint(self, painter, options, widget):
        pt = self.transform_points([
            QtGui.QVector3D(self.x._value, self.y._value, self.z._value)])[0]
        painter.setPen(QtGui.QPen(QtCore.Qt.white, 2))
        painter.setBrush(QtGui.QBrush(QtCore.Qt.red))
        painter.drawEllipse(pt.x() - 5, pt.y() - 5, 10, 10)

