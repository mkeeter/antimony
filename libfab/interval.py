import ctypes

class Interval(ctypes.Structure):
    """ @class Interval
        @brief Interval containing upper and lower bounds with overloaded arithmetic operators
    """
    _fields_ = [('lower', ctypes.c_float),
                ('upper', ctypes.c_float)]

    def __init__(self, lower=0, upper=None):
        """ @brief Constructor for Interval
            @param lower Lower bound
            @param upper Upper bound (if None, lower is used)
        """
        if upper is None:   upper = lower
        if isinstance(lower, Interval):
            lower, upper = lower.lower, lower.upper
        ctypes.Structure.__init__(self, lower, upper)

    def __str__(self):
        return "[%g, %g]" % (self.lower, self.upper)
    def __repr__(self):
        return "Interval(%g, %g)" % (self.lower, self.upper)
    def __add__(self, rhs):     return libfab.add_i(self, Interval(rhs))
    def __radd__(self, lhs):    return libfab.add_i(Interval(lhs), self)
    def __sub__(self, rhs):     return libfab.sub_i(self, Interval(rhs))
    def __rsub__(self, lhs):    return libfab.sub_i(Interval(lhs), self)
    def __mul__(self, rhs):     return libfab.mul_i(self, Interval(rhs))
    def __rmul__(self, lhs):    return libfab.mul_i(Interval(lhs), self)
    def __div__(self, rhs):     return libfab.div_i(self, Interval(rhs))
    def __rdiv__(self, lhs):    return libfab.div_i(Interval(lhs), self)
    def __neg__(self):          return libfab.neg_i(self)

    @staticmethod
    def sqrt(i):    return libfab.sqrt_i(i)
    @staticmethod
    def pow(i, e):  return libfab.pow_i(i, e)
    @staticmethod
    def sin(i):     return libfab.sin_i(i)
    @staticmethod
    def cos(i):     return libfab.cos_i(i)
    @staticmethod
    def tan(i):     return libfab.tan_i(i)

    def copy(self):
        return Interval(self.lower, self.upper)

from koko.c.libfab import libfab
