from PySide import QtCore, QtGui

import base

class CircleControl(base.NodeControl):

    def __init__(self, canvas, target):
        super(CircleControl, self).__init__(canvas, target)

        self.drag_pt = False
        self.drag_r  = False

        self.hover_pt = False
        self.hover_r  = False

        self.position = QtCore.QPointF()
        self.r = 0

        self.sync()
        self.make_masks()

    def mousePressEvent(self, event):
        if event.button() != QtCore.Qt.LeftButton:  return
        self.mouse_pos = self.mapToParent(event.pos())
        if self.center_mask.contains(event.pos()):
            self.drag_pt = True
        else:
            self.drag_r = True

    def mouseMoveEvent(self, event):
        p = self.mapToParent(event.pos())
        if self.drag_pt or self.drag_r:
            delta = p - self.mouse_pos
            scale = self.canvas.scale
            dx, dy = delta.x() / scale, -delta.y() / scale
            if self.drag_pt:
                self.drag_center(dx, dy)
            elif self.drag_r:
                x, y = self.canvas.pixel_to_mm(p.x(), p.y())
                self.drag_ring(x, y, dx, dy)

        elif not self.hover_pt and self.center_mask.contains(event.pos()):
            self.hover_pt = True
            self.update()
        elif not self.hover_r and self.ring_mask.contains(event.pos()):
            self.hover_r = True
            self.update()
        self.mouse_pos = p

    def drag_center(self, dx, dy):
        """ Drag this node by attempting to change its x and y coordinates
            dx and dy should be floating-point values.
        """
        if self.node._x.simple():
            self.node._x.set_expr(str(float(self.node._x.get_expr()) + dx))
        if self.node._y.simple():
            self.node._y.set_expr(str(float(self.node._y.get_expr()) + dy))


    def drag_ring(self, x, y, dx, dy):
        x -= self.position.x()
        y -= self.position.y()
        norm = (x**2 + y**2)**0.5
        dr = dx * x/norm + dy * y/norm
        if self.node._r.simple():
            self.node._r.set_expr(str(float(self.node._r.get_expr()) + dr))


    def mouseDoubleClickEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.open_editor()

    def mouseReleaseEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            self.drag_pt = False
            self.drag_r = False

    def leaveEvent(self, event):
        self.hover_pt = False
        self.hover_r = False
        self.update()

    def make_masks(self):
        for n in ['center','ring']:
            func = getattr(self, 'draw_' + n)
            painter = QtGui.QPainter()
            bitmap = QtGui.QBitmap(self.size())
            bitmap.clear()

            painter.begin(bitmap)
            func(painter, True)
            painter.end()

            setattr(self, n + '_mask', QtGui.QRegion(bitmap))

        self.setMask(self.center_mask.united(self.ring_mask))


    def sync(self):
        """ Move and scale this control to the appropriate position.
            Use self.position and self.r if eval fails.
        """
        try:    x = self.node.x
        except: x = self.position.x()

        try:    y = self.node.y
        except: y = self.position.y()

        try:    r = self.node.r
        except: r = self.r

        i = self.canvas.mm_to_pixel(x=x)
        j = self.canvas.mm_to_pixel(y=y)
        di = max(30, self.canvas.mm_to_pixel(x=x+r) -
                     self.canvas.mm_to_pixel(x=x-r) + 4)
        dj = max(30, self.canvas.mm_to_pixel(y=y-r) -
                     self.canvas.mm_to_pixel(y=y+r) + 4)

        # Decide whether anything has changed.
        changed = self.position != QtCore.QPointF(x, y) or self.r != r

        self.move(i - di/2, j - dj/2)
        self.resize(di, dj)

        # Cache these values
        self.position = QtCore.QPointF(x, y)
        self.r = r

        if changed:
            self.make_masks()
            self.update()
        if self.editor:     self.editor.sync()


    def draw_center(self, painter, mask=False):
        """ Draws a circle at the center of the widget.
        """
        width, height = self.width(), self.height()
        light = (200, 200, 200)
        dark  = (100, 100, 100)

        if mask:
            painter.setBrush(QtGui.QBrush(QtCore.Qt.color1))
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 2))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(*light)))
            painter.setPen(QtGui.QPen(QtGui.QColor(*dark), 2))

        if mask:                                d = 22
        elif self.hover_pt or self.drag_pt:     d = 20
        else:                                   d = 14

        painter.drawEllipse((width - d) / 2, (height - d) / 2, d, d)


    def draw_ring(self, painter, mask=False):
        """ Draws the ring around the widget.
        """
        width, height = self.width(), self.height()

        light = (200, 200, 200)

        i = self.canvas.mm_to_pixel(x=self.position.x())
        d = (self.canvas.mm_to_pixel(x=self.position.x()+self.r) - i) * 2

        if d <= 0:  return

        if mask:                            r = 6
        elif self.hover_r or self.drag_r:   r = 4
        else:                               r = 2

        if mask:
            painter.setBrush(QtGui.QBrush())
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, r))
        else:
            painter.setBrush(QtGui.QBrush())
            painter.setPen(QtGui.QPen(QtGui.QColor(*light), r))

        painter.drawEllipse((width - d) / 2, (height - d) / 2, d, d)


    def paintEvent(self, painter):
        painter = QtGui.QPainter(self)
        self.draw_center(painter)
        self.draw_ring(painter)
