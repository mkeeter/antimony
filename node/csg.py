import base
import datum

class Union(base.Node):
    def __init__(self, name, x, y):
        super(Union, self).__init__(name)

        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)
        self._shape = datum.ExpressionDatum(self, '')

        self.datums = [(i, getattr(self, '_'+i)) for i in ('name','shape')]

################################################################################

class Intersection(base.Node):
    def __init__(self, name, x, y):
        super(Intersection, self).__init__(name)

        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)

        self._A = datum.ExpressionDatum(self, '')
        self._B = datum.ExpressionDatum(self, '')

        self._shape = datum.ExpressionFunctionDatum(self, self.make_shape)

        self.datums = [(i, getattr(self, '_'+i)) for i in
                ('name', 'A', 'B', 'shape')]

    def make_shape(self):
        """ Computes the intersection of A and B
            (which are lovely auto-summing ExpressionDatum objects)
        """
        return self.A & self.B
