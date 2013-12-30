import math

import base
import datum

class Triangle(base.Node):
    def __init__(self, name, a, b, c):
        """ Constructs a triangle.
            a, b, and c should be Point node objects.
        """
        super(Triangle, self).__init__(name)
        self.a, self.b, self.c  = a, b, c

        self.add_datum('shape', datum.ExpressionFunctionDatum(self, self.make_shape))

    def make_shape(self):
        return fab.shapes.triangle(self.a.x, self.a.y,
                                   self.b.x, self.b.y,
                                   self.c.x, self.c.y)

import fab.shapes
