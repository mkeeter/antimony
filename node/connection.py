class Connection(object):
    """ Represents a connection between two datums.
        The target datum tracks the source datum's value.
    """
    def __init__(self, datum):
        self.source = datum
        self.source.outputs.append(self)
        self.target = None
        self.control = None

    def delete(self):
        """ Upon deletion, disconnect this connection from source
            and target nodes.
        """
        if self.source: self.disconnect_from_source()
        if self.target: self.disconnect_from_target()

    def can_connect_to(self, datum):
        """ Returns True if we can connect to the given datum.
        """
        return datum.can_connect(self)

    def connect_to(self, target):
        """ Connects to the target, which sets self.target to itself
        """
        target.connect_input(self)

    def disconnect_from_target(self):
        """ Disconnects from the target, which sets self.target to None
        """
        self.target.disconnect_input(self)

    def disconnect_from_source(self):
        self.source.disconnect_output(self)
