from PySide import QtGui

class Canvas(QtGui.QGraphicsView):
    def __init__(self, parent=None):
        self.scene = QtGui.QGraphicsScene(parent)
        super(Canvas, self).__init__(self.scene, parent)
