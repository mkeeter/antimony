import sys

from PySide import QtGui

import canvas

################################################################################

class Window(QtGui.QMainWindow):
    def __init__(self, app, canvas):
        super(Window, self).__init__()
        self.setWindowTitle("Antimony")
        self.setCentralWidget(canvas)
        self.setGeometry(0, 900/4, 1440/2, 900/2)
        self.make_menus(app)
        self.show()

    def make_menus(self, app):
         fileMenu = self.menuBar().addMenu("File");
         fileMenu.addAction(app.new_action)
         fileMenu.addAction(app.open_action)
         fileMenu.addAction(app.save_action)
         fileMenu.addAction(app.saveas_action)

################################################################################

class App(QtGui.QApplication):
    def __init__(self):
        super(App, self).__init__(sys.argv)
        self.setStyleSheet("""
        QGroupBox {
            background-color: #eee;
            border: 0px;
        }""")

        self.make_actions()

        self.canvas = canvas.Canvas()
        self.window = Window(self, self.canvas)

    def make_actions(self):
        self.new_action = QtGui.QAction("New", self)
        self.new_action.setShortcuts(QtGui.QKeySequence.New)
        self.new_action.triggered.connect(self.on_new)

        self.open_action = QtGui.QAction("Open", self)
        self.open_action.setShortcuts(QtGui.QKeySequence.Open)
        self.open_action.triggered.connect(self.on_open)

        self.save_action = QtGui.QAction("Save", self)
        self.save_action.setShortcuts(QtGui.QKeySequence.Save)
        self.save_action.triggered.connect(self.on_save)

        self.saveas_action = QtGui.QAction("Save As", self)
        self.saveas_action.setShortcuts(QtGui.QKeySequence.SaveAs)
        self.saveas_action.triggered.connect(self.on_saveas)

    def on_new(self):
        pass

    def on_open(self):
        pass

    def on_save(self):
        pass

    def on_saveas(self):
        pass
