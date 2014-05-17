from PySide import QtCore, QtGui
from sb.controls.axes import AxesControl

class Canvas(QtGui.QGraphicsView):
    rotated = QtCore.Signal(QtGui.QMatrix4x4)

    def __init__(self, parent=None):
        self.scene = QtGui.QGraphicsScene(parent)
        super(Canvas, self).__init__(self.scene, parent)
        self.setStyleSheet('QGraphicsView { border-style: none; }')
        self.setBackgroundBrush(QtCore.Qt.black)
        self.setDragMode(QtGui.QGraphicsView.ScrollHandDrag)
        self.setSceneRect(-self.width()/2, -self.height()/2,
                           self.width(), self.height())


    def wheelEvent(self, event):
        p = self.mapToScene(event.pos())
        r = self.sceneRect()
        s = 1.001 ** -event.delta()
        self.setSceneRect(p.x() + (r.x() - p.x()) / s,
                          p.y() + (r.y() - p.y()) / s,
                          r.width() / s, r.height() / s)
        self.scale(s, s)

    def mousePressEvent(self, event):
        super(Canvas, self).mousePressEvent(event)
        self._mouse_click_pos = self.mapToScene(event.pos())

    def mouseMoveEvent(self, event):
        if event.buttons() == QtCore.Qt.LeftButton:
            mcp = self.mapToScene(event.pos())
            r = self.sceneRect()
            r.translate(self._mouse_click_pos - mcp)
            self.setSceneRect(r)

