import operator

import datum

class Node(object):
    """ Graph node.  Contains one or more Datum objects.
    """
    def __init__(self, name):
        """ Node constructor.
            Adds node to master list and sets _deleted to False.
        """
        self._name = datum.NameDatum(self, name)
        self.control  = None
        nodes.append(self)

    def delete(self):
        """ Removes node from master list and sets _deleted to True.
        """
        if nodes is not None:   nodes.remove(self)

    def connections(self):
        """ Returns a list of connections attached to this node.
        """
        return reduce(operator.add,
                      [d[1].connections() for d in self.datums], [])

    def __getattr__(self, v):
        return self.__getattribute__('_'+v).value()

# Master list of nodes
nodes = []
def dict():     return {n._name.get_expr(): n for n in nodes}
