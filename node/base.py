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
        self.add_datum('name', datum.NameDatum(self, name))
        nodes.append(self)

    def add_datum(self, n, d):
        """ Adds a name + datum pair to the master list and sets
            an attribute storing this datum object.
        """
        self.datums.append((n,d))
        setattr(self, '_'+n, d)

    def children(self, nodes):
        """ Returns a list of all children nodes in this node.
        """
        return [(c, nodes.index(getattr(self, c))) for c in self.__dict__
                if isinstance(getattr(self, c), Node)]

    def deflate(self, nodes):
        """ Returns a flattened version of this node suitable for saving.
        """
        return [self.__class__,
                [(n, d.__class__, d._expr) for n, d in self.datums
                 if not isinstance(d, datum.FunctionDatum)],
                self.children(nodes)]

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
        """ Re-links children
            (this is the second stage of reconstructing a node, done
             after we've created all of the Node objects)
        """
        for child, index in children:
            setattr(self, child, nodes[index])

    def delete(self):
        """ Removes node from master list, deleting all connections as well.
        """
        if nodes is not None:   nodes.remove(self)
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
    nodes = [Node.inflate(n) for n in data]
    for n, d in zip(nodes, data):
        n.add_children(d[2], nodes)

