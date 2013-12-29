import base
import datum

class Circle(base.Node):

    def __init__(self, name, x, y, r):
        super(Circle, self).__init__(name)
        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)
        self._r = datum.FloatDatum(self, r)

        self._shape = datum.ExpressionFunctionDatum(self, self.make_shape)

        self.datums = [(i, getattr(self, '_'+i)) for i in
                       ('name','x','y','r','shape')]

    def make_shape(self):
        """ Return a math expression representing this circle.
        """
        return fab.shapes.circle(self.x, self.y, self.r)

import fab.shapes
