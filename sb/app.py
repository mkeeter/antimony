import sys

from PySide import QtGui

from sb.scene import GraphicsScene
from sb.ui.main_window import MainWindow

from sb.controls.axes import AxesControl

class App(QtGui.QApplication):
    def __init__(self):
        super(App, self).__init__(sys.argv)

        self.window = MainWindow()
        self.a = AxesControl(self.window.canvas)

        self.window.activateWindow()
        self.window.show()
        self.window.raise_()

if __name__ == '__main__':
    app = App()
    sys.exit(app.exec_())
