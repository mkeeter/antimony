from sb.nodes.node import Node2D
from sb.controls.circle import CircleControl
import sb.datum as datum

import fab.shapes

class Circle(Node2D):
    menu_name = "Circle"
    menu_category = "2D"

    name_prefix = 'c'
    _control = CircleControl

    def __init__(self, name, x, y, z, scale):
        super(Circle, self).__init__(name, x, y)
        self.object_datums['r'] = datum.FloatDatum(self, 100)
        self.object_datums['shape'] = datum.ExpressionFunctionDatum(
                self, self.make_shape)

    def make_shape(self):
        """ Return a math expression representing this circle.
        """
        return fab.shapes.circle(
                self.object_datums['x'].value,
                self.object_datums['y'].value,
                self.object_datums['r'].value)

