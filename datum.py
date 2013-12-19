import node

class Datum(object):
    stack = []

    def __init__(self, node, expr, type):
        self.node    = node
        self.expr    = repr(expr)
        self.type    = type

        self.parents = set()
        self.children = set()

        # Check to make sure that the initial expression is valid.
        self.eval()

    def update_children(self):
        """ Update the node control and editor for all children of this Datum
        """
        for c in self.children:
            c.node.control.update()
            if c.node.control.editor:
                c.node.control.editor.update()

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
        for d in Datum.stack:
            d.parents.add(self)
            self.children.add(d)

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
    def __init__(self, node, value):
        super(FloatDatum, self).__init__(node, value, float)
    def simple(self):
        try:    float(self.expr)
        except: return False
        else:   return True

################################################################################

import name
class NameDatum(Datum):
    def __init__(self, node, value):
        super(NameDatum, self).__init__(node, value, name.Name)

