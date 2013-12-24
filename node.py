class Node(object):
    """ Graph node.  Contains one or more Datum objects.
    """
    def __init__(self):
        """ Node constructor.
            Adds node to master list and sets _deleted to False.
        """
        self._deleted = False
        self.control  = None
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
def dict():     return {n._name.get_expr(): n for n in nodes}

################################################################################

import editor
from PySide import QtGui

class NodeControl(QtGui.QWidget):
    def __init__(self, canvas, node, *args, **kwargs):
        super(NodeControl, self).__init__(canvas)
        self.canvas = canvas

        self.node = node
        node.control = self

        self.editor  = None

    def open_editor(self):
        """ Opens / closes the editor.
        """
        if not self.editor:
            editor.MakeEditor(self)
        elif self.editor:
            self.editor.animate_close()

    def get_datum_output(self, d):
        """ Returns a canvas pixel location for the given datum's output.
        """
        if self.editor: return self.editor.get_datum_output(d)
        else:           return None

    def get_datum_input(self, d):
        """ Returns a canvas pixel location for the given datum's input.
        """
        if self.editor: return self.editor.get_datum_input(d)
        else:           return None



