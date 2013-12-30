class Connection(object):
    """ Represents a connection between two datums.
        The target datum tracks the source datum's value.
    """
    def __init__(self, datum):
        self.source = datum
        self.source.outputs.append(self)
        self.target = None
        self.control = None

    def deflate(self, nodes):
        """ Returns a set of connection indices
            (source node, datum; target node, datum)
        """
        if not self.target:     return None
        source_node = nodes.index(self.source.node)
        source_datum = [
                d[1] for d in nodes[source_node].datums].index(self.source)
        target_node = nodes.index(self.target.node)
        target_datum = [
                d[1] for d in nodes[target_node].datums].index(self.target)
        return (source_node, source_datum, target_node, target_datum)

    @classmethod
    def inflate(cls, deflated, nodes):
        """ Inflates a given connection, attaching it to the correct nodes
            and returning the new object.
        """
        source_node, source_datum, target_node, target_datum = deflated
        c = cls(nodes[source_node].datums[source_datum])
        c.connect_to(nodes[target_node].datums[target_datum])
        return c

    def delete(self):
        """ Upon deletion, disconnect this connection from source
            and target nodes.
        """
        if self.source: self.disconnect_from_source()
        if self.target: self.disconnect_from_target()

    def can_connect_to(self, datum):
        """ Returns True if we can connect to the given datum.
        """
        return datum.can_connect(self)

    def connect_to(self, target):
        """ Connects to the target, which sets self.target to itself
        """
        target.connect_input(self)

    def disconnect_from_target(self):
        """ Disconnects from the target, which sets self.target to None
        """
        self.target.disconnect_input(self)

    def disconnect_from_source(self):
        """ Disconnects from source, which sets self.source to None.
        """
        self.source.disconnect_output(self)

import operator

def serialize_connections(nodes):
    """ Return a list of deflated connections
        (which is simply a set of connection index values).
    """
    connections = reduce(operator.add, [n.connections() for n in nodes], [])
    deflated = [c.deflate(nodes) for c in set(connections)]
    return [d for d in deflated if d is not None]
