import math

from PySide import QtCore, QtGui

class ViewButton(QtGui.QWidget):
    def __init__(self, parent, alpha, beta, callback):
        super(ViewButton, self).__init__(parent)
        self.setFixedSize(20, 20)
        self.alpha = alpha
        self.beta = beta
        self.callback = callback
        self.hover = False

        self.show()

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.callback(self.alpha, self.beta)

    def enterEvent(self, event):
        self.hover = True
        self.update()

    def leaveEvent(self, event):
        self.hover = False
        self.update()

    def paintEvent(self, event):
        painter = QtGui.QPainter(self)
        if self.hover:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(255, 255, 255, 80)))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(255, 255, 255, 50)))
        painter.setPen(QtGui.QPen(QtGui.QColor(150, 150, 150), 2))
        painter.drawRect(self.rect())

class ViewTool(QtGui.QWidget):
    def __init__(self, parent, callback):
        super(ViewTool, self).__init__(parent)

        top = ViewButton(self, 0, 0, callback)
        top.move(top.width(), 1)

        front = ViewButton(self, 0, -math.pi/2, callback)
        front.move(front.width(), front.height())

        left = ViewButton(self, math.pi/2, -math.pi/2, callback)
        left.move(1, left.height())

        right = ViewButton(self, -math.pi/2, -math.pi/2, callback)
        right.move(2*right.width()-1, right.height())

        back = ViewButton(self, -math.pi, -math.pi/2, callback)
        back.move(3*back.width()-1, back.height())

        bottom = ViewButton(self, 0, -math.pi, callback)
        bottom.move(bottom.width(), 2*bottom.height() - 1)

        self.setFixedSize(bottom.width()*4, bottom.height()*3)

        self.move(parent.width() - self.width() - 20, 20)

        mask = QtGui.QRegion()
        for w in self.findChildren(ViewButton):
            mask |= QtGui.QRegion(w.geometry())
        self.setMask(mask)

        parent.installEventFilter(self)
        self.show()

    def eventFilter(self, obj, event):
        if obj == self.parentWidget() and event.type() == QtCore.QEvent.Resize:
            self.move(event.size().width() - self.width() - 20, 20)
        return False

