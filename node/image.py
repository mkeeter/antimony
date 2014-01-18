import base
import datum

class ImageNode(base.Node):
    def __init__(self, name, x, y, expr):
        super(Image, self).__init__(name)
        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('expr', datum.ExpressionDatum(self, expr))

        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

        def get_control(self):
            import control.image
            return control.image.ImageControl

        def make_shape(self):
            return self.expr.value()
