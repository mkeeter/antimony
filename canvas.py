import random

from PySide import QtCore, QtGui

import point
import editor

class Canvas(QtGui.QWidget):
    def __init__(self):
        super(Canvas, self).__init__()
        self.setGeometry(0, 900/4, 1440/2, 900/2)
        self.setWindowTitle("kokopuffs")

        self.center = QtCore.QPointF(0, 0)
        self.scale = 10.0 # scale is measured in pixels/mm

        self.scatter_points(1)
        self.show()

    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        painter.setBackground(QtGui.QColor(20, 20, 20))
        painter.eraseRect(self.rect())
        painter.setPen(QtGui.QColor(255, 255, 0))
        painter.drawLine(0, 0, 100, 100)

    def scatter_points(self, n):
        for i in range(n):
            pt = point.Point('p%i' % i, random.uniform(-10, 10), random.uniform(-10, 10))
            ctrl = point.PointControl(self, pt)
            e = editor.Editor(ctrl)
            ctrl.editor = e
            ctrl.raise_()

    def mm_to_pixel(self, x=None, y=None):
        """ Converts an x,y position in mm into a pixel coordinate.
        """
        if x is not None:
            x = int((x - self.center.x()) * self.scale + self.size().width()/2)
        if y is not None:
            y = int((self.center.y() - y) * self.scale + self.size().height()/2)

        if x is not None and y is not None:     return x, y
        elif x is not None:                     return x
        elif y is not None:                     return y

    def pixel_to_mm(self, x=None, y=None):
        """ Converts a pixel location into an x,y coordinate.
        """
        if x is not None:
            x =  (x - self.Size.x/2) / self.scale + self.center.x
        if y is not None:
            y = -((y - self.Size.y/2) / self.scale - self.center.y)
        if x is not None and y is not None:     return wx.RealPoint(x, y)
        elif x is not None:                     return x
        elif y is not None:                     return y

'''
import editor
import point
import node

class Canvas(wx.Panel):
    def __init__(self, parent, *args, **kwargs):
        super(Canvas, self).__init__(parent, *args, **kwargs)
        self.Bind(wx.EVT_PAINT, self.paint)

        self.center = wx.RealPoint(0, 0)
        self.scale = 10.0 # scale is measured in pixels/mm

        self.mouse_pos = wx.Point(0, 0)
        self.drag_target = None

        self.controls = []
        self.connections = []

        self.Bind(wx.EVT_LEFT_DOWN, self.on_left_down)
        self.Bind(wx.EVT_LEFT_UP, self.on_left_up)
        self.Bind(wx.EVT_LEFT_DCLICK, self.on_dclick)
        self.Bind(wx.EVT_MOTION, self.on_motion)

        self.Bind(wx.EVT_SIZE, self.update_children)

        self.scatter_points(4)

    def scatter_points(self, n):
        for i in range(n):
            pt = point.Point('p%i' % i, random.uniform(-10, 10), random.uniform(-10, 10))
            ctrl = point.PointControl(self, pt)
            if random.uniform(0, 10) > 9:   ctrl.open_editor()


    def update_children(self, event=None):
        """ Update Editor panels and NodeControl objects so that they
            are in the correct places in the canvas pixel panel.
        """
        # Update any Editor windows that may be active
        for c in self.Children:     c.update()

        # Update any NodeControl objects
        for c in self.controls:     c.update()

    def make_pick_buffer(self):
        """ Render out all NodeControls into a pick buffer.
            R and G in the pick buffer represent index within the
            self.controls list; B is a per-node value that may be
            used to create multiple selectable regions.
        """
        bmp = wx.EmptyBitmap(*self.Size)
        dc = wx.MemoryDC()
        dc.SelectObject(bmp)

        # Draw each control in a unique color
        for i, c in enumerate(self.controls + self.connections):
            color = ((i+1) % 255, (i+1) / 255)
            c.draw(dc, pick=color)

        self.pick = wx.ImageFromBitmap(bmp)


    def paint(self, event=None):
        """ Update the pick buffer and rendered scene.
        """

        if not self.drag_target:    self.make_pick_buffer()

        dc = wx.PaintDC(self)
        dc.SetBackground(wx.Brush((20, 20, 20)))
        dc.Clear()

        x, y = self.mm_to_pixel(0, 0)
        dc.SetPen(wx.Pen((255, 0, 0), 2))
        dc.DrawLine(x, y, x + 50, y)
        dc.SetPen(wx.Pen((0, 255, 0), 2))
        dc.DrawLine(x, y, x, y - 50)

        for c in self.controls + self.connections:
            c.draw(dc)


    def mm_to_pixel(self, x=None, y=None):
        """ Converts an x,y position in mm into a pixel coordinate.
        """
        if x is not None:
            x = int((x - self.center.x) * self.scale + self.Size.x/2)
        if y is not None:
            y = int((self.center.y - y) * self.scale + self.Size.y/2)

        if x is not None and y is not None:     return x, y
        elif x is not None:                     return x
        elif y is not None:                     return y

    def pixel_to_mm(self, x=None, y=None):
        """ Converts a pixel location into an x,y coordinate.
        """
        if x is not None:
            x =  (x - self.Size.x/2) / self.scale + self.center.x
        if y is not None:
            y = -((y - self.Size.y/2) / self.scale - self.center.y)
        if x is not None and y is not None:     return wx.RealPoint(x, y)
        elif x is not None:                     return x
        elif y is not None:                     return y


    def on_left_down(self, event):
        """ Assigns a target to self.drag_target.
        """
        i = (self.pick.GetRed(*event.GetPosition()) +
             self.pick.GetGreen(*event.GetPosition()) * 255) - 1
        j = self.pick.GetBlue(*event.GetPosition())

        if i >= 0 and i < len(self.controls):
            self.drag_target = self.controls[i].GetDragTarget(j)
        else:
            self.drag_target = self

    def on_dclick(self, event):
        """ On double-click, attempt to open an editor if
            a NodeControl is active.
        """
        i = (self.pick.GetRed(*event.GetPosition()) +
             self.pick.GetGreen(*event.GetPosition()) * 255) - 1
        j = self.pick.GetBlue(*event.GetPosition())

        if i >= 0 and i < len(self.controls):
            self.controls[i].open_editor()

    def on_left_up(self, event):
        """ Releases the current drag target by calling release()
        """
        if self.drag_target:
            self.drag_target.release()
        self.drag_target = None
        self.make_pick_buffer()

    def on_motion(self, event):
        """ Drags the current drag target (if one exists).
        """
        delta = event.GetPosition() - self.mouse_pos
        x, y = self.pixel_to_mm(*event.GetPosition())
        if self.drag_target:
            self.drag_target.drag(
                    x, y,delta.x / self.scale, -delta.y / self.scale)
        self.mouse_pos = event.GetPosition()


    def drag(self, x, y, dx, dy):
        """ Drags the canvas around.
        """
        self.center -= wx.RealPoint(dx, dy)
        self.update_children()

    def release(self):  pass

    def find_input(self, pos):
        """ Hunts through all Editor panels to find one with
            an io.Input control at the given position, returning
            None otherwise.
        """
        for c in self.Children:
            if isinstance(c, editor.Editor):
                i = c.find_input(pos)
                if i is not None:
                    return i
        return None
'''
