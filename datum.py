import node

class Datum(object):
    def __init__(self, expr, type):
        self.type    = type
        self.expr    = repr(expr)

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
        t = eval(self.expr, node.dict())
        if not isinstance(t, self.type):    t = self.type(t)
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

################################################################################

import name
class NameDatum(Datum):
    def __init__(self, value):
        super(NameDatum, self).__init__(value, name.Name)

