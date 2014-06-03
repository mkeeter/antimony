from sb.nodes.node import Node2D
from sb.controls.circle import CircleControl
import sb.datum as datum

import fab.shapes

class Circle(Node2D):
    menu_name = "Circle"
    menu_category = "2D"
    name_prefix = 'c'

    def __init__(self, canvas, name, x, y, r):
        super().__init__(canvas, name, x, y)
        self.datums += [
                datum.FloatDatum('r', self, 100),
                datum.ExpressionFunctionDatum('shape', self, self.make_shape)]
        self.control = CircleControl(canvas, self)

    @classmethod
    def new(cls, canvas, name, x, y, z, scale):
        return cls(canvas, name, x, y, scale)

    def make_shape(self):
        """ Return a math expression representing this circle.
        """
        return fab.shapes.circle(
                self.get_datum('x').value,
                self.get_datum('y').value,
                self.get_datum('r').value)

