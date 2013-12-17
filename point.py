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

    def reposition(self):
        """ Move this control to the appropriate position.
        """
        try:    x = self.Parent.mm_to_pixel(x=self.node.x)
        except: x = self.GetPosition().x + self.Size.x / 2

        try:    y = self.Parent.mm_to_pixel(y=self.node.y)
        except: y = self.GetPosition().y + self.Size.y / 2

        self.MoveXY(x - self.Size.x / 2, y - self.Size.y / 2)

    def draw(self, event):
        bmp = wx.EmptyBitmap(*self.Size)
        dc = wx.MemoryDC()
        dc.SelectObject(bmp)

        x, y = self.Size.x / 2, self.Size.y / 2
        light = (200, 200, 200)
        dark  = (100, 100, 100)
        dc.SetBrush(wx.Brush(light))
        dc.SetPen(wx.Pen(dark, 2))
        dc.DrawCircle(x, y, 10 if self.hover else 6)

        self.region = wx.RegionFromBitmapColour(bmp, wx.Colour(0, 0, 0, 0))

        wx.PaintDC(self).Blit(0, 0, self.Size.x, self.Size.y, dc, 0, 0)


