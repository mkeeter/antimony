import base
import datum

import operator

class Array2D(base.Node2D):
    def __init__(self, name, x, y, i, j, dx, dy):
        super(Array2D, self).__init__(name, x, y)

        self.add_datum('i', datum.IntDatum(self, i))
        self.add_datum('j', datum.IntDatum(self, j))
        self.add_datum('dx', datum.FloatDatum(self, dx))
        self.add_datum('dy', datum.FloatDatum(self, dy))

        self.add_datum('input', datum.ExpressionDatum(self, None))
        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.iterate
        return control.iterate.Array2DControl

    def make_shape(self):
        if self.i <= 0 or self.j <= 0:
            raise RuntimeError("Invalid number of array objects")
        shape = self.input

        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")

        return reduce(operator.or_, [
            reduce(operator.or_, [
                fab.shapes.move(shape, i*self.dx, j*self.dy) 
                    for i in range(self.i)])
                for j in range(self.j)])

import fab.shapes
