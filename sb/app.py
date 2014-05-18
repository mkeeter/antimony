import sys

from PySide import QtCore, QtGui

from sb.ui.main_window import MainWindow

from sb.controls.axes import AxesControl
from sb.nodes.point import Point3D

class App(QtGui.QApplication):
    def __init__(self):
        super(App, self).__init__(sys.argv)

        self.window = MainWindow()
        a = AxesControl(self.window.canvas)
        b = Point3D('hi', 'hi.y', 10, 10)
        b.make_controls(self.window.canvas)

        self.window.activateWindow()
        self.window.show()
        self.window.raise_()

if __name__ == '__main__':
    app = App()
    sys.exit(app.exec_())
