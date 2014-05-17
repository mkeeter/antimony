from PySide import QtGui
from ui_main_window import Ui_MainWindow

class MainWindow(QtGui.QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.add_view()
        self.set_shortcuts()

    def add_view(self):
        """ Adds the QGraphicsView that represents our scene.
        """
        scene = QtGui.QGraphicsScene()
        scene.setSceneRect( -100.0, -100.0, 200.0, 200.0)
        item = QtGui.QGraphicsEllipseItem(None, scene)
        item.setRect( -50.0, -50.0, 100.0, 100.0)

        w = QtGui.QGraphicsView(scene)
        w.setStyleSheet("QWidget { background-color: red; }")
        self.ui.gridLayout.addWidget(w, 0, 0)
        w.lower()

    def set_shortcuts(self):
        """ Sets keyboard shortcuts for UI actions
            (as Qt Designer doesn't have a way to do so).
        """
        self.ui.actionNew.setShortcuts(QtGui.QKeySequence.New)
        self.ui.actionOpen.setShortcuts(QtGui.QKeySequence.Open)
        self.ui.actionSave.setShortcuts(QtGui.QKeySequence.Save)
        self.ui.actionSaveAs.setShortcuts(QtGui.QKeySequence.SaveAs)
        self.ui.actionQuit.setShortcuts(QtGui.QKeySequence.Quit)

if __name__ == '__main__':
    import sys
    app = QtGui.QApplication(sys.argv)
    m = MainWindow()
    m.show()
    m.raise_()
    app.exec_()
