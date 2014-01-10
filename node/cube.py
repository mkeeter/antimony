import math

import base
import datum

class Cube(base.Node):
    def __init__(self, name, a, b):
        """ Constructs a cube.
            a and b should be Point node objects.
        """
        super(Cube, self).__init__(name)
        self.a, self.b = a, b
        a.parent = b.parent = self
        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.cube
        return control.cube.CubeControl

    def make_shape(self):
        return fab.shapes.cube(min(self.a.x, self.b.x),
                               max(self.a.x, self.b.x),
                               min(self.a.y, self.b.y),
                               max(self.a.y, self.b.y),
                               min(self.a.z, self.b.z),
                               max(self.a.z, self.b.z))

import fab.shapes
