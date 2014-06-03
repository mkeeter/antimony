import operator
import functools

from PySide import QtCore

from sb.manager import NodeManager
from sb.name import Name
from fab.expression import Expression

################################################################################

# These objects define different input schemes for Datum objects, including
#   No inputs allowed
#   One input allowed
#   Multiple inputs allowed

class _SingleInput(QtCore.QObject):
    def __init__(self, parent):
        super(_SingleInput, self).__init__(parent)
        self.i = None
    def __bool__(self):
        return self.i is not None
    def get_value(self):
        return self.i.value
    def get_display_str(self):
        return self.i.display_str()
    def accepts(self, d):
        return (self.i is None and d != self.parent() and
                d.data_type == self.parent().data_type)
    def connect(self, d):
        self.i = d
        d.destroyed.connect(lambda: self.disconnect(d))
        self.parent().emit_changed()
    def disconnect(self, d):
        """ Disconnects the given datum from this input handler.
            Also removes it from the parent datum's connected datum set
            and forces an update of the datum's expression.
        """
        self.i = None
        if d in self.parent()._connected_datums:
            self.parent()._connected_datums.remove(d)
        self.parent().emit_changed()

class _MultiInput(QtCore.QObject):
    def __init__(self, parent):
        super(_MultiInput, self).__init__(parent)
        self.i = []
    def __bool__(self):
        return len(self.i) != 0
    def get_value(self):
        return functools.reduce(operator.or_, [a.value for a in self.i])
    def get_display_str(self):
        return "%i input%s" % (len(self.i), 's' if len(self.i) != 1 else '')
    def accepts(self, d):
        return (d not in self.i and d != self.parent() and
                d.data_type == self.parent().data_type)
    def connect(self, d):
        self.i.append(d)
        d.destroyed.connect(lambda: self.disconnect(d))
        self.parent().emit_changed()
    def disconnect(self, d):
        self.i.remove(d)
        if d in self.parent()._connected_datums:
            self.parent()._connected_datums.remove(d)
        if len(self.i) == 0:
            self.parent().set_expr('None')
        else:
            self.parent().emit_changed()

################################################################################

class Datum(QtCore.QObject):

    class ValidError(RuntimeError): pass

    _caller = None
    changed = QtCore.Signal(object, object, bool)

    def __init__(self, name, node, data_type, **kwargs):
        """ Base constructor for Datum objects.

            Valid keyword arguments:
                has_output: True or False (default True)
                input_type: class to handle inputs
        """
        super(Datum, self).__init__(node)
        self.name = name

        self.data_type = data_type

        self.has_output = kwargs.get('has_output', True)
        self.input_handler = kwargs.get('input_type', lambda x: None)(self)

        self._value = self.data_type()
        self._valid = False

        # self._connected_datums is a set of child datums which have been
        # connected to the changed signal.
        self._connected_datums = set()

    @property
    def value(self):
        """ Connects the calling node to our changed signal, then returns
            self._value if valid and raise an exception if not.
        """
        self.connect_caller()
        if self._valid:
            return self._value
        else:
            raise Datum.ValidError()

    @property
    def valid(self):
        return self._valid


    def can_connect(self, d):
        """ Returns True if we can accept an incoming connection from the
            given datum.
        """
        return (self.input_handler is not None and
                self.input_handler.accepts(d))

    def connect_caller(self):
        """ Connects Datum._caller's update slot to this node's changed signal
        """
        # Link the previous item in the call stack to our
        # 'changed' signal so that when we change, it updates itself.
        if Datum._caller and not self in Datum._caller._connected_datums:
            self.changed.connect(Datum._caller.update)
            Datum._caller._connected_datums.add(self)


    def disconnect_parents(self):
        """ Disconnect all signals that are connected to the 'update' slot.
            Should be called when we're about to update (which re-establishes
            connections that are still active).
        """
        for d in self._connected_datums:
            d.disconnect(QtCore.SIGNAL('changed'), self.update)
        self._connected_datums = set()

    def emit_changed(self):
        """ Forces the datum to emit self.changed with appropriate values.
        """
        self.changed.emit(self, self._value, self._valid)

    def update(self):
        """ Updates self._valid and self._value, emitting self.changed
            if either has changed.
        """
        self.disconnect_parents()
        Datum._caller = self
        try:
            value = self.get_value()
            valid = True
        except:
            import traceback
            traceback.print_exc()
            value = self._value
            valid = False
        finally:
            Datum._caller = None

        if value != self._value or valid != self._valid:
            self._value = value
            self._valid = valid
            self.changed.emit(self, self._value, self._valid)

################################################################################

class EvalDatum(Datum):
    """ Datum where a value is calculated by running 'eval' on a user-provided
        string (or a user-provided input connection).
    """
    def __init__(self, name, node, data_type, expr, **kwargs):
        super(EvalDatum, self).__init__(name, node, data_type, **kwargs)
        self.set_expr(str(expr))

    def display_str(self):
        """ Returns the expression string.
        """
        if self.input_handler:
            return self.input_handler.get_display_str()
        else:
            return self._expr

    def set_expr(self, e):
        """ Sets the expression string and calls self.update
        """
        if not hasattr(self, '_expr') or e != self._expr:
            self._expr = e
            self.update()

    def get_value(self):
        """ Attempts to evaluate the expression and return a value.
            Raises an exception if this fails.
        """
        if self.input_handler:
            t = self.input_handler.get_value()
            self._expr = self.input_handler.get_display_str()
        else:
            t = eval(self._expr, NodeManager.make_dict())
            if not isinstance(t, self.data_type):
                t = self.data_type(t)

        return t

    def simple(self):
        """ Returns True if the expression can be directly converted into
            the desired data type; false otherwise.
        """
        try:    self.data_type(self._expr)
        except: return False
        else:   return True

################################################################################

class FloatDatum(EvalDatum):
    def __init__(self, name, node, value):
        super(FloatDatum, self).__init__(name, node, float, value,
                                         input_type=_SingleInput)

    def increment(self, delta):
        if self.simple():
            self += delta

    def __iadd__(self, delta):
        """ Increments this node's expression.
            Throws an exception if this operation fails.
        """
        self.set_expr(str(float(self._expr) + delta))
        return self

################################################################################

class IntDatum(EvalDatum):
    def __init__(self, name, node, value):
        super(IntDatum, self).__init__(name, node, int, value,
                                       input_type=_SingleInput)

    '''
    def __iadd__(self, delta):
        self.set_expr(str(self.value() + delta))
        return self

    def __isub__(self, delta):
        self.set_expr(str(self.value() - delta))
        return self
    '''

################################################################################

class NameDatum(EvalDatum):
    def __init__(self, name, node, value):
        super(NameDatum, self).__init__(name, node, Name, value, has_output=False)

    def display_str(self):
        return self._expr[1:-1]

    def set_expr(self, e):
        """ Sets the expression string and syncs children as needed.
        """
        super(NameDatum, self).set_expr("'%s'" % e)

class StringDatum(EvalDatum):
    def __init__(self, name, node, value):
        super(StringDatum, self).__init__(name, node, str, "'%s'" % value)

    def display_str(self):
        return self._expr[1:-1].replace("\\","")
    def set_expr(self, e):
        super(StringDatum, self).set_expr("'%s'" % e.replace("'","\\'"))

################################################################################

class ExpressionDatum(EvalDatum):
    def __init__(self, name, node, value):
        super(ExpressionDatum, self).__init__(
                name, node, Expression, value, input_type=_MultiInput)

################################################################################

class FunctionDatum(Datum):
    """ Represents a value calculated from a function of the parent node.
        Usually used for a node output value.
    """

    def __init__(self, name, node, function_name, data_type):
        self.function_name = function_name
        super(FunctionDatum, self).__init__(name, node, data_type)

    def display_str(self):
        if self._valid:     return 'Function'
        else:               return 'Function (invalid)'

    def can_edit(self): return False

    def get_value(self):
        """ Attempts to evaluate the given function and return a value.
            Raises an exception if this fails.
        """
        t = getattr(self.parent(), self.function_name)()
        if not isinstance(t, self.data_type):
            t = self.data_type(t)
        return t

################################################################################

class FloatFunctionDatum(FunctionDatum):
    """ Represents a calculated float value.
    """
    def __init__(self, name, node, function):
        super(FloatFunctionDatum, self).__init__(name, node, function, float)

    def get_expr(self):
        if self._valid:     return str(self._value)
        else:               return 'Invalid'

################################################################################

class ExpressionFunctionDatum(FunctionDatum):
    """ Represents a math expression calculated by a function.
    """

    def __init__(self, name, node, function):
        """ Pass in a parent node and a function to generate
            an output Expression.
        """

        # If we were passed in a function, get the function name instead.
        if not isinstance(function, str):
            function = function.__name__
        super(ExpressionFunctionDatum, self).__init__(
                name, node, function, Expression)

################################################################################

'''
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
        if s != self._script:
            self._script = s
            self.update()

    def display_str(self):
        return self._script

    def make_input(self, name, t, d):
        if name in ['name', 'x', 'y', 'z']:
            raise RuntimeError("Reserved name")
        if t not in [float, Expression]:
            raise RuntimeError("Invalid type")
        if name in ([o[0] for o in self._outputs] +
                    [i[0] for i in self._inputs]):
            raise RuntimeError("Duplicate input name")
        self._inputs.append([name, t])

        try:                    d[name] = getattr(self.node, name)
        except AttributeError:  d[name] = t()


    def make_output(self, name, var):
        if name in ['name','x','y','z']:
            raise RuntimeError("Reserved name")
        if type(var) not in [float, Expression]:
            raise RuntimeError("Invalid type")
        if name in ([o[0] for o in self._outputs] +
                    [i[0] for i in self._inputs]):
            raise RuntimeError("Duplicate output name")

        self._outputs.append([name, type(var), var])

    def get_value(self):
        self._inputs.clear()
        self._outputs.clear()

        d = {}
        d['input']  = lambda name, t: self.make_input(name, t, d)
        d['output'] = self.make_output
        try:
            exec self._script in d
        except Exception as e:
            print e
            raise

        return d

################################################################################

class OutputDatum(Datum):
    """ A datum representing a script output.
        self._value must be pro-actively set after script evaluation.
    """
    def __init__(self, node, T):
        super(OutputDatum, self).__init__(node, T, NoInput)
        self._value = self.data_type()

    def set_value(self, value):
        """ Stores a new value and triggers synching.
        """
        if value != self._value:
            self._value = value
            self.update()

    def get_expr(self):
        return str(self._value)

    def can_edit(self): return False

class FloatOutputDatum(OutputDatum):
    def __init__(self, node):
        super(FloatOutputDatum, self).__init__(node, float)

class ExpressionOutputDatum(OutputDatum):
    def __init__(self, node):
        super(ExpressionOutputDatum, self).__init__(node, Expression)
'''
