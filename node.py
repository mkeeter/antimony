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

class NodeControl(wx.Control):
    def __init__(self, parent, node, *args, **kwargs):
        super(NodeControl, self).__init__(parent, *args, **kwargs)
        self.node = node
        self.hover = False

        self.Bind(wx.EVT_MOTION, self.on_motion)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.on_leave_window)
        self.Bind(wx.EVT_PAINT, self.draw)

    def on_motion(self, event):
        if not self.hover:
            self.hover = True
            self.Refresh()

    def on_leave_window(self, event):
        if self.hover:
            self.hover = False
            self.Refresh()

    def draw(self, event):
        raise NotImplementedError(
                "NodeControl.draw must be defined in subclass.")

