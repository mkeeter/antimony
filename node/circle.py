import base
import datum

class Circle(base.Node2D):

    def __init__(self, name, x, y, r):
        super(Circle, self).__init__(name, x, y)
        self.add_datum('r', datum.FloatDatum(self, r))

        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.circle
        return control.circle.CircleControl

    def make_shape(self):
        """ Return a math expression representing this circle.
        """
        return fab.shapes.circle(self.x, self.y, self.r)

import fab.shapes
