from PySide import QtCore, QtGui

class ScriptEditor(QtGui.QTextEdit):
    def __init__(self, parent):
        super(ScriptEditor, self).__init__(parent)

        font = QtGui.QFont()
        font.setFamily("Courier")
        self.setFont(font)

