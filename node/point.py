import base
import datum

class Point(base.Node):

    def __init__(self, name, x, y):

        super(Point, self).__init__(name)
        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)

        self.datums = [(i, getattr(self, '_'+i)) for i in ('name','x','y')]

