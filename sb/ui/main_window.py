from PySide import QtGui
from sb.scene import GraphicsScene
from sb.view import GraphicsView
from ui_main_window import Ui_MainWindow

class MainWindow(QtGui.QMainWindow):
    def __init__(self, scene):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.add_view(scene)
        self.set_shortcuts()

    def add_view(self, scene):
        """ Adds the QGraphicsView that represents our scene.
        """
        scene.setSceneRect( -100.0, -100.0, 200.0, 200.0)
        self.item = QtGui.QGraphicsEllipseItem(None, scene)
        self.item.setRect( -50.0, -50.0, 100.0, 100.0)

        v = QtGui.QGraphicsView(scene)
        v.setStyleSheet("QWidget { background-color: red; }")
        self.ui.gridLayout.addWidget(v, 0, 0)
        v.lower()

    def set_shortcuts(self):
        """ Sets keyboard shortcuts for UI actions
            (as Qt Designer doesn't have a way to do so).
        """
        self.ui.actionNew.setShortcuts(QtGui.QKeySequence.New)
        self.ui.actionOpen.setShortcuts(QtGui.QKeySequence.Open)
        self.ui.actionSave.setShortcuts(QtGui.QKeySequence.Save)
        self.ui.actionSaveAs.setShortcuts(QtGui.QKeySequence.SaveAs)
        self.ui.actionQuit.setShortcuts(QtGui.QKeySequence.Quit)
