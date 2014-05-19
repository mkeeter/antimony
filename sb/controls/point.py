from PySide import QtCore, QtGui

from sb.controls.control import Control

class Point3DControl(Control):
    def __init__(self, node, canvas, x, y, z):
        super(Point3DControl, self).__init__(canvas, node)
        self.x = x
        self.y = y
        self.z = z
        self.setFlags(QtGui.QGraphicsItem.ItemIsMovable |
                      QtGui.QGraphicsItem.ItemIsSelectable)

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

    def mousePressEvent(self, event):
        """ Saves a mouse click position (in scene coordinates).
        """
        self._mouse_click_pos = event.pos()

    def mouseMoveEvent(self, event):
        drag = event.pos() - self._mouse_click_pos
        d = self.itransform_points([drag])[0]

        if self.x.simple():
            self.x += d.x()
        if self.y.simple():
            self.y += d.y()
        if self.z.simple():
            self.z += d.z()
        self._mouse_click_pos = event.pos()

    def update_center(self):
        """ Recalculates viewport coordinates where the node viewer should be
            positioned, then emits center_changed with that position.
        """
        pt = self.transform_points([
            QtGui.QVector3D(self.x._value, self.y._value, self.z._value)])[0]
        self.center_changed.emit(self._canvas.mapFromScene(pt))

