import wx
import name

import colors

_colors = {
        name.Name:  colors.yellow,
        float:      colors.blue
}

class IO(wx.Control):
    def __init__(self, parent, datum):
        wx.Control.__init__(self, parent, size=(10, 10))

        self.datum = datum
        self.color = _colors.get(self.datum.type, colors.red)

        self.SetBackgroundColour(self.color)
        self.Bind(wx.EVT_MOTION, self.on_motion)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.on_leave_window)

    def on_motion(self, event):
        self.SetBackgroundColour(
                [min(255, c + 75) for c in self.color])
        self.Refresh()

    def on_leave_window(self, event):
        self.SetBackgroundColour(self.color)
        self.Refresh()

class Input(IO):
    def __init__(self, parent, datum):
        super(Input, self).__init__(parent, datum)

class Output(IO):
    def __init__(self, parent, datum):
        super(Output, self).__init__(parent, datum)
