import math

import base
import datum

class Triangle(base.Node):
    def __init__(self, name, a, b, c):
        """ Constructs a triangle.
            a, b, and c should be Point node objects.
        """
        super(Triangle, self).__init__(name)
        self.a, self.b, self.c  = a, b, c

        self._x = datum.FunctionDatum(self, self.get_x, float)
        self._y = datum.FunctionDatum(self, self.get_y, float)
        self._shape = datum.ExpressionDatum(self, self.make_shape)

        self.datums = [(i, getattr(self, '_'+i)) for i in
                       ('name','shape')]


    def get_x(self):
        """ Calculate x as the arithmetic mean of points x coordinates.
        """
        return (self.a.x + self.b.x + self.c.x) / 3.0


    def get_y(self):
        """ Calculate y as the arithmetic mean of points y coordinates.
        """
        return (self.a.y + self.b.y + self.c.y) / 3.0


    def make_shape(self):
        x0, y0 = self.a.x, self.a.y
        x1, y1 = self.b.x, self.b.y
        x2, y2 = self.c.x, self.c.y

        # Find the angles of the points about the center
        xm = (x0 + x1 + x2) / 3.
        ym = (y0 + y1 + y2) / 3.
        angles = [math.atan2(y - ym, x - xm) for x, y in [(x0,y0), (x1,y1), (x2,y2)]]

        # Sort the angles so that the smallest one is first
        if angles[1] < angles[0] and angles[1] < angles[2]:
            angles = [angles[1], angles[2], angles[0]]
        elif angles[2] < angles[0] and angles[2] < angles[1]:
            angles = [angles[2], angles[0], angles[1]]

        # Enforce that points must be in clockwise order by swapping if necessary
        if angles[2] > angles[1]:
            x0, y0, x1, y1 = x1, y1, x0, y0

        def edge(x, y, dx, dy):
            # dy*(X-x)-dx*(Y-y)
            return '-*f%(dy)g-Xf%(x)g*f%(dx)g-Yf%(y)g' % locals()

        e0 = edge(x0, y0, x1-x0, y1-y0)
        e1 = edge(x1, y1, x2-x1, y2-y1)
        e2 = edge(x2, y2, x0-x2, y0-y2)

        # -min(e0, min(e1, e2))
        s = Expression('ni%(e0)si%(e1)s%(e2)s' % locals())
        s.xmin, s.xmax = min(x0, x1, x2), max(x0, x1, x2)
        s.ymin, s.ymax = min(y0, y1, y2), max(y0, y1, y2)

        return s

from fab.expression import Expression
