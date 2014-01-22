import base
import datum

class RotateNode(base.Node3D):
    def __init__(self, name, x, y, z):
        super(RotateNode, self).__init__(name, x, y, z)
        self.add_datum('angle', datum.FloatDatum(self, 20))

        self.add_datum('input', datum.ExpressionDatum(self, None))
        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))


class RotateX(RotateNode):
    def get_control(self):
        import control.rotate
        return control.rotate.RotateX

    def make_shape(self):
        """ Returns a shape, rotated about this node.
        """
        return fab.shapes.move(
                fab.shapes.rotate_x(
                    fab.shapes.move(self.input, -self.x, -self.y, -self.z),
                    self.angle),
                self.x, self.y, self.z)


class RotateY(RotateNode):
    def get_control(self):
        import control.rotate
        return control.rotate.RotateY

    def make_shape(self):
        """ Returns a shape, rotated about this node.
        """
        return fab.shapes.move(
                fab.shapes.rotate_y(
                    fab.shapes.move(self.input, -self.x, -self.y, -self.z),
                    self.angle),
                self.x, self.y, self.z)


class RotateZ(RotateNode):
    def get_control(self):
        import control.rotate
        return control.rotate.RotateZ

    def make_shape(self):
        """ Returns a shape, rotated about this node.
        """
        return fab.shapes.move(
                fab.shapes.rotate_z(
                    fab.shapes.move(self.input, -self.x, -self.y, -self.z),
                    self.angle),
                self.x, self.y, self.z)
import fab.shapes
