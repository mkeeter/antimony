import base
import datum

class Circle(base.Node):

    def __init__(self, name, x, y, r):
        super(Circle, self).__init__(name)
        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('r', datum.FloatDatum(self, r))

        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, self.make_shape))

    def get_control(self, is_child):
        import control.circle
        return control.circle.CircleControl

    def make_shape(self):
        """ Return a math expression representing this circle.
        """
        return fab.shapes.circle(self.x, self.y, self.r)

import fab.shapes
