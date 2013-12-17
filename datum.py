import node

class Datum(object):
    stack = []

    def __init__(self, expr, type):
        self.type    = type
        self.expr    = repr(expr)

        self.parents = set()
        self.children = set()

        # Check to make sure that the initial expression is valid.
        self.eval()

    def value(self):
        """ Gets the value from this datum,
            or evaluated from the expression.

            Raises an exception if expression evaluation fails.
        """
        return self.eval()

    def eval(self):
        """ Attempts to evaluate the expression and return a value.
            Raises an exception if this fails.
        """

        if self in Datum.stack:
            raise RuntimeError("Infinite recursion in Datum.")

        # If this was called from another datum, then register it as a
        # parent and register self as its children
        if Datum.stack:
            Datum.stack[-1].parents.add(self)
            self.children.add(Datum.stack[-1])

        # Clear parents and remove references to self in parents
        for p in self.parents:
            p.children.remove(self)
        self.parents = set()

        # Put oneself down on the stack.
        Datum.stack.append(self)

        try:
            t = eval(self.expr, node.dict())
            if not isinstance(t, self.type):    t = self.type(t)
        except:     raise
        finally:    Datum.stack.pop()

        return t

    def valid(self):
        """ Returns True if this datum's expression is valid, false otherwise.
        """
        try:    self.eval()
        except: return False
        else:   return True

################################################################################

class FloatDatum(Datum):
    def __init__(self, value):
        super(FloatDatum, self).__init__(value, float)
    def simple(self):
        try:    float(self.expr)
        except: return False
        else:   return True

################################################################################

import name
class NameDatum(Datum):
    def __init__(self, value):
        super(NameDatum, self).__init__(value, name.Name)

