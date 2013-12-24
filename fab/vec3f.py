import ctypes
from math import sin, cos, radians, sqrt

class Vec3f(ctypes.Structure):
    """ @class Vec3f
        @brief Three-element vector with overloaded arithmetic operators.
    """
    _fields_ = [('x', ctypes.c_float),
                ('y', ctypes.c_float),
                ('z', ctypes.c_float)]
    def __init__(self, x=0., y=0., z=0.):
        try:                x = list(x)
        except TypeError:   ctypes.Structure.__init__(self, x, y, z)
        else:               ctypes.Structure.__init__(self, x[0], x[1], x[2])

    def __str__(self):
        return "(%g, %g, %g)" % (self.x, self.y, self.z)
    def __repr__(self):
        return "Vec3f(%g, %g, %g)" % (self.x, self.y, self.z)
    def __add__(self, rhs):
        return Vec3f(self.x + rhs.x, self.y + rhs.y, self.z + rhs.z)
    def __sub__(self, rhs):
        return Vec3f(self.x - rhs.x, self.y - rhs.y, self.z - rhs.z)
    def __div__(self, rhs):
        return Vec3f(self.x/rhs, self.y/rhs, self.z/rhs)
    def __neg__(self):
        return Vec3f(-self.x, -self.y, -self.z)
    def length(self):
        return sqrt(self.x**2 + self.y**2 + self.z**2)
    def copy(self):
        return Vec3f(self.x, self.y, self.z)

    def __iter__(self):
        """ @brief Iterates over (x, y, z) list
        """
        return [self.x, self.y, self.z].__iter__()
