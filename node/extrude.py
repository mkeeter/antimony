import base
import datum

class ExtrudeZ(base.Node3D):
    def __init__(self, name, x, y, z, top):
        super(ExtrudeZ, self).__init__(name, x, y, z)
        self.add_datum('top', datum.FloatDatum(self, top))

        self.add_datum('input', datum.ExpressionDatum(self, None))

        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.extrude
        return control.extrude.ExtrudeZControl

    def make_shape(self):
        """ Extrudes the given shape on the Z axis.
        """
        return fab.shapes.extrude_z(self.input, min(self.z, self.top),
                                                max(self.z, self.top))

import fab.shapes
