import base
import datum

class EquationViewer(base.Node):
    def __init__(self, name, x, y, z):
        super(EquationViewer, self).__init__(name)
        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('z', datum.FloatDatum(self, z))

        self.add_datum('input', datum.ExpressionDatum(self, "None"))
        self.add_datum('output', datum.ExpressionFunctionDatum(self, "make_shape"))

    def get_control(self):
        import control.equation
        return control.equation.EquationViewerControl

    def make_shape(self):
        shape = self.input
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape
