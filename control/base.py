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

        self.move(self.canvas.mm_to_pixel(x=x),
                  self.canvas.mm_to_pixel(y=y))

        self.position = QtCore.QPointF(x, y)

        if self.editor:     self.editor.sync()

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

def make_node_widgets(canvas):
    import node.base

    children = []
    for n in node.base.nodes:
        children += [c for name, c in n.children(node.base.nodes)]

    for i, n in enumerate(node.base.nodes):
        if i in children:
            n.get_control(is_child=True)(canvas, n, node.base.nodes[i])
        else:
            n.get_control(is_child=False)(canvas, n)


from ui.editor import MakeEditor


