from PySide import QtCore, QtGui

class NodeControl(QtGui.QWidget):
    def __init__(self, canvas, node):
        super(NodeControl, self).__init__(canvas)
        self.setMouseTracking(True)

        self.canvas = canvas

        self.node = node
        node.control = self

        self.editor  = None


    def contextMenuEvent(self, event):
        """ Ignore context menu events so that these widgets
            can accept right-click events.
        """
        pass

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

    def editor_position(self):
        """ Returns a canvas pixel location at which the editor
            should be placed.
        """
        p = self.position
        return QtCore.QPoint(*self.canvas.mm_to_pixel(p.x(), p.y()))

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

################################################################################

class DraggableNodeControl(NodeControl):
    def __init__(self, canvas, node):
        super(DraggableNodeControl, self).__init__(canvas, node)

        self.mouse_pos = QtCore.QPoint()
        self.dragging = False
        self.hovering = False

    def hit(self, pos):
        """ Checks to see if the given position is a hit that should
            drag or delete the widget.  Overload in child classes.
        """
        return True

    def mousePressEvent(self, event):
        """ On a mouse press that hits the node's core, delete or drag
            the node (for right and left click respectively).
        """
        if not self.hit(event.pos()):
            return
        elif event.button() == QtCore.Qt.RightButton:
            self.delete()
        elif event.button() == QtCore.Qt.LeftButton:
            self.mouse_pos = self.mapToParent(event.pos())
            self.dragging = True

    def mouseDoubleClickEvent(self, event):
        """ On a double-click that hits the node's core, open an editor.
        """
        if self.hit(event.pos()) and event.button() == QtCore.Qt.LeftButton:
            self.open_editor()

    def mouseReleaseEvent(self, event):
        """ On a left-button release event, stop dragging.
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.dragging = False

    def mouseMoveEvent(self, event):
        """ When the mouse is moved, update self.mouse_pos and drag the
            widget using self.drag (which must be defined in child classes).
        """
        p = self.mapToParent(event.pos())
        if self.dragging:
            delta = p - self.mouse_pos
            scale = self.parentWidget().scale
            self.drag(delta.x() / scale, -delta.y() / scale)
        elif self.hovering != self.hit(event.pos()):
            self.hovering = self.hit(event.pos())
            self.update()
        self.mouse_pos = p

    def leaveEvent(self, event):
        """ When the mouse leaves the widget, set hovering to False.
        """
        if self.hovering:
            self.hovering = False
            self.update()

from ui.editor import MakeEditor

