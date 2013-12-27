import math

def wrapped(f):
    """ Function decorator that (for convenience) attempts to convert
        all arguments into Expression objects.
    """
    def wrapped(*args, **kwargs):
        return f(*[Expression(a) if not isinstance(a, Expression)
                                else a for a in args],
                **{k:Expression(a) if not isinstance(a, Expression)
                                else a for k, a in kwargs.items()})
    return wrapped

class Expression(object):
    """ Represents a math expression which may be parsed into a tree.
        May also keep track of expression bounds (if known).
    """

    def __init__(self, math):

        if type(math) in [int, float]:
            self.math = 'f%s' % math
        elif type(math) is str:
            self.math = math
        else:
            raise TypeError("Cannot construct a Expression from '%s'" % math)

        self.xmin = self.ymin = self.zmin = float('-infinity')
        self.xmax = self.ymax = self.zmax = float('+infinity')

    def __eq__(self, other):
        return all(getattr(self, a) == getattr(other, a) for a in
                ['math','xmin','ymin','zmin','xmax','ymax','zmax'])

    def __repr__(self):
        return self.math

    def to_tree(self):
        return tree.MathTree.from_expression(self)

    # Numerical addition
    @wrapped
    def __add__(self, rhs):
        return Expression('+' + self.math + rhs.math)
    @wrapped
    def __radd__(self, lhs):
        return Expression('+' + lhs.math + self.math)

    # Numerical subtraction
    @wrapped
    def __sub__(self, rhs):
        return Expression('-' + self.math + rhs.math)
    @wrapped
    def __rsub__(self, lhs):
        return Expression('-' + lhs.math + self.math)

    # Multiplication
    @wrapped
    def __mul__(self, rhs):
        return Expression('*' + self.math + rhs.math)
    @wrapped
    def __rmul__(self, lhs):
        return Expression('*' + lhs.math + self.math)

    # Division
    @wrapped
    def __div__(self, rhs):
        return Expression('/' + self.math + rhs.math)
    @wrapped
    def __rdiv__(self, lhs):
        return Expression('/' + lhs.math + self.math)

    # Negation
    def __neg__(self):
        return Expression('n' + self.math, shape=self.shape)

    # Logical intersection
    @wrapped
    def __and__(self, rhs):
        e = Expression('a' + self.math + rhs.math)
        e.xmin = max(self.xmin, rhs.xmin)
        e.xmax = min(self.xmax, rhs.xmax)
        e.ymin = max(self.ymin, rhs.ymin)
        e.ymax = min(self.ymax, rhs.ymax)
        return e
    @wrapped
    def __rand__(self, rhs):
        e = Expression('a' + lhs.math + self.math)
        e.xmin = max(lhs.xmin, self.xmin)
        e.xmax = min(lhs.xmax, self.xmax)
        e.ymin = max(lhs.ymin, self.ymin)
        e.ymax = min(lhs.ymax, self.ymax)
        return e

    # Logical union
    @wrapped
    def __and__(self, rhs):
        e = Expression('i' + self.math + rhs.math)
        e.xmin = min(self.xmin, rhs.xmin)
        e.xmax = max(self.xmax, rhs.xmax)
        e.ymin = min(self.ymin, rhs.ymin)
        e.ymax = max(self.ymax, rhs.ymax)
        return e
    @wrapped
    def __rand__(self, rhs):
        e = Expression('i' + lhs.math + self.math)
        e.xmin = min(lhs.xmin, self.xmin)
        e.xmax = max(lhs.xmax, self.xmax)
        e.ymin = min(lhs.ymin, self.ymin)
        e.ymax = max(lhs.ymax, self.ymax)
        return e

    def has_xy_bounds(self):
        """ Returns True if this expression has valid XY bounds.
        """
        return (not math.isinf(self.xmax - self.xmin) and
                not math.isinf(self.ymax - self.ymin))

    def has_xyz_bounds(self):
        """ Returns True if this expression has valid XYZ bounds.
        """
        return (not math.isinf(self.xmax - self.xmin) and
                not math.isinf(self.ymax - self.ymin) and
                not math.isinf(self.zmax - self.zmin))

    def get_xy_region(self, voxels_per_unit):
        """ Returns a region appropriate for 2D or height-map rendering.
        """
        zmin = 0 if math.isinf(self.zmin) else self.zmin
        zmax = 0 if math.isinf(self.zmin) else self.zmax

        return region.Region((self.xmin, self.ymin, zmin),
                             (self.xmax, self.ymax, zmax), voxels_per_unit)


import tree
import region
