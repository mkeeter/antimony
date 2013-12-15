import node
import datum

class Point(node.Node):

    def __init__(self, name, x, y):
        super(Point, self).__init__()

        self._x = datum.FloatDatum(x)
        self._y = datum.FloatDatum(y)
        self._name = datum.NameDatum(name)

        self.inputs = [(i, getattr(self, '_'+i)) for i in ('name','x','y')]
