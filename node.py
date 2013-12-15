class Node(object):

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
