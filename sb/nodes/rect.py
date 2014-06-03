from sb.nodes.node import Node
import sb.datum as datum
from sb.controls.rect import RectangleControl

import fab.shapes

class Rectangle(Node):
    menu_name = "Rectangle"
    menu_category = "2D"
    name_prefix = 'r'

    def __init__(self, canvas, name, x0, y0, x1, y1):
        super().__init__(canvas, name)
        self.datums += [
                datum.FloatDatum('x0', self, x0),
                datum.FloatDatum('y0', self, y0),
                datum.FloatDatum('x1', self, x1),
                datum.FloatDatum('y1', self, y1),
                datum.ExpressionFunctionDatum('shape', self, self.make_shape)]
        self.control = RectangleControl(canvas, self)

    @classmethod
    def new(cls, canvas, name, x, y, z, scale):
        return cls(canvas, name, x, y, x + scale, y + scale)

    def make_shape(self):
        return fab.shapes.rectangle(
                self.get_datum('x0').value, self.get_datum('x1').value,
                self.get_datum('y0').value, self.get_datum('y1').value)
