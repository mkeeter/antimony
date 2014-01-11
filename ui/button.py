from PySide import QtCore, QtGui

class Button(QtGui.QWidget):
    def __init__(self, parent, callback, offset):
        super(Button, self).__init__(parent)
        self.setFixedSize(30, 30)

        self.callback = callback
        self.offset = offset
        self.hover = False
        self.selected = False

        self.position(parent.height())
        self.show()
        parent.installEventFilter(self)

    def eventFilter(self, obj, event):
        if obj == self.parentWidget() and event.type() == QtCore.QEvent.Resize:
            self.position(event.size().height())
        return False

    def mousePressEvent(self, event):
        """ On left-click, call the callback.
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.callback(self)

    def enterEvent(self, event):
        self.hover = True
        self.update()

    def leaveEvent(self, event):
        self.hover = False
        self.update()

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        if self.hover:
            painter.setBackground(QtGui.QColor(255, 0, 0))
        else:
            painter.setBackground(QtGui.QColor(0, 255, 0))
        painter.eraseRect(self.rect())

    def position(self, height):
        """ Moves the button to an appropriate y position.
        """
        self.move(20, (height / 2) + self.offset * (self.height() + 10))

    def fill(self, painter):
        if self.hover:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(255, 255, 255, 80)))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(255, 255, 255, 50)))
        if self.selected:
            painter.setPen(QtGui.QPen(QtGui.QColor(255, 255, 255, 200), 5))
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(255, 255, 255, 100), 4))
        painter.drawRect(self.rect())

class AddButton(Button):
    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.fill(painter)
        painter.setPen(QtGui.QPen(QtGui.QColor(150, 150, 150), 4))
        painter.drawLine(7, 15, 23, 15)
        painter.drawLine(15, 7, 15, 23)

class MoveButton(Button):
    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.fill(painter)
        painter.setPen(QtGui.QPen(QtGui.QColor(150, 150, 150), 2))
        painter.drawLine(7, 7, 7, 23)
        painter.drawLine(7, 7, 5, 9)
        painter.drawLine(7, 7, 9, 9)
        painter.drawLine(7, 23, 23, 23)
        painter.drawLine(23, 23, 21, 21)
        painter.drawLine(23, 23, 21, 25)

class DelButton(Button):
    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        self.fill(painter)
        painter.setPen(QtGui.QPen(QtGui.QColor(150, 150, 150), 4))
        painter.drawLine(7, 7, 23, 23)
        painter.drawLine(23, 7, 7, 23)


