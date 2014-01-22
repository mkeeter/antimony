import base
import datum

class RotateZ(base.Node3D):
    def __init__(self, name, x, y, z):
        super(RotateZ, self).__init__(name, x, y, z)
        self.add_datum('angle', datum.FloatDatum(self, 20))

        self.add_datum('input', datum.ExpressionDatum(self, None))
        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.rotate
        return control.rotate.RotateZ

    def make_shape(self):
        """ Returns a shape, rotated about this node.
        """
        try:
            return fab.shapes.move(
                    fab.shapes.rotate_z(
                        fab.shapes.move(self.input, -self.x, -self.y, -self.z),
                        self.angle),
                    self.x, self.y, self.z)
        except Exception as e:
            print e
            raise

import fab.shapes
