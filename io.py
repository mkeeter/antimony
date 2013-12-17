import wx
import name

import colors

_colors = {
        name.Name:  colors.yellow,
        float:      colors.blue
}

class IO(wx.Control):
    def __init__(self, parent, T):
        self.type = T
        wx.Control.__init__(self, parent, size=(10, 10))
        self.SetBackgroundColour(_colors.get(self.type, colors.red))
        self.Bind(wx.EVT_MOTION, self.on_motion)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.on_leave_window)

    def on_motion(self, event):
        self.SetBackgroundColour(
                [min(255, c + 75) for c in _colors.get(self.type, colors.red)])
        self.Refresh()
    def on_leave_window(self, event):
        self.SetBackgroundColour(_colors.get(self.type, colors.red))
        self.Refresh()
