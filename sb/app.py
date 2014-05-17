import sys

from PySide import QtGui

from sb.scene import GraphicsScene
from sb.ui.main_window import MainWindow

class App(QtGui.QApplication):
    def __init__(self):
        super(App, self).__init__(sys.argv)

        self.window = MainWindow()
        t = self.window.canvas.scene.addText("OMGWTF")
        t.setFlags(QtGui.QGraphicsItem.ItemIsMovable)
        self.window.activateWindow()
        self.window.show()
        self.window.raise_()

if __name__ == '__main__':
    app = App()
    sys.exit(app.exec_())
