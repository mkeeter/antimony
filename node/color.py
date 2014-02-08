import base
import datum

class ColorNode(base.Node3D):
    """ Applies a color to a given node.
    """
    def __init__(self, name, x, y, z):
        super(ColorNode, self).__init__(name, x, y, z)
        self.add_datum('R', datum.FloatDatum(self, 1.0))
        self.add_datum('G', datum.FloatDatum(self, 1.0))
        self.add_datum('B', datum.FloatDatum(self, 1.0))

        self.add_datum('input',
                datum.ExpressionDatum(self, "None"))
        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.color
        return control.color.ColorNodeControl

    def make_shape(self):
        e = self.input
        e.color = (self.R, self.G, self.B)
        return e
