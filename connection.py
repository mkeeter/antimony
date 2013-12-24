from PySide import QtCore, QtGui

import name
import colors

_colors = {
        name.Name:  colors.yellow,
        float:      colors.blue
}

class Connection(object):
    """ Represents a connection between two datums.
        The target datum tracks the source datum's value.
    """
    def __init__(self, datum):
        self.source = datum
        self.source.outputs.append(self)
        self.target = None
        self.control = None

    def connect_to(self, target):
        self.target = target
        self.target.inputs.append(self)

    def disconnect_from_target(self):
        self.target.inputs.remove(self)
        self.target = None

    def disconnect_from_source(self):
        self.source.outputs.remove(self)
        self.source = None



class ConnectionControl(QtGui.QWidget):
    """ GUI wrapper around a connection.
    """
    def __init__(self, connection, parent):
        super(ConnectionControl, self).__init__(parent)
        self.connection = connection
        connection.control = self

        self.resize(10, 10)

        origin = self.get_origin()
        self.drag_pos = self.get_origin()
        self.sync()

        self.show()

    def sync(self):
        """ Positions this connection appropriately and hides based on
            whether we should draw this connection.
        """
        origin, target = self.get_origin(), self.get_target()

        # If we don't have a node to which we should connect,
        # then hide this widget and return.
        if origin is None or target is None:
            self.hide()
            return
        # Otherwise, show and raise the widget.
        elif self.isHidden():
            self.show()
            self.raise_()

        xmin = min(origin.x(), target.x()) - 5
        ymin = min(origin.y(), target.y()) - 5

        newGeom = QtCore.QRect(xmin, ymin,
                max(origin.x(), target.x()) + 5 - xmin,
                max(origin.y(), target.y()) + 5 - ymin)

        if newGeom != self.geometry():
            self.setGeometry(newGeom)
            self.make_mask()


    def make_mask(self):
        """ Updates the widget mask.
        """
        painter = QtGui.QPainter()
        bitmap = QtGui.QBitmap(self.size())
        bitmap.clear()

        painter.begin(bitmap)
        self.paint(painter, mask=True)
        painter.end()

        self.setMask(bitmap)


    def dragTo(self, pos):
        """ Sets the drag endpoint to the given position
            (in canvas coordinates).
        """
        self.drag_pos = pos
        self.sync()

    def release(self):
        """ Attempts to connect this widget to an Input widget.
            Calls deleteLater if not successful.
        """
        hit = self.parentWidget().find_input(self.drag_pos)
        if hit is None:
            self.connection.disconnect_from_source()
            self.hide()
            self.deleteLater()
        else:
            self.connection.connect_to(hit.datum)
            self.sync()


    def paintEvent(self, paintEvent):
        if self.isHidden():     return
        self.paint(QtGui.QPainter(self))

    def paint(self, painter, mask=False):
        origin, target = self.get_origin(), self.get_target()

        if mask:    color = QtCore.Qt.color1
        else:       color = QtGui.QColor(255, 0, 0)

        painter.setPen(QtGui.QPen(color, 4))
        painter.drawLine(origin - self.pos(), target - self.pos())

    def get_origin(self):
        """ Returns a canvas pixel location for the connected io.Output object.
            If no such object exists, returns None.
        """
        control = self.connection.source.node.control
        return control.get_datum_output(self.connection.source)


    def get_target(self):
        """ Returns a canvas pixel location for the connected io.Input object.
            If we aren't yet connected, return the mouse drag position.
            If we are connected but no target position exists, return None
        """
        if not self.connection.target:  return self.drag_pos
        control = self.connection.target.node.control
        return control.get_datum_input(self.connection.target)

################################################################################

class IO(QtGui.QWidget):
    def __init__(self, datum, parent):
        super(IO, self).__init__(parent)
        self.setFixedSize(10, 10)

        self.datum = datum
        self.color = _colors.get(self.datum.type, colors.red)
        self.hovering = False

    def enterEvent(self, event):
        self.hovering = True
        self.update()

    def leaveEvent(self, event):
        self.hovering = False
        self.update()

    def paintEvent(self, paintEvent):
        painter = QtGui.QPainter(self)
        if self.hovering:   color = (min(255, c + 60) for c in self.color)
        else:               color = self.color
        painter.setBackground(QtGui.QColor(*color))
        painter.eraseRect(self.rect())


class Input(IO):
    def __init__(self, datum, parent):
        super(Input, self).__init__(datum, parent)

    def mouse_hit(self, pos):
        """ Returns true if the given position (in canvas coordinates)
            is within this widget.
        """
        return self.geometry().contains(pos - self.parentWidget().pos())


class Output(IO):
    def __init__(self, datum, parent):
        super(Output, self).__init__(datum, parent)
        self.connection = None

    def mouseMoveEvent(self, event):
        if self.connection:
            self.connection.dragTo(
                    event.pos() + self.pos() + self.parentWidget().pos())

    def mouseReleaseEvent(self, event):
        if self.connection:
            self.connection.dragTo(
                    event.pos() + self.pos() + self.parentWidget().pos())
            self.connection.release()
            self.connection = None

    def mousePressEvent(self, event):
        self.connection = ConnectionControl(
                Connection(self.datum),
                self.parentWidget().parentWidget())

'''
class ConnectionControl(object):
    """ UI Wrapper around a Connection object.
    """
    def __init__(self, canvas, c):
        self.connection = c
        self.canvas = canvas

        # Get initial mouse position in the io.Output.
        self.mouse_pos = self.get_origin().center()

    def get_origin(self):
        """ Returns an io.Output object associated with the target datum
            If no such object exists, returns None.
        """
        editor = self.connection.source.node.control.editor
        if editor:
            return editor.get_datum_output(self.connection.source)
        else:
            return None

    def get_endpoint(self):
        """ Returns an io.Input object associated with the target datum
            If no such object exists, returns None.
        """
        editor = self.connection.target.node.control.editor
        if editor:
            return editor.get_datum_input(self.connection.target)
        else:
            return None

    def on_motion(self, event):
        """ Update mouse position and redraws the canvas.
            The event should be triggered from an io.Output control.
        """
        origin = self.get_origin()
        self.mouse_pos = (
            event.GetPosition() +
            origin.GetPosition() + origin.Parent.GetPosition())
        self.canvas.Refresh()


    def draw(self, dc, pick=False):
        """ Draws a straight line between start and endpoint
            (or start and mouse cursor position, if the endpoint hasn't
             yet been attached to anything).
        """
        # Get the IO control's position.
        origin = self.get_origin()
        origin_pos = origin.center()

        if self.connection.target is None:
            endpoint_pos = self.mouse_pos
        else:
            endpoint = self.get_endpoint()
            endpoint_pos = endpoint.center()

        dc.SetPen(wx.Pen((255, 255, 255), 4))
        if ((endpoint_pos > origin_pos.x) ^
            (endpoint_pos > origin_pos.y)):
            dc.DrawLinePoint(origin_pos, endpoint_pos)
        else:
            dc.DrawLinePoint(endpoint_pos, origin_pos)


    def on_release(self, event):
        """ Release mouse event.
            The event should be triggered from an io.Output control,
            as we'll use that to get a global mouse position.
        """
        origin = self.get_origin()
        self.mouse_pos = (
            event.GetPosition() +
            origin.GetPosition() +
            origin.Parent.GetPosition())

        d = self.canvas.find_input(self.mouse_pos)
        if d is None:
            self.canvas.connections.remove(self)
            self.connection = None
        else:
            self.connection.connect_to(d.datum)
        self.canvas.Refresh()
'''
