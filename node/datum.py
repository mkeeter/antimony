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
        if self.i:  return self.i.source.get_expr()
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
    def __iter__(self):     return self.i.__iter__()
    def value(self):
        if self.i:  return reduce(self.op, [i.source.value() for i in self.i])
        else:       return None
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
        if not self.i:  self.parent.set_expr('None')
        self.parent.sync()
    def expr(self):
        if len(self.i) > 1:     return "%i inputs" % len(self.i)
        elif len(self.i) == 1:  return "1 input"
        else:                   return None

################################################################################

class Datum(object):
    stack = []

    def __init__(self, node, T, inputType):
        self.node    = node
        self.type    = T

        self.outputs = []
        self.input   = inputType(self)

        self.parents = set()
        self.children = set()

    def render_me(self):
        """ Returns True if this is an expression that we can render.
        """
        return (isinstance(self, ExpressionFunctionDatum) and
                not list(self.input) and self.valid() and
                not any(o.target for o in self.outputs) and
                self.value().has_xy_bounds())


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


    def sync(self):
        """ Update the node control and editor for all children of this Datum
            This may trigger a canvas refresh operation.
        """
        for c in [self] + list(self.children):
            if c.node.control:  c.node.control.sync()


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

    def valid(self):
        """ Returns True if this datum's expression is valid, false otherwise.
        """
        try:    self.value()
        except: return False
        else:   return True

################################################################################

class EvalDatum(Datum):
    """ Datum where a value is calculated by running 'eval' on a user-provided
        string (or a user-provided input connection).
    """
    def __init__(self, node, T, expr, inputType=SingleInput):
        self._expr = str(expr)
        super(EvalDatum, self).__init__(node, T, inputType)

    def set_expr(self, e):
        """ Sets the expression string.
            Updates node, editor, and children as needed
            (which may trigger a canvas Refresh)
        """
        if e == self._expr:     return

        self._expr = e
        self.sync()

    def value(self):
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

################################################################################

class FloatDatum(EvalDatum):
    def __init__(self, node, value):
        super(FloatDatum, self).__init__(node, float, value)

    def simple(self):
        """ Returns True if the expression can be directly converted into
            a floating-point value; false otherwise.
        """
        try:    float(self._expr)
        except: return False
        else:   return True

    def __iadd__(self, delta):
        """ Increments this node's expression.
            Throws an exception if this operation fails.
        """
        self.set_expr(str(float(self.get_expr()) + delta))
        return self

################################################################################

class IntDatum(EvalDatum):
    def __init__(self, node, value):
        super(IntDatum, self).__init__(node, int, value)
    def __iadd__(self, delta):
        self.set_expr(str(self.value() + delta))
        return self
    def __isub__(self, delta):
        self.set_expr(str(self.value() - delta))
        return self

################################################################################

import name
class NameDatum(EvalDatum):
    def __init__(self, node, value):
        super(NameDatum, self).__init__(node, name.Name, "'%s'" % value)

    def get_expr(self):
        return self._expr[1:-1]

    def set_expr(self, e):
        """ Sets the expression string and syncs children as needed.
        """
        super(NameDatum, self).set_expr("'%s'" % e)

class StringDatum(EvalDatum):
    def __init__(self, node, value):
        super(StringDatum, self).__init__(node, str, "'%s'" % value)

    def get_expr(self):
        return self._expr[1:-1].replace("\\","")
    def set_expr(self, e):
        super(StringDatum, self).set_expr("'%s'" % e.replace("'","\\'"))

################################################################################

import operator

class ExpressionDatum(EvalDatum):
    def __init__(self, node, value):
        super(ExpressionDatum, self).__init__(
                node, Expression, value, inputType=MultiInput)

################################################################################

class FunctionDatum(Datum):
    """ Represents a value calculated from a function of the parent node.
        Usually used for a node output value.
    """

    def __init__(self, node, function_name, T):
        self.function_name = function_name
        super(FunctionDatum, self).__init__(
                node, T, inputType=NoInput)

    def set_expr(self, value):
        raise TypeError("set_expr is an invalid operation for a FunctionDatum")

    def get_expr(self):
        if self.valid():    return 'Function'
        else:               return 'Function (invalid)'
    def can_edit(self): return False

    def value(self):
        """ Attempts to evaluate the given function and return a value.
            Raises an exception if this fails.
        """
        self.push_stack()
        try:
            t = getattr(self.node, self.function_name)()
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

################################################################################

class ScriptDatum(Datum):
    """ Represents a Python script.
    """

    def __init__(self, node, script):
        self._script = script
        self._inputs = []
        self._outputs = []

        super(ScriptDatum, self).__init__(
                node, dict, inputType=NoInput)

    def set_expr(self, s):
        if s == self._script:   return

        self._script = s
        self.sync()

    def get_expr(self):
        return self._script

    def make_input(self, name, t, d):
        if name == 'name':
            raise RuntimeError("Reserved name")
        elif name not in ['x','y','z']:
            self._inputs.append([name, t])
        d[name] = getattr(self.node, name)

    def make_output(self, name, var):
        if name in ['name','x','y','z']:
            raise RuntimeError("Reserved name")
        self._outputs.append([name, var, type(var)])

    def push_stack(self):
        """ Push stack for recursive eval and clear inputs and outputs.
        """
        super(ScriptDatum, self).push_stack()
        self._inputs = []
        self._outputs = []


    def value(self):
        self.push_stack()

        d = {}
        d['input'] = lambda name, t: self.make_input(name, t, d)
        d['output'] = lambda name, var: self.make_output(name, var)
        try:
            exec self._script in d
        except Exception as e:
            print e
            raise
        finally:    self.pop_stack()

        return d

