import operator

from PySide import QtCore

import sb.datum as datum
from sb.manager import NodeManager

class Node(QtCore.QObject):
    """ Graph node.  Contains datums (both object and view-related),
        wireframes, and handles.

        Should be deleted by calling self.deleteLater()
    """
    changed = QtCore.Signal()

    def __init__(self, name):
        """ Node constructor.
        """

        # Call the QObject constructor with the NodeManager as parent
        super(Node, self).__init__(NodeManager)

        # Here are object datums (which relate to generated shapes)
        # and view datums (which modify rendering properties)
        self.object_datums = {}
        self.view_datums = {}

        # Create an empty list of UI handles
        self.handles = []

        # Every object has a name (which is used by NodeManager when creating
        # its dictionary of NodeProxy objects.
        self.object_datums['name'] = datum.NameDatum(self, name)

        # Set _post_init to be called when control returns to the event loop
        # (which will happen after the superclass finishes its own __init__)
        self._post_init_timer = QtCore.QTimer()
        self._post_init_timer.singleShot(0, self._post_init)

    def _post_init(self):
        del self._post_init_timer
        for d in self.object_datums, self.view_datums:
            for a in d.itervalues():
                a.changed.connect(self.changed)


    def proxy(self):
        return NodeProxy(self)

################################################################################

class Node2D(Node):
    """ Node with x,y coordinates
    """
    def __init__(self, name, x, y):
        super(Node2D, self).__init__(name)
        self.object_datums['x'] = datum.FloatDatum(self, x)
        self.object_datums['y'] = datum.FloatDatum(self, y)

class Node3D(Node):
    """ Node with x,y,z coordinates.
    """
    def __init__(self, name, x, y, z):
        super(Node3D, self).__init__(name)
        self.object_datums['x'] = datum.FloatDatum(self, x)
        self.object_datums['y'] = datum.FloatDatum(self, y)
        self.object_datums['z'] = datum.FloatDatum(self, z)

################################################################################

class NodeProxy(object):
    """ Proxy object that looks up values in the target node's datums.
    """
    def __init__(self, node):
        self._node = node
    def __getattr__(self, n):
        return self._node.object_datums[n].value

