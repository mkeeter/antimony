from PySide import QtCore, QtGui

class NodeControl(QtGui.QWidget):
    def __init__(self, canvas, node, *args, **kwargs):
        super(NodeControl, self).__init__(canvas)
        self.setMouseTracking(True)

        self.canvas = canvas

        self.node = node
        node.control = self

        self.editor  = None

    def delete(self):
        """ Cleanly deletes both abstract and UI representations.
        """
        # Delete connection widgets
        for t, d in self.node.datums:
            for c in d.connections():
                if c:
                    c.control.deleteLater()
        self.node.delete()
        if self.editor: self.editor.deleteLater()
        self.deleteLater()

    def open_editor(self):
        """ Opens / closes the editor.
        """
        if not self.editor:
            MakeEditor(self)
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

