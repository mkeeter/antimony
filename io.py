import wx
import name

colors = {
        name.Name:   (0, 0, 255),
        float: (0, 255, 0)
}

class IO(wx.Control):
    def __init__(self, parent, type):
        wx.Control.__init__(self, parent, size=(10, 10))
        self.SetBackgroundColour(colors.get(type, (255, 0, 0)))
