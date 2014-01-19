import base
import datum

class EquationViewer(base.Node3D):
    def __init__(self, name, x, y, z):
        super(EquationViewer, self).__init__(name, x, y, z)

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
