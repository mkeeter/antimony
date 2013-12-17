import wx

import node
import datum

class Point(node.Node):

    def __init__(self, name, x, y):

        self._x = datum.FloatDatum(x)
        self._y = datum.FloatDatum(y)
        self._name = datum.NameDatum(name)

        self.inputs = [(i, getattr(self, '_'+i)) for i in ('name','x','y')]

        super(Point, self).__init__()

################################################################################

class PointControl(node.NodeControl):
    def __init__(self, parent, target):
        super(PointControl, self).__init__(parent, target, size=(30, 30))
        self.Bind(wx.EVT_LEFT_DCLICK, self.open_editor)
        self.reposition()

    def reposition(self):
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
        dc.DrawCircle(x, y, 10 if self.hover or self.drag else 6)

        self.region = wx.RegionFromBitmapColour(bmp, wx.Colour(0, 0, 0, 0))

        wx.PaintDC(self).Blit(0, 0, self.Size.x, self.Size.y, dc, 0, 0)


