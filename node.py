class Node(object):
    """ Graph node.  Contains one or more Datum objects.
    """
    def __init__(self):
        """ Node constructor.
            Adds node to master list and sets _deleted to False.
        """
        self._deleted = False
        nodes.append(self)
        pass

    def __del__(self):
        """ Node destructor
            If _deleted is still False, calls self.delete.
        """
        if not self._deleted:    self.delete()

    def delete(self):
        """ Removes node from master list and sets _deleted to True.
        """
        if nodes is not None:   nodes.remove(self)
        self._deleted = True

    def __getattr__(self, v):
        return self.__getattribute__('_'+v).value()

# Master list of nodes
nodes = []
def dict():     return {n._name.expr[1:-1]: n for n in nodes}

################################################################################

import wx
import editor

class NodeControl(wx.Control):
    def __init__(self, parent, node, *args, **kwargs):
        super(NodeControl, self).__init__(parent, *args, **kwargs)
        self.node = node
        self.hover = False
        self.drag  = False
        self.mouse_pos = wx.Point(0, 0)

        self.Bind(wx.EVT_MOTION, self.on_motion)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.on_leave_window)
        self.Bind(wx.EVT_PAINT, self.draw)
        self.Bind(wx.EVT_LEFT_DOWN, self.start_drag)
        self.Bind(wx.EVT_LEFT_UP, self.stop_drag)

        self.editor = None

    def on_motion(self, event):
        """ Sets 'hover' to True if the cursor is over the control.

            Checks against the 'region' attribute if it is defined, otherwise
            assumes that the control fills the entire region.
        """
        if hasattr(self, 'region'):
            hover = self.region.Contains(*event.GetPosition())
        else:
            hover = True
        if hover != self.hover:
            self.hover = hover
            self.Refresh()

        pos = self.GetPosition() + event.GetPosition()
        delta = pos - self.mouse_pos
        if self.drag:
            dx =  delta.x / self.Parent.scale
            dy = -delta.y / self.Parent.scale
            if self.node._x.simple():
                self.node._x.expr = str(float(self.node._x.expr) + dx)
            if self.node._y.simple():
                self.node._y.expr = str(float(self.node._y.expr) + dy)
            self.update()
            if self.editor:     self.editor.update()
        self.mouse_pos = pos

    def on_leave_window(self, event):
        if self.hover:
            self.hover = False
            self.Refresh()

    def draw(self, event):
        raise NotImplementedError(
                "NodeControl.draw must be defined in subclass.")

    def open_editor(self, event=None):
        if self.hover and not self.editor:
            self.editor = editor.MakeEditor(self)
            self.Raise()

    def start_drag(self, event):    self.drag = True
    def stop_drag(self, event):     self.drag = False


