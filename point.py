import wx

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
        super(PointControl, self).__init__(canvas, target, size=(30, 30))

        self.position = (0, 0)
        self.size = (10, 10)
        canvas.controls.append(self)
        self.update()


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
        px, py = self.position
        try:    x = self.canvas.mm_to_pixel(x=self.node.x) - self.size[0]/2
        except: x = px

        try:    y = self.canvas.mm_to_pixel(y=self.node.y) - self.size[1]/2
        except: y = py

        if x != px or y != py:
            self.position = (x, y)
            self.canvas.Refresh()


    def draw(self, dc, pick=False):
        x, y = self.size[0] / 2, self.size[0] / 2
        light = (200, 200, 200)
        dark  = (100, 100, 100)

        dc.SetBrush(wx.Brush(pick + (0,) if pick else light))
        dc.SetPen(wx.Pen(pick + (0,) if pick else dark, 2))

        dc.DrawCircle(self.position[0] + self.size[0]/2,
                      self.position[1] + self.size[1]/2, 6)
