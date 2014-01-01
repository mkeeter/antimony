import operator

import base

################################################################################

# These objects define different input schemes for Datum objects, including
#   No inputs allowed
#   One input allowed
#   Multiple inputs allowed
class NoInput(object):
    """ Object that rejects all inputs.
    """
    def __init__(self, parent):     self.parent = parent
    def __nonzero__(self):          return False
    def __iter__(self):             return [].__iter__()
    def accepts(self, conn):        return False
    def size(self):                 return 0
    def value(self):                return None
    def connect(self, conn):
        raise RuntimeError("Cannot connect to a NoInput object")
    def disconnect(self, conn):
        raise RuntimeError("Cannot disconnect from a NoInput object")

class SingleInput(object):
    """ Object that represents a single-source input system.
    """
    def __init__(self, parent):
        self.parent = parent
        self.i = None
    def __nonzero__(self):  return self.i is not None
    def value(self):        return self.i.source.value() if self.i else None
    def size(self):
        return 0 if self.i is None else 1
    def __iter__(self):
        return [].__iter__() if self.i is None else [self.i].__iter__()
    def accepts(self, conn):
        return (conn.source != self.parent and
                conn.source.type == self.parent.type and
                self.i is None)
    def connect(self, conn):
        self.i = conn
        conn.target = self.parent
        self.parent.sync()
    def disconnect(self, conn):
        self.i = None
        conn.target = None
        self.parent.sync()
    def expr(self):
        if self.i:  return self.i.source._expr
        else:       return None

class MultiInput(object):
    """ Object that represents a multi-source input system.
        Uses reduce with the given operator to combine inputs.
    """
    def __init__(self, parent, op=operator.or_):
        self.parent = parent
        self.i = []
        self.op = op
    def __nonzero__(self):  return self.i != []
    def size(self):         return len(i)
    def value(self):
        if self.i:  return reduce(self.op, [i.source.value() for i in self.i])
        else:       return None
    def __iter__(self):
        return self.i.__iter__()
    def accepts(self, conn):
        return (conn.source != self.parent and
                conn.source.type == self.parent.type and
                not conn.source in [c.source for c in self.i])
    def connect(self, conn):
        self.i.append(conn)
        conn.target = self.parent
        self.parent.sync()
    def disconnect(self, conn):
        self.i.remove(conn)
        conn.target = None
        self.parent.sync()
    def expr(self):
        if len(self.i) > 1:     return "%i inputs" % len(self.i)
        elif len(self.i) == 1:  return "1 input"
        else:                   return None

################################################################################

class Datum(object):
    stack = []

    def __init__(self, node, expr, T, inputType=SingleInput):
        self.node    = node
        self._expr   = str(expr)
        self.type    = T

        self.outputs = []
        self.input   = inputType(self)

        self.parents = set()
        self.children = set()

        # Check to make sure that the initial expression is valid.
        self.eval()

    def connections(self):
        """ Returns a list of connections attached to this datum.
        """
        return self.outputs + list(self.input)

    def can_connect(self, conn):
        """ Returns True if we can accept the given connection (as an input)
        """
        return self.input.accepts(conn)

    def connect_input(self, conn):
        """ Links an input connection to this node.
        """
        self.input.connect(conn)

    def disconnect_input(self, conn):
        """ Disconnects an input connection.
        """
        self.input.disconnect(conn)

    def disconnect_output(self, conn):
        """ Disconnects an output connection.
        """
        self.outputs.remove(conn)
        conn.source = None

    def get_expr(self):
        """ Returns the expression string.
        """
        if self.input:  self._expr = self.input.expr()
        return self._expr

    def can_edit(self):
        """ Returns True if we can edit this datum
            (false if it's tied to an input)
        """
        return self.input.value() is None

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
                t = self.input.value()
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
        super(NameDatum, self).__init__(node, "'%s'" % value, name.Name)

    def get_expr(self):
        return self._expr[1:-1]

    def set_expr(self, e):
        """ Sets the expression string and syncs children as needed.
        """
        super(NameDatum, self).set_expr("'%s'" % e)


################################################################################

import operator

class ExpressionDatum(Datum):
    def __init__(self, node, value):
        super(ExpressionDatum, self).__init__(
                node, value, Expression, inputType=MultiInput)

################################################################################

class FunctionDatum(Datum):
    """ Represents a value calculated from a function.
        Usually used for a node output value.
    """

    def __init__(self, node, function, T):
        self.function = function
        super(FunctionDatum, self).__init__(
                node, None, T, inputType=NoInput)

    def set_expr(self, value):
        raise TypeError("set_expr is an invalid operation for a FunctionDatum")

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
    """ Represents a math expression calculated by a function.
    """

    def __init__(self, node, function):
        """ Pass in a parent node and a function to generate
            an output Expression.
        """
        super(ExpressionFunctionDatum, self).__init__(
                node, function, Expression)



