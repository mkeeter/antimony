import base
import datum

class Scale(base.Node):
    def __init__(self, name, x, y, z, sx, sy, sz):
        super(Scale, self).__init__(name)
        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('z', datum.FloatDatum(self, z))
        self.add_datum('sx', datum.FloatDatum(self, sx))
        self.add_datum('sy', datum.FloatDatum(self, sy))
        self.add_datum('sz', datum.FloatDatum(self, sz))

        self.add_datum('input', datum.ExpressionDatum(self, None))

        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.scale
        return control.scale.ScaleControl

    def make_shape(self):
        """ Computes a scaled version of the given input shape.
        """
        shape = self.input
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape.scale_x(self.x, self.sx).\
                     scale_y(self.y, self.sy).\
                     scale_z(self.z, self.sz)


import fab.shapes
