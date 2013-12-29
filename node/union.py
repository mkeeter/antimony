import base
import datum

class Union(base.Node):
    def __init__(self, name, x, y):
        super(Union, self).__init__(name)

        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)
        self._shape = datum.ExpressionDatum(self, '')

        self.datums = [(i, getattr(self, '_'+i)) for i in ('name','shape')]
