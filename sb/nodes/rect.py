from sb.nodes.node import Node
import sb.datum as datum
from sb.controls.rect import RectangleControl

import fab.shapes

class Rectangle(Node):
    menu_name = "Rectangle"
    menu_category = "2D"

    name_prefix = 'r'
    _control = RectangleControl

    def __init__(self, name, x, y, z, scale):
        super(Rectangle, self).__init__(name)
        self.object_datums['x0'] = datum.FloatDatum(self, x)
        self.object_datums['y0'] = datum.FloatDatum(self, y)
        self.object_datums['x1'] = datum.FloatDatum(self, x + scale)
        self.object_datums['y1'] = datum.FloatDatum(self, y + scale)
        self.object_datums['shape'] = datum.ExpressionFunctionDatum(
                self, self.make_shape)

    def make_shape(self):
        return fab.shapes.rectangle(
                self.object_datums['x0'].value,
                self.object_datums['x1'].value,
                self.object_datums['y0'].value,
                self.object_datums['y1'].value)
