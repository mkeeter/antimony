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


from ui.editor import MakeEditor

