import operator

from PySide import QtCore, QtGui

import colors

class NodeControl(QtGui.QWidget):
    def __init__(self, canvas, node):
        super(NodeControl, self).__init__(canvas)
        self.setMouseTracking(True)

        self.canvas = canvas

        self.node = node
        node.control = self

        self.editor  = None

    def sync(self):
        """ Synchs the editor and all node connections.
        """
        if self.editor:
            self.editor.sync()
        for c in self.node.connections():
            c.control.sync()

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
        return self.canvas.unit_to_pixel(p)

    def get_input_pos(self):
        return self.pos() + QtCore.QPoint(0, self.height()/2)
    def get_output_pos(self):
        return self.pos() + QtCore.QPoint(self.width(), self.height()/2)

    def get_datum_output(self, d):
        """ Returns a canvas pixel location for the given datum's output.
        """
        if self.editor:
            frac = self.editor._mask_size
            return (self.get_output_pos() * (1 - frac) +
                    self.editor.get_datum_output(d) * frac)
        else:
            return self.get_output_pos()


    def get_datum_input(self, d):
        """ Returns a canvas pixel location for the given datum's input.
        """
        if self.editor:
            frac = self.editor._mask_size
            return (self.get_input_pos() * (1 - frac) +
                    self.editor.get_datum_input(d) * frac)
        else:
            return self.get_input_pos()

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
            delta = (self.canvas.pixel_to_unit(p) -
                     self.canvas.pixel_to_unit(self.mouse_pos))
            self.drag(delta.x(), delta.y())
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

################################################################################

class TextLabelControl(DraggableNodeControl):
    """ Represents a draggable label floating in space.
        Must be attached to a node with x and y (float) datums.
    """
    def __init__(self, canvas, target, text):
        super(TextLabelControl, self).__init__(canvas, target)

        self.text = text
        self.font = QtGui.QFont()

        fm = QtGui.QFontMetrics(self.font)
        rect = fm.boundingRect(self.text)
        self.setFixedSize(rect.width() + 20, rect.height() + 20)

        self.sync()
        self.show()
        self.raise_()


    def editor_position(self):
        """ Place the editor to the bottom-right of the widget.
        """
        return (super(TextLabelControl, self).editor_position() +
                QtCore.QPoint(self.width(), self.height()))


    def sync(self):
        """ Move this control to the appropriate position.
            Use self.position (cached) if eval fails.
        """
        try:    x = self.node.x
        except: x = self.position.x()

        try:    y = self.node.y
        except: y = self.position.y()

        pos = self.canvas.unit_to_pixel(x, y)
        self.move(pos.x(), pos.y())

        self.position = QtCore.QPointF(x, y)

        super(TextLabelControl, self).sync()

    def paintEvent(self, paintEvent):
        """ On paint event, paint oneself.
        """
        self.paint(QtGui.QPainter(self))

    def paint(self, painter):
        painter = QtGui.QPainter(self)
        painter.setPen(QtGui.QColor(*colors.blue))
        if self.dragging or self.hovering:
            painter.setBrush(QtGui.QColor(*(colors.blue + (150,))))
        else:
            painter.setBrush(QtGui.QColor(*(colors.blue + (100,))))
        painter.drawRect(self.rect())
        painter.setPen(QtGui.QColor(255, 255, 255))
        painter.setFont(self.font)
        painter.drawText(10, self.height() - 10, self.text)

    def drag(self, dx, dy):
        """ Drag this node by attempting to change its x and y coordinates
            dx and dy should be floating-point values.
        """
        if self.node._x.simple():
            self.node._x.set_expr(str(float(self.node._x.get_expr()) + dx))
        if self.node._y.simple():
            self.node._y.set_expr(str(float(self.node._y.get_expr()) + dy))

    def get_input_pos(self):
        return self.pos() + QtCore.QPoint(0, self.height()/2)

    def get_output_pos(self):
        return self.pos() + QtCore.QPoint(self.width(), self.height()/2)

################################################################################

def make_node_widgets(canvas):
    import node.base

    children = {}
    for n in node.base.nodes:
        for key in n.__dict__:
            attr = getattr(n, key)
            if isinstance(attr, node.base.Node):
                children[node.base.nodes.index(attr)] = attr

    for i, n in enumerate(node.base.nodes):
        if i in children:
            n.get_control(is_child=True)(canvas, n, children[i])
        else:
            n.get_control(is_child=False)(canvas, n)


from ui.editor import MakeEditor


