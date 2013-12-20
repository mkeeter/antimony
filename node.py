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

import wx
import editor

class NodeControl(object):
    def __init__(self, canvas, node, *args, **kwargs):
        self.canvas = canvas

        self.node = node
        node.control = self

        self.editor  = None

    def on_motion(self, event):
        raise NotImplementedError(
                "NodeControl.on_motion must be defined in subclass.")

    def on_click(self, event):
        raise NotImplementedError(
                "NodeControl.on_motion must be defined in subclass.")

    def draw(self, event):
        raise NotImplementedError(
                "NodeControl.draw must be defined in subclass.")

    def open_editor(self, event=None):
        """ Opens / closes the editor.
        """
        if not self.editor:
            self.editor = editor.MakeEditor(self)
        elif self.editor:
            self.editor.start_close()

