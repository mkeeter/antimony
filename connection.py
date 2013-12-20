import wx

class Connection(wx.Control):
    def __init__(self, parent, origin):
        super(Connection, self).__init__(parent, size=(10, 10))
        self.origin = origin
        self.destination = None

        # Get mouse position within the canvas object.
        self.mouse_pos = self.origin_pos = (
            origin.GetPosition() +
            origin.Parent.GetPosition() + wx.Point(5, 5))

        self.Bind(wx.EVT_PAINT, self.draw)
        self.Bind(wx.EVT_MOTION, self.on_motion)
        self.Bind(wx.EVT_LEFT_UP, self.on_release)

        self.update()

    def on_motion(self, event):
        self.mouse_pos = (
                event.GetPosition() +
                self.origin.GetPosition() +
                self.origin.Parent.GetPosition())
        self.update()

    def draw(self, event):
        dc = wx.PaintDC(self)
        dc.SetPen(wx.Pen(self.origin.color, 4))
        if ((self.mouse_pos.x > self.origin_pos.x) ^
            (self.mouse_pos.y > self.origin_pos.y)):
            dc.DrawLine(5, self.Size.y - 5, self.Size.x - 5, 5)
        else:
            dc.DrawLine(5, 5, self.Size.x - 5, self.Size.y - 5)

    def get_bounds(self):
        xmin = min(self.mouse_pos.x - 5, self.origin_pos.x - 5)
        ymin = min(self.mouse_pos.y - 5, self.origin_pos.y - 5)
        xmax = max(self.mouse_pos.x + 5, self.origin_pos.x + 5)
        ymax = max(self.mouse_pos.y + 5, self.origin_pos.y + 5)
        return xmin, ymin, xmax, ymax

    def update(self, event=None):
        if not self.destination:
            xmin, ymin, xmax, ymax = self.get_bounds()
            self.Size = (xmax - xmin, ymax - ymin)
            self.MoveXY(xmin, ymin)

    def on_release(self, event):
        self.mouse_pos = (
            event.GetPosition() +
            self.origin.GetPosition() +
            self.origin.Parent.GetPosition())
        d = self.Parent.find_input(self.mouse_pos)
        if d is None:
            wx.CallAfter(self.Destroy)
        else:
            self.destination = d
            self.Unbind(wx.EVT_MOTION)
            self.Unbind(wx.EVT_LEFT_UP)





current = None
