import wx

class Connection(object):
    def __init__(self, canvas, origin):
        """ Constructor for an floating connection.
            origin should be a io.Output object.
        """
        self.canvas = canvas
        self.origin = origin
        self.destination = None

        # Get mouse position within the canvas object.
        self.mouse_pos = self.origin_pos = self.canvas.pixel_to_mm(*(
            self.origin.GetPosition() +
            self.origin.Parent.GetPosition() + wx.Point(5, 5)))

    def on_motion(self, event):
        """ Update mouse position and redraws the canvas.
            The event should be triggered from an io.Output control.
        """
        self.mouse_pos = self.canvas.pixel_to_mm(*(
            event.GetPosition() +
            self.origin.GetPosition() + self.origin.Parent.GetPosition()))
        self.canvas.Refresh()


    def draw(self, dc, pick=False):
        dc.SetPen(wx.Pen(pick + (0,) if pick else self.origin.color, 4))
        if ((self.mouse_pos.x > self.origin_pos.x) ^
            (self.mouse_pos.y > self.origin_pos.y)):
            dc.DrawLinePoint(self.canvas.mm_to_pixel(self.origin_pos.x,
                                                     self.origin_pos.y),
                             self.canvas.mm_to_pixel(self.mouse_pos.x,
                                                     self.mouse_pos.y))

        else:
            dc.DrawLinePoint(self.canvas.mm_to_pixel(self.mouse_pos.x,
                                                     self.mouse_pos.y),
                             self.canvas.mm_to_pixel(self.origin_pos.x,
                                                     self.origin_pos.y))


    def on_release(self, event):
        return
        self.mouse_pos = (
            event.GetPosition() +
            self.origin.GetPosition() +
            self.origin.Parent.GetPosition())
        d = self.Parent.find_input(self.mouse_pos)
        if d is None:
            self.canvas.connections.remove(self)
            self.destination = False
        else:
            self.destination = d





current = None
