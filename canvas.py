import random

import wx

import editor
import point
import node

class Canvas(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        super(Canvas, self).__init__(parent, *args, **kwargs)
        self.Bind(wx.EVT_PAINT, self.paint)

        for i in range(10):
            pt = point.Point('p%i' % i, random.uniform(-10, 10), random.uniform(-10, 10))
            ctrl = point.PointControl(self, pt)
            if random.uniform(0, 10) > 9:   ctrl.open_editor()

        self.center = wx.RealPoint(0, 0)
        self.scale = 10.0 # scale is measured in pixels/mm
        self.mouse_pos = wx.Point(0, 0)
        self.dragging = False

        self.Bind(wx.EVT_LEFT_DOWN, self.start_drag)
        self.Bind(wx.EVT_LEFT_UP, self.stop_drag)
        self.Bind(wx.EVT_MOTION, self.on_motion)
        self.Bind(wx.EVT_SIZE, self.update_children)

    def update_children(self, event=None):
        for c in self.Children:
            c.update()

    def paint(self, event=None):

        dc = wx.PaintDC(self)
        dc.SetBackground(wx.Brush((20, 20, 20)))
        dc.Clear()

        x, y = self.mm_to_pixel(0, 0)
        dc.SetPen(wx.Pen((255, 0, 0), 2))
        dc.DrawLine(x, y, x + 50, y)
        dc.SetPen(wx.Pen((0, 255, 0), 2))
        dc.DrawLine(x, y, x, y - 50)


    def mm_to_pixel(self, x=None, y=None):
        """ Converts an x,y position in mm into an i,j coordinate.
        """
        if x is not None:
            x = int((x - self.center.x) * self.scale + self.Size.x/2)
        if y is not None:
            y = int((self.Size.y/2) - (y - self.center.y) * self.scale)

        if x is not None and y is not None:     return x, y
        elif x is not None:                     return x
        elif y is not None:                     return y

    def start_drag(self, event):    self.dragging = True
    def stop_drag(self, event):     self.dragging = False

    def on_motion(self, event):
        delta = event.GetPosition() - self.mouse_pos
        if self.dragging:
            self.center += wx.RealPoint(-delta.x / self.scale,
                                        delta.y / self.scale)
            self.update_children()
            self.Refresh()
        self.mouse_pos = event.GetPosition()



