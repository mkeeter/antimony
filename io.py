import wx
import name

colors = {
        name.Name: (0, 0, 200),
        float:     (0, 200, 0)
}

class IO(wx.Control):
    def __init__(self, parent, type):
        self.type = type
        wx.Control.__init__(self, parent, size=(10, 10))
        self.SetBackgroundColour(colors.get(type, (200, 0, 0)))
        self.Bind(wx.EVT_MOTION, self.on_motion)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.on_leave_window)

    def on_motion(self, event):
        self.SetBackgroundColour(
                [c + 55 for c in colors.get(self.type, (200, 0, 0))])
        self.Refresh()
    def on_leave_window(self, event):
        self.SetBackgroundColour(colors.get(self.type, (200, 0, 0)))
        self.Refresh()
