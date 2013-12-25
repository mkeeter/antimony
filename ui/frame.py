from PySide import QtGui

class Frame(QtGui.QWidget):
    def __init__(self):
        super(Frame, self).__init__()
        self.setGeometry(100, 100, 500, 400)
        self.setWindowTitle("kokopuffs")
        self.show()
