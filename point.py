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
    def __init__(self, parent, target):
        super(PointControl, self).__init__(parent, target, size=(30, 30))
        self.Bind(wx.EVT_LEFT_DCLICK, self.open_editor)
        self.hovering = False
        self.dragging = False

    def is_mouse_over(self, position):
        return self.region.Contains(*position)

    def on_motion(self, event):
        over = self.is_mouse_over(event.GetPosition())
        if over != self.hovering:
            self.hovering = over
            self.Refresh()

        pos = self.GetPosition() + event.GetPosition()
        if self.dragging:
            self.mouse_drag(pos - self.mouse_pos)
        self.mouse_pos = pos

        # Release the event if we're not using it.
        if not over and not self.dragging:  event.Skip()


    def on_click(self, event):
        self.dragging = (event.ButtonDown() and
                         self.is_mouse_over(event.GetPosition()))


    def mouse_drag(self, delta):
        dx =  delta.x / self.Parent.scale
        dy = -delta.y / self.Parent.scale
        if self.node._x.simple():
            self.node._x.set_expr(str(float(self.node._x.get_expr()) + dx))
        if self.node._y.simple():
            self.node._y.set_expr(str(float(self.node._y.get_expr()) + dy))


    def update(self):
        """ Move this control to the appropriate position.
        """
        px, py = self.GetPosition()
        try:    x = self.Parent.mm_to_pixel(x=self.node.x) - self.Size.x/2
        except: x = px

        try:    y = self.Parent.mm_to_pixel(y=self.node.y) - self.Size.y/2
        except: y = py

        if x != px or y != py:
            self.MoveXY(x, y)

    def draw(self, event):
        bmp = wx.EmptyBitmap(*self.Size)
        dc = wx.MemoryDC()
        dc.SelectObject(bmp)

        x, y = self.Size.x / 2, self.Size.y / 2
        light = (200, 200, 200)
        dark  = (100, 100, 100)
        dc.SetBrush(wx.Brush(light))
        dc.SetPen(wx.Pen(dark, 2))
        dc.DrawCircle(x, y, 10 if self.hovering or self.dragging else 6)

        self.region = wx.RegionFromBitmapColour(bmp, wx.Colour(0, 0, 0, 0))

        wx.PaintDC(self).Blit(0, 0, self.Size.x, self.Size.y, dc, 0, 0)


