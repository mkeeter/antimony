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
        self.setFixedSize(30, 30)

        self.dragging = False
        self.hovering = False
        self.make_mask()
        self.sync()

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.mouse_pos = self.mapToParent(event.pos())
            self.dragging = True

    def mouseDoubleClickEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.open_editor()

    def mouseReleaseEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.dragging = False

    def mouseMoveEvent(self, event):
        p = self.mapToParent(event.pos())
        if self.dragging:
            delta = p - self.mouse_pos
            scale = self.parentWidget().scale
            self.drag(delta.x() / scale, -delta.y() / scale)
        self.mouse_pos = p

    def enterEvent(self, event):
        self.hovering = True
        self.update()

    def leaveEvent(self, event):
        self.hovering = False
        self.update()

    def drag(self, dx, dy):
        """ Drag this node by attempting to change its x and y coordinates
            dx and dy should be floating-point values.
        """
        if self.node._x.simple():
            self.node._x.set_expr(str(float(self.node._x.get_expr()) + dx))
        if self.node._y.simple():
            self.node._y.set_expr(str(float(self.node._y.get_expr()) + dy))


    def sync(self):
        """ Move this control to the appropriate position.
            Use self.position (cached) if eval fails.
        """
        try:    x = self.node.x
        except: x = self.position.x()

        try:    y = self.node.y
        except: y = self.position.y()

        self.move(self.canvas.mm_to_pixel(x=x) - self.width()/2,
                  self.canvas.mm_to_pixel(y=y) - self.height()/2)

        self.position = QtCore.QPointF(x, y)


    def paintEvent(self, paintEvent):
        """ On paint event, paint oneself.
        """
        self.paint(QtGui.QPainter(self))

    def paint(self, qp, mask=False):
        """ Paint either the point or a mask for the point.
        """
        width, height = self.width(), self.height()
        light = (200, 200, 200)
        dark  = (100, 100, 100)

        if mask:
            qp.setBrush(QtGui.QBrush(QtCore.Qt.color1))
            qp.setPen(QtGui.QPen(QtCore.Qt.color1, 2))
        else:
            qp.setBrush(QtGui.QBrush(QtGui.QColor(*light)))
            qp.setPen(QtGui.QPen(QtGui.QColor(*dark), 2))

        if mask:                                d = 22
        elif self.hovering or self.dragging:    d = 20
        else:                                   d = 14

        qp.drawEllipse((width - d) / 2, (height - d) / 2, d, d)

    def make_mask(self):
        """ Render a mask and set it to this widget's mask.
        """
        painter = QtGui.QPainter()
        bitmap = QtGui.QBitmap(self.size())
        bitmap.clear()

        painter.begin(bitmap)
        self.paint(painter, True)
        painter.end()

        self.setMask(bitmap)
