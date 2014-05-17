from PySide import QtGui

from sb.canvas import Canvas
from ui_main_window import Ui_MainWindow

class MainWindow(QtGui.QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.add_canvas()
        self.set_shortcuts()

    def add_canvas(self):
        """ Adds the QGraphicsView that represents our scene.
        """
        self.canvas = Canvas()
        self.ui.gridLayout.addWidget(self.canvas, 0, 0)
        self.canvas.lower()

    def set_shortcuts(self):
        """ Sets keyboard shortcuts for UI actions
            (as Qt Designer doesn't have a way to do so).
        """
        self.ui.actionNew.setShortcuts(QtGui.QKeySequence.New)
        self.ui.actionOpen.setShortcuts(QtGui.QKeySequence.Open)
        self.ui.actionSave.setShortcuts(QtGui.QKeySequence.Save)
        self.ui.actionSaveAs.setShortcuts(QtGui.QKeySequence.SaveAs)
        self.ui.actionQuit.setShortcuts(QtGui.QKeySequence.Quit)
