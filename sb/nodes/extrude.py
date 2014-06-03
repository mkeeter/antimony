from sb.nodes.node import Node

from sb.controls.extrude import ExtrudeControl
import sb.datum

import fab.shapes

class Extrude(Node):
    menu_name = "Extrude (Z)"
    menu_category = "3D"
    name_prefix = 'e'

    def __init__(self, canvas, name, x, y, zmin, zmax, _scale, inpt):
        super().__init__(canvas, name)
        self.datums += [
                sb.datum.FloatDatum('x', self, x),
                sb.datum.FloatDatum('y', self, y),
                sb.datum.FloatDatum('zmin', self, zmin),
                sb.datum.FloatDatum('zmax', self, zmax),
                sb.datum.ExpressionDatum('input', self, inpt),
                sb.datum.ExpressionFunctionDatum(
                    'shape', self, self.make_shape),
                sb.datum.FloatDatum('_scale', self, _scale)]

        self.control = ExtrudeControl(canvas, self)

    @classmethod
    def new(cls, canvas, name, x, y, z, scale):
        return cls(canvas, name, x, y, z, z+scale, scale, None)

    def make_shape(self):
        """ Extrudes the given shape.
        """
        return fab.shapes.extrude_z(
                self.get_datum('input').value,
                self.get_datum('zmin').value,
                self.get_datum('zmax').value)

