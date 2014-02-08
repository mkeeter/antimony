from PySide import QtCore, QtGui
import keyword
from control import colors

class _Highlighter(QtGui.QSyntaxHighlighter):
    def __init__(self, parent):
        super(_Highlighter, self).__init__(parent)
        self.rules = []

        keyword_format = QtGui.QTextCharFormat()
        keyword_format.setForeground(QtGui.QColor(*colors.blue))

        for k in keyword.kwlist:
            self.rules.append((QtCore.QRegExp(k), keyword_format))


    def highlightBlock(self, text):
        for r in self.rules:
            index = r[0].indexIn(text)
            while index >= 0:
                length = r[0].matchedLength()
                self.setFormat(index, length, r[1])
                index = r[0].indexIn(text, index + length)


class ScriptEditor(QtGui.QPlainTextEdit):
    def __init__(self, parent):
        super(ScriptEditor, self).__init__(parent)

        font = QtGui.QFont()
        font.setFamily("Courier")
        self.setFont(font)

        fm = QtGui.QFontMetrics(font)
        self.resize(fm.width(' ')*60, 100)

        _Highlighter(self.document())

        self.resizing = False

    def mousePressEvent(self, event):
        if event.x() < self.width() - 20:
            return event.ignore()
        else:
            self.resizing = True
            self.mouse_x = event.x()

    def mouseMoveEvent(self, event):
        if self.resizing:
            self.resize(max(40, self.width() + event.x() - self.mouse_x),
                        self.height())
            self.mouse_x = event.x()

