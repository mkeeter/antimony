import random

from PySide import QtCore, QtGui

import point
import editor

class Canvas(QtGui.QWidget):
    def __init__(self):
        super(Canvas, self).__init__()
        self.setGeometry(0, 900/4, 1440/2, 900/2)
        self.setWindowTitle("kokopuffs")

        self.center = QtCore.QPointF(0, 0)
        self.scale = 10.0 # scale is measured in pixels/mm

        self.scatter_points(2)
        self.show()

    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        painter.setBackground(QtGui.QColor(20, 20, 20))
        painter.eraseRect(self.rect())
        painter.setPen(QtGui.QColor(255, 255, 0))
        painter.drawLine(0, 0, 100, 100)

    def resizeEvent(self, event):
        for c in self.findChildren(QtGui.QWidget):
            if hasattr(c, 'sync'):  c.sync()

    def scatter_points(self, n):
        for i in range(n):
            pt = point.Point('p%i' % i, random.uniform(-10, 10), random.uniform(-10, 10))
            ctrl = point.PointControl(self, pt)
            e = editor.Editor(ctrl)
            ctrl.editor = e
            ctrl.raise_()

    def mm_to_pixel(self, x=None, y=None):
        """ Converts an x,y position in mm into a pixel coordinate.
        """
        if x is not None:
            x = int((x - self.center.x()) * self.scale + self.size().width()/2)
        if y is not None:
            y = int((self.center.y() - y) * self.scale + self.size().height()/2)

        if x is not None and y is not None:     return x, y
        elif x is not None:                     return x
        elif y is not None:                     return y

    def pixel_to_mm(self, x=None, y=None):
        """ Converts a pixel location into an x,y coordinate.
        """
        if x is not None:
            x =  (x - self.Size.x/2) / self.scale + self.center.x
        if y is not None:
            y = -((y - self.Size.y/2) / self.scale - self.center.y)
        if x is not None and y is not None:     return wx.RealPoint(x, y)
        elif x is not None:                     return x
        elif y is not None:                     return y

    def find_input(self, pos):
        """ Hunts through all Editor panels to find one with
            a connection.Input control at the given position, returning
            None otherwise.
        """
        for c in self.findChildren(editor.Editor):
            i = c.find_input(pos)
            if i is not None:   return i
        return None

