import operator

from PySide import QtCore

import sb.datum as datum
from sb.manager import NodeManager

class Node(QtCore.QObject):
    """ Graph node.  Contains datums in the self.datums list and
        a control object in self.control

        Should be deleted by calling self.deleteLater()
    """

    def __init__(self, canvas, name):
        """ Node constructor.
        """
        # Call the QObject constructor with the NodeManager as parent
        super(Node, self).__init__(NodeManager)
        self.datums = [datum.NameDatum('name', self, name)]

    def get_datum(self, n):
        """ Returns the datum with the given name.
        """
        return [d for d in self.datums if d.name == n][0]

    def proxy(self):
        return NodeProxy(self)

################################################################################

class Node2D(Node):
    """ Node with x,y coordinates
    """
    def __init__(self, canvas, name, x, y):
        super().__init__(canvas, name)
        self.datums += [datum.FloatDatum('x', self, x),
                        datum.FloatDatum('y', self, y)]

class Node3D(Node):
    """ Node with x,y,z coordinates.
    """
    def __init__(self, canvas, name, x, y, z):
        super().__init__(canvas, name)
        self.datums += [datum.FloatDatum('x', self, x),
                        datum.FloatDatum('y', self, y),
                        datum.FloatDatum('z', self, z)]

################################################################################

class NodeProxy(object):
    """ Proxy object that looks up values in the target node's datums.
    """
    def __init__(self, node):
        self._node = node
    def __getattr__(self, n):
        return self._node.get_datum(n).value

