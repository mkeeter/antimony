import operator

import datum

class Node(object):
    """ Graph node.  Contains one or more Datum objects.
    """
    def __init__(self, name):
        """ Node constructor.
            Adds node to master list and sets _deleted to False.
        """

        # Save a master list of datums to serialize nodes for saving.
        self.datums = []
        self.control  = None
        self.parent   = None
        self.add_datum('name', datum.NameDatum(self, name))
        nodes.append(self)

    def get_control(self):
        """ Used to get a UI control widget for this node.
        """
        raise NotImplementedError("Derived class must implement get_control")

    def add_datum(self, n, d):
        """ Adds a name + datum pair to the master list and sets
            an attribute storing this datum object.
        """
        self.datums.append((n,d))
        setattr(self, '_'+n, d)


    def deflate(self, nodes):
        """ Returns a flattened version of this node suitable for saving.
            The flattened node is a three item list:
                This node's class
                A list of datum tuples (name, datum class, datum argument)
                A list of children tuples (name, index in node list)
        """
        # Flatten each datum.
        # Order matters, so we use our built-in array
        # (otherwise connections will end up connected to the wrong places,
        #  because they use index within self.datums)
        datums = []
        for d in self.datums:
            if isinstance(d[1], datum.FunctionDatum):
                datums.append((d[0], d[1].__class__, d[1].function_name))
            elif isinstance(d[1], datum.EvalDatum):
                datums.append((d[0], d[1].__class__, d[1]._expr))

        # Find and flatten all children
        # Order doesn't matter here because there aren't any connections.
        children = []
        for key in self.__dict__:
            attr = getattr(self, key)
            if isinstance(attr, Node):
                children.append((key, nodes.index(attr)))

        return [self.__class__, datums, children]

    @classmethod
    def inflate(cls, deflated):
        """ Inflates a deflated node back into a proper object.
        """
        datums = deflated[1]
        # Get this datum's name
        name = [d[2] for d in datums if d[0] == 'name'][0]
        n = cls(name[1:-1])
        for datum_name, datum_type, datum_expr in datums:
            # Skip the name datum, because that was set in the constructor
            if datum_name == 'name':    continue
            n.add_datum(datum_name, datum_type(n, datum_expr))

        # Swap in the appropriate class
        n.__class__ = deflated[0]
        return n

    def add_children(self, children, nodes):
        """ Re-links children (when called on parent node)
            (this is the second stage of reconstructing a node, done
             after we've created all of the Node objects)
        """
        for child, index in children:
            setattr(self, child, nodes[index])
            nodes[index].parent = self

    def delete(self, child=None):
        """ Removes node from master list, deleting all connections as well.
        """
        # Remove from master list
        if nodes is not None:   nodes.remove(self)

        # Delete connections from datums
        for t, d in self.datums:
            for c in d.connections():
                c.delete()


    def connections(self):
        """ Returns a list of connections attached to this node.
        """
        return reduce(operator.add,
                      [d[1].connections() for d in self.datums], [])

    def __getattr__(self, v):
        """ Overload getattr to evaluate datum's value.
        """
        return self.__getattribute__('_'+v).value()


# Master list of nodes
nodes = []
def dict():     return {n._name.get_expr(): n for n in nodes}

def get_name(prefix):
    names = [n._name.get_expr() for n in nodes]
    i = 0
    while '%s%i' % (prefix, i) in names:
        i += 1
    return '%s%i' % (prefix, i)

def serialize_nodes():
    return [n.deflate(nodes) for n in nodes]

def load_nodes(data):
    global nodes
    nodes = [Node.inflate(n) for n in data]
    for n, d in zip(nodes, data):
        n.add_children(d[2], nodes)

