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
        if self._sync():    # defined in subclasses
            self.reposition()
        if self.editor:                     self.editor.sync()

        # Sync all connections (if their controls have been constructed,
        # to prevent problems when loading files)
        for c in self.node.connections():
            if c.control:   c.control.sync()


    def contextMenuEvent(self, event):
        """ Ignore context menu events so that these widgets
            can accept right-click events.
        """
        pass

    def delete(self):
        """ Cleanly deletes both abstract and UI representations.
        """
        # If this is a child node, then delete from the parent instead
        if self.node.parent:
            self.node.parent.control.delete()
            return

        # Release the mouse (just in case)
        self.releaseMouse()

        # Delete connection widgets
        for t, d in self.node.datums:
            for c in d.connections():
                if c:   c.control.deleteLater()

        # Delete any children nodes
        for key in self.node.__dict__:
            attr = getattr(self.node, key)
            if isinstance(attr, node.base.Node):
                attr.parent = None  # clear parent first to prevent recursion
                attr.control.delete()

        self.node.delete()
        if self.editor: self.editor.deleteLater()
        self.canvas.setFocus()
        self.deleteLater()

    def release(self):
        self.releaseMouse()
        for d in dir(self):
            if isinstance(getattr(self,d), DragManager):
                getattr(self, d).drag = False
        self.update()

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
        return self.editor_position() - QtCore.QPoint(8, 0)
    def get_output_pos(self):
        return self.editor_position() + QtCore.QPoint(8, 0)

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

    def draw_lines(self, lines, offset):
        """ Draws a set of lines (given by QVector3D coordinates).
            Lines should be a list of lists of QVector3Ds.
            Returns a painter path with those lines drawn.
        """
        path = QtGui.QPainterPath()
        for L in lines:
            path.moveTo(self.canvas.unit_to_pixel(L[0]) - offset)
            for p in L[1:]:
                path.lineTo(self.canvas.unit_to_pixel(p) - offset)
        return path


################################################################################

class DragManager(QtCore.QObject):
    def __init__(self, parent, callback, mask=None):
        super(DragManager, self).__init__()
        self.parent = parent
        self.callback  = callback
        self.mask = mask

        self.mouse_pos = QtCore.QPoint()
        self.drag = False
        self.hover = False

        self.parent.installEventFilter(self)


    def eventFilter(self, object, event):
        if object == self.parent:
            if event.type() == QtCore.QEvent.MouseButtonPress:
                return self.mouse_press(event)
            elif event.type() == QtCore.QEvent.MouseMove:
                return self.mouse_move(event)
            elif event.type() == QtCore.QEvent.Leave:
                return self.mouse_leave(event)
            elif event.type() == QtCore.QEvent.MouseButtonRelease:
                return self.mouse_release(event)
            elif event.type() == QtCore.QEvent.MouseButtonDblClick:
                return self.mouse_doubleClick(event)
        return False


    def mouse_move(self, event):
        """ When the mouse moves, store the updated mouse position
            (in canvas pixel coordinates) and update drag / hover as needed.

            Returns True if something changed, False otherwise.
        """
        pos = event.pos()
        changed = False
        pos_global = self.parent.mapToParent(pos)

        # If we're dragging, then do so
        if self.drag:
            v = self.parent.canvas.drag_vector(self.mouse_pos, pos_global)
            p = self.parent.canvas.pixel_to_unit(pos_global)
            self.callback(v, p)
            changed = True

        # If the mouse is above this object, then hover on
        elif self.mask is None or self.mask.contains(pos):
            if not self.hover:
                self.hover = True
                changed = True

        # Otherwise hover off
        else:
            if self.hover:
                self.hover = False
                changed = True

        # Store mouse position (for dragging calculations)
        self.mouse_pos = pos_global
        if changed:     self.parent.update()

        # Return True if this event caused something to change;
        # false otherwise
        return changed


    def mouse_press(self, event):
        """ On left mouse press, start dragging if it hits the mask.
        """
        mode = QtGui.QApplication.instance().mode
        if ((self.mask is None or self.mask.contains(event.pos())) and
            event.button() == QtCore.Qt.LeftButton):

            if mode == 'move':
                self.drag = True
                self.parent.update()
            elif mode == 'delete':
                self.parent.delete()
            return True
        return self.drag

    def mouse_release(self, event):
        """ On left mouse release, stop dragging and eat the event
            if we were previous dragging.
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.parent.release()
            return True
        return False

    def mouse_leave(self, event):
        """ On mouse leave, stop hovering
            (but don't eat the event).
        """
        if self.hover:
            self.hover = False
            self.parent.update()
        return False

    def mouse_doubleClick(self, event):
        if event.button() == QtCore.Qt.LeftButton and (
                self.mask is None or self.mask.contains(event.pos())):
            self.parent.open_editor()
            return True
        return False


class DragXY(DragManager):
    def __init__(self, parent, mask=None):
        super(DragXY, self).__init__(parent, self.dragXY, mask)

    def dragXY(self, v, p):
        """ Drag this node by attempting to change its x and y coordinates
        """
        if self.parent.node._x.simple():
            self.parent.node._x.set_expr(
                    str(float(self.parent.node._x.get_expr()) + v.x()))
        if self.parent.node._y.simple():
            self.parent.node._y.set_expr(
                    str(float(self.parent.node._y.get_expr()) + v.y()))

class DragXYZ(DragManager):
    def __init__(self, parent, mask=None):
        super(DragXYZ, self).__init__(parent, self.dragXYZ, mask)

    def dragXYZ(self, v, p):
        """ Drag this node by attempting to change its x, y, and z coordinates
        """
        if self.parent.node._x.simple():
            self.parent.node._x.set_expr(
                    str(float(self.parent.node._x.get_expr()) + v.x()))
        if self.parent.node._y.simple():
            self.parent.node._y.set_expr(
                    str(float(self.parent.node._y.get_expr()) + v.y()))
        if self.parent.node._z.simple():
            self.parent.node._z.set_expr(
                    str(float(self.parent.node._z.get_expr()) + v.z()))

################################################################################

class TextLabelControl(NodeControl):
    """ Represents a draggable label floating in space.
        Must be attached to a node with x and y (float) datums.
    """
    def __init__(self, canvas, target, text):
        super(TextLabelControl, self).__init__(canvas, target)

        self.text = text
        self.font = QtGui.QFont()
        self.drag_control = DragXYZ(self)

        self.position = QtGui.QVector3D()

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


    def _sync(self):
        """ Move this control to the appropriate position.
            Use self.position (cached) if eval fails.
        """
        v = QtGui.QVector3D(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y(),
                self.node.z if self.node._z.valid() else self.position.z())
        changed = (v != self.position)
        self.position = v

        return changed

    def reposition(self):
        self.move(self.canvas.unit_to_pixel(self.position))

    def paintEvent(self, paintEvent):
        """ On paint event, paint oneself.
        """
        self.paint(QtGui.QPainter(self))

    def paint(self, painter):
        painter = QtGui.QPainter(self)
        painter.setPen(QtGui.QColor(*colors.blue))
        if self.drag_control.drag or self.drag_control.hover:
            painter.setBrush(QtGui.QColor(*(colors.blue + (150,))))
        else:
            painter.setBrush(QtGui.QColor(*(colors.blue + (100,))))
        painter.drawRect(self.rect())
        painter.setPen(QtGui.QColor(255, 255, 255))
        painter.setFont(self.font)
        painter.drawText(10, self.height() - 10, self.text)

    def get_input_pos(self):
        return self.pos() + QtCore.QPoint(0, self.height()/2)

    def get_output_pos(self):
        return self.pos() + QtCore.QPoint(self.width(), self.height()/2)

################################################################################

def make_node_widgets(canvas):

    children = {}
    for n in node.base.nodes:
        for key in n.__dict__:
            attr = getattr(n, key)
            if isinstance(attr, node.base.Node):
                children[node.base.nodes.index(attr)] = attr

    for i, n in enumerate(node.base.nodes):
        n.get_control()(canvas, n)


from ui.editor import MakeEditor
import node.base
import fab.expression
