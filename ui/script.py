from PySide import QtCore, QtGui
import keyword
from control import colors

class _Highlighter(QtGui.QSyntaxHighlighter):
    def __init__(self, parent):
        super(_Highlighter, self).__init__(parent)
        self.rules = []

        keyword_format = QtGui.QTextCharFormat()
        keyword_format.setForeground(QtGui.QColor(*colors.green))

        for k in keyword.kwlist:
            self.rules.append(
                    (QtCore.QRegExp(r'\b' + k + r'\b'), keyword_format))

        quote_format = QtGui.QTextCharFormat()
        quote_format.setForeground(QtGui.QColor(*colors.cyan))
        self.rules.append((QtCore.QRegExp(r'\".*\"'), quote_format))
        self.rules.append((QtCore.QRegExp(r"\'.*\'"), quote_format))

        num_format = QtGui.QTextCharFormat()
        num_format.setForeground(QtGui.QColor(*colors.blue))
        self.rules.append((QtCore.QRegExp(r'\d+'), num_format))
        self.rules.append((QtCore.QRegExp(r'\d+\.\d*'), num_format))

        comment_format = QtGui.QTextCharFormat()
        comment_format.setForeground(QtGui.QColor(*colors.base01))
        self.rules.append((QtCore.QRegExp('#.*'), comment_format))

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

        self.setTabStopWidth(fm.width('    '))

        _Highlighter(self.document())
        self.setLineWrapMode(self.NoWrap)

        self.horizontalScrollBar().setStyleSheet("QScrollBar {height:0px;}")
        self.verticalScrollBar().setStyleSheet("QScrollBar {width:0px;}")
        self.setMouseTracking(True)

        self.resizing = False

    def mousePressEvent(self, event):
        if event.x() < self.width() - 20:
            super(ScriptEditor, self).mousePressEvent(event)
        else:
            self.resizing = True
            self.mouse_x = event.x()

    def mouseReleaseEvent(self, event):
        if self.resizing:
            self.resizing = False
        else:
            super(ScriptEditor, self).mouseReleaseEvent(event)

    def mouseMoveEvent(self, event):
        if self.resizing:
            self.resize(max(40, self.width() + event.x() - self.mouse_x),
                        self.height())
            self.mouse_x = event.x()
        else:
            if event.x() >= self.width() - 20:
                QtGui.QApplication.instance().setOverrideCursor(
                        QtCore.Qt.SizeHorCursor)
            else:
                QtGui.QApplication.instance().setOverrideCursor(
                        QtCore.Qt.IBeamCursor)
                super(ScriptEditor, self).mouseMoveEvent(event)

    def leaveEvent(self, event):
        QtGui.QApplication.instance().setOverrideCursor(
                QtCore.Qt.ArrowCursor)
        super(ScriptEditor, self).leaveEvent(event)


