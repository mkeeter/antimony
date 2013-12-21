import sys

from PySide import QtGui

import canvas

class App(QtGui.QApplication):
    def __init__(self):
        super(App, self).__init__(sys.argv)
        self.canvas = canvas.Canvas()
