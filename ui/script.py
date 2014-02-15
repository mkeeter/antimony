# coding=utf-8

from PySide import QtCore, QtGui
import keyword
from control import colors

class _Highlighter(QtGui.QSyntaxHighlighter):
    def __init__(self, parent):
        super(_Highlighter, self).__init__(parent)
        self.rules = []

        keyword_format = QtGui.QTextCharFormat()
        keyword_format.setForeground(QtGui.QColor(*colors.green))

        for k in keyword.kwlist + ['input', 'output']:
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


################################################################################

class ScriptEditor(QtGui.QPlainTextEdit):
    def __init__(self, parent):
        super(ScriptEditor, self).__init__(parent)

        font = QtGui.QFont()
        font.setFamily("Courier")
        self.setFont(font)

        fm = QtGui.QFontMetrics(font)
        self.base_width = fm.width(' ')*60
        self.resize(0, 100)
        self.hide()

        self.setTabStopWidth(fm.width('    '))

        _Highlighter(self.document())
        self.setLineWrapMode(self.NoWrap)

        self.horizontalScrollBar().setStyleSheet("QScrollBar {height:0px;}")
        self.verticalScrollBar().setStyleSheet("QScrollBar {width:0px;}")
        self.setMouseTracking(True)

        self.resizing = False
        self.target = None
        self.textChanged.connect(self.update_target)

        self.make_buttons()


    def make_buttons(self):
        """ Creates buttons to resize and close the editor window.
        """
        ss = """
            QPushButton {
                background-color: "%s";
                border-width: 4px;
                margin: 0px;
                color: "%s";
            }
        """ % (colors.base02h, colors.base1h)
        close_button = QtGui.QPushButton(u'✖', self)
        close_button.clicked.connect(self.close)
        close_button.setStyleSheet(ss)
        close_button.setCursor(QtCore.Qt.ArrowCursor)

        resize_button = QtGui.QPushButton(u'⇔', self)
        resize_button.setStyleSheet(ss)
        resize_button.setCursor(QtCore.Qt.SizeHorCursor)

        # Set up resize button to resize the window with mouse events
        def mousePress(event):
            QtGui.QPushButton.mousePressEvent(resize_button, event)
            self.resizing = True
            self.mouse_x = event.x()
        def mouseMove(event):
            if not self.resizing:   return
            self.set_width(max(40, self.width() + event.x() - self.mouse_x))
        def mouseRelease(event):
            QtGui.QPushButton.mouseReleaseEvent(resize_button, event)
            self.resizing = False
        resize_button.mousePressEvent = mousePress
        resize_button.mouseMoveEvent = mouseMove
        resize_button.mouseReleaseEvent = mouseRelease

        grid = QtGui.QGridLayout(self)
        grid.setColumnStretch(0, 1)
        grid.setColumnStretch(1, 0)
        grid.setRowStretch(0, 0)
        grid.setRowStretch(1, 0)
        grid.setRowStretch(2, 1)
        grid.addWidget(close_button, 0, 1)
        grid.addWidget(resize_button, 1, 1)

        grid.setContentsMargins(0, 0, 0, 0)
        self.setLayout(grid)

    def update_target(self):
        """ Calls set_expr on the target datum.
        """
        if self.target is None:     return
        elif self.toPlainText() != self.target.get_expr():
            self.target.set_expr(self.toPlainText())
            self.target.node.update_datums()

    # Create a width_ property to animate opening and closing.
    def get_width(self):
        return self.width()
    def set_width(self, w):
        self.resize(w, self.height())
    width_ = QtCore.Property(float, get_width, set_width)

    def open(self, datum):
        self.target = datum
        self.setPlainText(self.target.get_expr())
        if not self.isVisible():
            self._animate_open()

    def close(self):
        self.target = None
        self._animate_close()

    def _animate_open(self):
        """ Animates the script editor sliding open.
        """
        a = QtCore.QPropertyAnimation(self, "width_", self)
        a.setDuration(100)
        a.setStartValue(0)
        a.setEndValue(self.base_width)
        self.show()
        a.start(QtCore.QPropertyAnimation.DeleteWhenStopped)

    def _animate_close(self):
        """ Animates the panel slicing closed.
            Calls self.hide when the panel is completely closed.
        """
        a = QtCore.QPropertyAnimation(self, "width_", self)
        a.setDuration(100)
        a.setStartValue(self.width())
        a.setEndValue(0)
        a.finished.connect(self.hide)
        a.start(QtCore.QPropertyAnimation.DeleteWhenStopped)

