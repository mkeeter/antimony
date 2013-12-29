import base
import datum

class Union(base.Node):
    def __init__(self, name, x, y):
        super(Union, self).__init__(name)

        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)

        self._A = datum.ExpressionDatum(self, 'f1')
        self._B = datum.ExpressionDatum(self, 'f1')

        self._shape = datum.ExpressionFunctionDatum(self, self.make_shape)

        self.datums = [(i, getattr(self, '_'+i)) for i in
                ('name','A','B','shape')]

    def make_shape(self):
        """ Computes the union of A and B
            (which are already auto-summing ExpressionDatum objects)
        """
        shape = self.A | self.B
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape

################################################################################

class Intersection(base.Node):
    def __init__(self, name, x, y):
        super(Intersection, self).__init__(name)

        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)

        self._A = datum.ExpressionDatum(self, 'f1')
        self._B = datum.ExpressionDatum(self, 'f1')

        self._shape = datum.ExpressionFunctionDatum(self, self.make_shape)

        self.datums = [(i, getattr(self, '_'+i)) for i in
                ('name', 'A', 'B', 'shape')]

    def make_shape(self):
        """ Computes the intersection of A and B
            (which are lovely auto-summing ExpressionDatum objects)
        """
        shape = self.A & self.B
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape
