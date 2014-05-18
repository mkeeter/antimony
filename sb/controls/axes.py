from PySide import QtCore, QtGui
from sb.controls.control import Control

class AxesControl(Control):
    def __init__(self, canvas):
        super(AxesControl, self).__init__(canvas)

    def boundingRect(self):
        return self.bounding_box([
            QtGui.QVector3D(0, 0, 0),
            QtGui.QVector3D(100, 0, 0),
            QtGui.QVector3D(0, 100, 0),
            QtGui.QVector3D(0, 0, 100)])

    def paint(self, painter, options, widget):
        pts = self.transform_points([
            QtGui.QVector3D(0, 0, 0),
            QtGui.QVector3D(100, 0, 0),
            QtGui.QVector3D(0, 100, 0),
            QtGui.QVector3D(0, 0, 100)])
        painter.setPen(QtGui.QPen(QtCore.Qt.red, 2))
        painter.drawLine(pts[0], pts[1])
        painter.setPen(QtGui.QPen(QtCore.Qt.green, 2))
        painter.drawLine(pts[0], pts[2])
        painter.setPen(QtGui.QPen(QtCore.Qt.blue, 2))
        painter.drawLine(pts[0], pts[3])
