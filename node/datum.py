import base

class Datum(object):
    stack = []

    def __init__(self, node, expr, T):
        self.node    = node
        self._expr    = repr(expr)
        self.type    = T

        self.outputs = []
        self.input   = None

        self.parents = set()
        self.children = set()

        # Check to make sure that the initial expression is valid.
        self.eval()

    def connections(self):
        """ Returns a list of connections attached to this datum.
        """
        return self.outputs + ([self.input] if self.input else [])

    def can_connect(self, conn):
        """ Returns True if we can accept the given connection (as an input)
        """
        return (self != conn.source and
                self.type == conn.source.type and
                self.input is None)

    def connect_input(self, conn):
        """ Links an input connection to this node.
        """
        self.input = conn
        conn.target = self
        self.sync()

    def disconnect_input(self, conn):
        """ Disconnects an input connection.
        """
        self.input = None
        conn.target = None
        self.sync()

    def disconnect_output(self, conn):
        """ Disconnects an output connection.
        """
        self.outputs.remove(conn)
        conn.source = None

    def get_expr(self):
        """ Returns the expression string.
        """
        return self._expr

    def can_edit(self):
        """ Returns True if we can edit this datum
            (false if it's tied to an input)
        """
        return self.input is None

    def set_expr(self, e):
        """ Sets the expression string.
            Updates node, editor, and children as needed
            (which may trigger a canvas Refresh)
        """
        if e == self._expr:     return

        self._expr = e
        self.sync()


    def sync(self):
        """ Update the node control and editor for all children of this Datum
            This may trigger a canvas refresh operation.
        """
        for c in [self] + list(self.children):
            if c.node.control:  c.node.control.sync()


    def value(self):
        """ Gets the value from this datum,
            or evaluated from the expression.

            Raises an exception if expression evaluation fails.
        """
        return self.eval()

    def push_stack(self):
        """ Pushes oneself onto the evaluation stack, tracking children
            and parents that are already there.
        """
        # If this was called from another datum, then register it as a
        # parent and register self as its children
        for d in Datum.stack:
            if d is not self:
                d.parents.add(self)
                self.children.add(d)

        if self in Datum.stack:
            raise RuntimeError("Infinite recursion in Datum.")

        # Clear parents and remove references to self in parents
        for p in self.parents:
            p.children.remove(self)
        self.parents = set()

        # Put oneself down on the stack.
        Datum.stack.append(self)


    def pop_stack(self):
        """ Pops oneself from the top of the evaluation stack.
        """
        Datum.stack.pop()


    def eval(self):
        """ Attempts to evaluate the expression and return a value.
            Raises an exception if this fails.
        """
        self.push_stack()
        try:
            if self.input:
                self._expr = self.input.source._expr
                t = self.input.source.value()
            else:
                t = eval(self._expr, base.dict())
                if not isinstance(t, self.type):    t = self.type(t)
        except:     raise
        finally:    self.pop_stack()

        return t

    def valid(self):
        """ Returns True if this datum's expression is valid, false otherwise.
        """
        try:    self.eval()
        except: return False
        else:   return True

################################################################################

class FloatDatum(Datum):
    def __init__(self, node, value):
        super(FloatDatum, self).__init__(node, value, float)
    def simple(self):
        """ Returns True if the expression can be directly converted into
            a floating-point value; false otherwise.
        """
        try:    float(self._expr)
        except: return False
        else:   return True

################################################################################

import name
class NameDatum(Datum):
    def __init__(self, node, value):
        super(NameDatum, self).__init__(node, value, name.Name)

    def get_expr(self):
        return self._expr[1:-1]

    def set_expr(self, e):
        """ Sets the expression string and syncs children as needed.
        """
        super(NameDatum, self).set_expr("'%s'" % e)

################################################################################

class MultiInputDatum(Datum):
    """ Represents a node that can take in multiple inputs.
    """
    def __init__(self, node, value, T):
        self.inputs = []
        super(MultiInputDatum, self).__init__(node, value, T)
        del self.input

    def connections(self):
        """ Returns a list of connections attached to this datum.
        """
        return self.outputs + self.inputs

    def can_connect(self, conn):
        """ Returns true if we can accept the given connection as an input
        """
        return (self != conn.source and self.type == conn.source.type
                and not conn.source in [c.source for c in self.inputs])

    def connect_input(self, conn):
        """ Links an input connection to this node.
        """
        self.inputs.append(conn)
        conn.target = self
        self.sync()

    def disconnect_input(self, conn):
        """ Disconnects an input connection.
        """
        self.inputs.remove(conn)
        conn.target = None
        self.sync()

    def get_expr(self):
        """ Returns the expression string, or a placeholder if we have
            multiple inputs.
        """
        if len(self.inputs) > 1:
            return "%i inputs" % len(self.inputs)
        elif len(self.inputs) == 1:
            return "1 input"
        else:
            return self._expr

    def can_edit(self):
        """ Returns True if we can edit this datum
            (false if it's tied to an input)
        """
        return self.inputs == []

################################################################################

import operator

class ExpressionDatum(MultiInputDatum):
    def __init__(self, node, value):
        super(ExpressionDatum, self).__init__(node, value, Expression)

    def eval(self):
        """ Attempts to evaluate the expression and return a value.
            Raises an exception if this fails.
        """
        self.push_stack()
        try:
            if self.inputs:
                t = reduce(operator.or_, [i.source.value()
                                          for i in self.inputs])
            else:
                t = eval(self._expr, base.dict())
                if not isinstance(t, self.type):    t = self.type(t)
        except:     raise
        finally:    self.pop_stack()

        return t

################################################################################

class FunctionDatum(Datum):
    """ Represents a value calculated from a function.
        Usually used for a node output value.
    """

    def __init__(self, node, function, T):
        self.function = function
        super(FunctionDatum, self).__init__(node, None, T)

    def invalid(self):
        raise TypeError("Invalid operation for output datum")
    def can_connect(self, conn):        self.invalid()
    def connect_input(self, conn):      self.invalid()
    def disconnect_input(self, conn):   self.invalid()
    def set_expr(self, value):          self.invalid()

    def get_expr(self):
        if self.valid():    return 'Function'
        else:               return 'Function (invalid)'
    def can_edit(self): return False

    def eval(self):
        """ Attempts to evaluate the given function and return a value.
            Raises an exception if this fails.
        """
        self.push_stack()
        try:
            t = self.function()
            if not isinstance(t, self.type):    t = self.type(t)
        except: raise
        finally:    self.pop_stack()

        return t

################################################################################

class FloatFunctionDatum(FunctionDatum):
    """ Represents a calculated float value.
    """
    def __init__(self, node, function):
        super(FloatFunctionDatum, self).__init__(node, function, float)

    def get_expr(self):
        if self.valid():    return str(self.value())
        else:               return 'Invalid'

################################################################################

from fab.expression import Expression

class ExpressionFunctionDatum(FunctionDatum):
    """ Represents a math expression.
    """

    def __init__(self, node, function):
        """ Pass in a parent node and a function to generate
            an output Expression.
        """
        super(ExpressionFunctionDatum, self).__init__(
                node, function, Expression)



