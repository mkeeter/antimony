import node

class Datum(object):
    stack = []

    def __init__(self, node, expr, type):
        self.node    = node
        self._expr    = repr(expr)
        self.type    = type

        self.parents = set()
        self.children = set()

        # Check to make sure that the initial expression is valid.
        self.eval()

    def get_expr(self):
        """ Returns the expression string.
        """
        return self._expr

    def set_expr(self, e):
        """ Sets the expression string.
            Updates node, editor, and children as needed
            (which may trigger a canvas Refresh)
        """
        if e == self._expr:     return

        self._expr = e
        self.update_children()
        self.node.control.update()
        if self.node.control.editor:
            self.node.control.editor.update()
            self.node.control.editor.sync_text()


    def update_children(self):
        """ Update the node control and editor for all children of this Datum
            This may trigger a canvas refresh operation.
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

        try:
            t = eval(self._expr, node.dict())
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
        """ Sets the expression string and updates children as needed.
        """
        e = "'%s'" % e
        if e != self._expr:
            self._expr = e
            self.update_children()

