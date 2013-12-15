import wx

import editor
import point

class Canvas(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        wx.Panel.__init__(self, parent, *args, **kwargs)
        self.Bind(wx.EVT_PAINT, self.paint)

        boop = editor.Editor(self, point.Point('asdf',2,2))
        boop.MoveXY(20, 20)

    def paint(self, event=None):
        dc = wx.PaintDC(self)
        dc.SetBackground(wx.Brush((20, 20, 20)))
        dc.Clear()

