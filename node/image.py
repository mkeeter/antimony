import base
import datum

class ImageNode(base.Node):
    def __init__(self, name, x, y, expr, w, h):
        super(ImageNode, self).__init__(name)
        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('scale', datum.FloatDatum(self, 1))

        # Secret datums (used in reconstruction but nowhere else)
        self.add_datum('expr', datum.ExpressionDatum(self, "'%s'" % expr))
        self.add_datum('w', datum.FloatDatum(self, w))
        self.add_datum('h', datum.FloatDatum(self, h))

        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.image
        return control.image.ImageControl

    def make_shape(self):
        e = self.expr
        e.xmin, e.xmax = -1, self.w + 1
        e.ymin, e.ymax = -1, self.h + 1
        e = scale_x(e, 0, self.scale)
        e = scale_y(e, self.h/2, -self.scale)
        e = move(e, self.x, self.y)
        return e

from fab.expression import Expression
from fab.shapes import move, scale_x, scale_y
