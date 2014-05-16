from PySide import QtCore

class NodeManager(QtCore.QObject):
    def __init__(self):
        super(NodeManager, self).__init__()

    def make_dict(self):
        from sb.node import Node
        return {str(n.object_datums['name']._value): n.proxy()
                for n in self.findChildren(Node)}

# Create a single instance of the node manager
NodeManager = NodeManager()

