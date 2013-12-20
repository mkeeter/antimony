import wx
import name

import colors
import connection

_colors = {
        name.Name:  colors.yellow,
        float:      colors.blue
}

class IO(wx.Control):
    def __init__(self, parent, datum):
        wx.Control.__init__(self, parent, size=(10, 10))

        self.datum = datum
        self.color = _colors.get(self.datum.type, colors.red)
        self.region = wx.Region(0, 0, 10, 10)

        self.SetBackgroundColour(self.color)


class Input(IO):
    def __init__(self, parent, datum):
        super(Input, self).__init__(parent, datum)

    def mouse_hit(self, pos):
        """ Checks if this input contains a (canvas) coordinate.
        """
        return self.region.Contains(
            *(pos - self.GetPosition() - self.Parent.GetPosition()))

class Output(IO):
    def __init__(self, parent, datum):
        super(Output, self).__init__(parent, datum)
        self.Bind(wx.EVT_LEFT_DOWN,    self.on_click)
        self.Bind(wx.EVT_LEFT_UP,      self.on_release)
        self.Bind(wx.EVT_MOTION,       self.on_motion)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.on_motion)
        self.connection = None

    def on_click(self, event):
        print self.Parent.Parent
        self.connection = connection.Connection(self.Parent.Parent, self)
        wx.PostEvent(self.connection, event)

    def on_release(self, event):
        if self.connection and self.connection.destination is None:
            wx.PostEvent(self.connection, event)
        else:
            event.Skip()

    def on_motion(self, event):

        # First, check to see if we should be forwarding events to a
        # connection object that's being created.
        if self.connection and self.connection.destination is None:
            wx.PostEvent(self.connection, event)
        elif self.region.Contains(*event.GetPosition()):
            self.SetBackgroundColour(
                    [min(255, c + 75) for c in self.color])
            self.Refresh()
        else:
            self.SetBackgroundColour(self.color)
            self.Refresh()
            event.Skip()
