import base
import datum

from fab.expression import Expression

class Circle(base.Node):
    def __init__(self, name, x, y, r):
        super(Circle, self).__init__(name)
        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)
        self._r = datum.FloatDatum(self, r)

        self._shape = datum.FunctionDatum(self, self.make_shape, Expression)

        self.datums = [(i, getattr(self, '_'+i)) for i in ('name','x','y','r')]
        self.outputs = [('shape', self._shape)]

    def make_shape(self):
        return '+f%ff%f' % (self.x, self.y)
