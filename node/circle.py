import base
import datum

from fab.expression import Expression

class Circle(base.Node):
    def __init__(self, name, x, y, r):
        super(Circle, self).__init__(name)
        self._x = datum.FloatDatum(self, x)
        self._y = datum.FloatDatum(self, y)
        self._r = datum.FloatDatum(self, r)

        self._shape = datum.ExpressionDatum(self, self.make_shape)

        self.datums = [(i, getattr(self, '_'+i)) for i in
                       ('name','x','y','r','shape')]

    def make_shape(self):
        """ Return a math expression representing this circle.
        """
        e = Expression('-r+q-Xf%gq-Yf%gf%g' % (self.x, self.y, self.r))
        e.xmin, e.xmax = self.x - self.r, self.x + self.r
        e.ymin, e.ymax = self.y - self.r, self.y + self.r
        return e
