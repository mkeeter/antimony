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
        self.hovering = False
        self.dragging = False

        self.position = (0, 0)
        self.size = (10, 10)
        canvas.controls.append(self)
        self.update()

    def mouse_over(self, over):
        """ Handles mouse motion.
        """
        over = self.is_mouse_over(event.GetPosition())
        if over != self.hovering:
            self.hovering = over
            self.Refresh()

        pos = event.GetPosition()
        if self.dragging:
            self.mouse_drag(pos - self.mouse_pos)
        self.mouse_pos = pos

        # Release the event if we're not using it.
        return over or self.dragging


    def on_click(self, event):
        if self.is_mouse_over(event.GetPosition()):
            self.dragging = event.ButtonDown()
            return True
        else:
            return False


    def mouse_drag(self, delta):
        dx =  delta.x / self.canvas.scale
        dy = -delta.y / self.canvas.scale
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

        dc.SetBrush(wx.Brush(pick if pick else light))
        dc.SetPen(wx.Pen(pick if pick else dark, 2))

        dc.DrawCircle(self.position[0] + self.size[0]/2,
                      self.position[1] + self.size[1]/2,
                      10 if self.hovering or self.dragging else 6)
