import wx

class Connection(object):
    """ Represents a connection between two datums.
        The target datum tracks the source datum's value.
    """
    def __init__(self, datum):
        self.source = datum
        self.source.outputs.append(self)
        self.target = None

    def connect_to(self, target):
        self.target = target


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

current = None
