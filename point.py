import wx

import node
import datum

class Point(node.Node):

    def __init__(self, name, x, y):

        self._x = datum.FloatDatum(x)
        self._y = datum.FloatDatum(y)
        self._name = datum.NameDatum(name)

        self.inputs = [(i, getattr(self, '_'+i)) for i in ('name','x','y')]

        super(Point, self).__init__()

################################################################################

class PointControl(node.NodeControl):
    def __init__(self, parent, target):
        super(PointControl, self).__init__(parent, target, size=(30, 30))

    def reposition(self):
        """ Move this control to the appropriate position.
        """
        x, y = self.Parent.mm_to_pixel(self.node.x, self.node.y)
        self.MoveXY(x - self.Size.x / 2, y - self.Size.y / 2)

    def draw(self, event):
        dc = wx.PaintDC(self)
        dc.SetBackground(wx.Brush((0, 0, 255, 255)))
        dc.Clear()

