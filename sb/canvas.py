from PySide import QtCore, QtGui

class Canvas(QtGui.QGraphicsView):
    rotated = QtCore.Signal(QtGui.QMatrix4x4)

    def __init__(self, parent=None):
        self.scene = QtGui.QGraphicsScene(parent)
        super(Canvas, self).__init__(self.scene, parent)
        self.setStyleSheet('QGraphicsView { border-style: none; }')
        self.setBackgroundBrush(QtCore.Qt.black)
