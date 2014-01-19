import base
import datum

class Scale(base.Node3D):
    def __init__(self, name, x, y, z, sx, sy, sz):
        super(Scale, self).__init__(name, x, y, z)
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

        # Apply x, y, and z scale factors.
        sx = fab.shapes.scale_x(shape, self.x, self.sx)
        sxy = fab.shapes.scale_y(sx,   self.y, self.sy)
        sxyz = fab.shapes.scale_z(sxy, self.z, self.sz)

        return sxyz


import fab.shapes
