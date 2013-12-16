import wx

import editor
import point

class Canvas(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.Bind(wx.EVT_PAINT, self.paint)

        pt = point.Point('asdf',0, 0)
        ctrl = point.PointControl(self, pt)
        ed = editor.Editor(self, pt)
        ed.MoveXY(20, 20)

        self.center = wx.Point(0, 0)
        self.scale = 10 # scale is measured in pixels/mm

    def paint(self, event=None):
        for c in self.Children: c.reposition()

        dc = wx.PaintDC(self)
        dc.SetBackground(wx.Brush((20, 20, 20)))
        dc.Clear()

    def mm_to_pixel(self, x, y):
        """ Converts an x,y position in mm into an i,j coordinate.
        """
        return (int((x - self.center.x) * self.scale + self.Size.x/2),
                int((self.Size.y/2) - (y - self.center.y) * self.scale))


