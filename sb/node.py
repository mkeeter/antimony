import operator

from PySide import QtCore

import sb.datum as datum
from sb.manager import NodeManager

class Node(QtCore.QObject):
    """ Graph node.  Contains datums (both object and view-related),
        wireframes, and handles.

        Should be deleted by calling self.deleteLater()
    """

    def __init__(self, name):
        """ Node constructor.
            Adds node to master list and sets _deleted to False.
        """
        super(Node, self).__init__(NodeManager)

        # Save a master list of datums to serialize nodes for saving.
        self.object_datums = {'name': datum.NameDatum(self, name)}
        self.view_datums = {}
        self.handles = []

    def proxy(self):
        return NodeProxy(self)

################################################################################

class Node2D(Node):
    """ Graph node with x,y coordinates
    """
    def __init__(self, name, x, y):
        super(Node2D, self).__init__(name)
        self.object_datums['x'] = datum.FloatDatum(self, x)
        self.object_datums['y'] = datum.FloatDatum(self, y)

class Node3D(Node2D):
    """ Graph node with x,y,z coordinates.
    """
    def __init__(self, name, x, y, z):
        super(Node3D, self).__init__(name)
        self.add_datum('z', datum.FloatDatum(self, z))

################################################################################

class NodeProxy(object):
    """ Proxy object that looks up values in the target node's datums.
    """
    def __init__(self, node):
        self._node = node
    def __getattr__(self, n):
        return self._node.object_datums[n].value

