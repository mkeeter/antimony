import base
import datum

class GetBounds(base.Node):
    def __init__(self, name, x, y):
        super(GetBounds, self).__init__(name)

        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)

        self._input = datum.ExpressionDatum(self, 'f1')

        self.datums = [(i, getattr(self, '_'+i)) for i in ('name','input')]
        for i in 'xmin','ymin','zmin','xmax','ymax','zmax':
            setattr(self, '_'+i, datum.FloatFunctionDatum(
                self, lambda s=i: getattr(self.input, s)))
            self.datums.append((i, getattr(self, '_'+i)))


class SetBounds(base.Node):
    def __init__(self, name, x, y):
        super(SetBounds, self).__init__(name)

        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)

        self._input = datum.ExpressionDatum(self, 'f1')

        self.datums = [('name',self._name), ('input',self._input)]

        for i in 'xmin','ymin','zmin','xmax','ymax','zmax':
            setattr(self, '_'+i, datum.FloatDatum(self, 0))
            self.datums.append((i, getattr(self, '_'+i)))

        self._output = datum.ExpressionFunctionDatum(self, self.make_shape)
        self.datums.append(('output',self._output))

    def make_shape(self):
        s = self.input
        for i in 'xmin','ymin','zmin','xmax','ymax','zmax':
            setattr(s, i, getattr(self, i))
        return s


