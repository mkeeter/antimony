from PySide import QtCore, QtGui

import node
import datum

class Point(node.Node):

    def __init__(self, name, x, y):

        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)
        self._name = datum.NameDatum(self, name)

        self.inputs = [(i, getattr(self, '_'+i)) for i in ('name','x','y')]

        super(Point, self).__init__()

################################################################################

class PointControl(node.NodeControl):
    def __init__(self, canvas, target):
        super(PointControl, self).__init__(canvas, target)
        self.setFixedSize(20, 20)
        self.update()

    def mousePressEvent(self, event):
        print "Mouse press at", event.x(), event.y()

    def drag(self, x, y, dx, dy):
        """ Drag this node by attempting to change its x and y coordinates
            dx and dy should be floating-point values.
        """
        if self.node._x.simple():
            self.node._x.set_expr(str(float(self.node._x.get_expr()) + dx))
        if self.node._y.simple():
            self.node._y.set_expr(str(float(self.node._y.get_expr()) + dy))


    def update(self):
        """ Move this control to the appropriate position.
        """
        px, py = self.x(), self.y()
        width, height = self.width(), self.height()

        try:    x = self.canvas.mm_to_pixel(x=self.node.x) - width/2
        except: x = px

        try:    y = self.canvas.mm_to_pixel(y=self.node.y) - height/2
        except: y = py

        if x != px or y != py:
            self.move(x, y)


    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        painter.setBackground(QtGui.QColor(200, 100, 100))
        painter.eraseRect(self.rect())
        self.paint(painter)

    def paint(self, qp):
        width, height = self.width(), self.height()
        light = (200, 200, 200)
        dark  = (100, 100, 100)

        qp.setBrush(QtGui.QBrush(
            QtGui.QColor(*light)))
        qp.setPen(QtGui.QPen(
            QtGui.QColor(*dark), 2))

        qp.drawEllipse((width - 16) / 2, (height - 16) / 2, 16, 16)
