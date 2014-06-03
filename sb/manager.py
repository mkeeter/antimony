from PySide import QtCore

class NodeManager(QtCore.QObject):
    def __init__(self):
        super(NodeManager, self).__init__()

    def make_dict(self):
        from sb.nodes.node import Node
        return {str(n.get_datum('name')._value): n.proxy()
                for n in self.findChildren(Node) if hasattr(n, 'datums')}

    def get_name(self, node_class):
        """ Finds an appropriate unused name for the given node class.
        """
        i = 0
        while True:
            name = '%s%i' % (node_class.name_prefix, i)
            if name in [str(f.datums['name']._value)
                        for f in self.findChildren(node_class)]:
                i += 1
            else:
                return name

# Create a single instance of the node manager
NodeManager = NodeManager()

