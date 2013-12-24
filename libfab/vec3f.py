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

    @staticmethod
    def M(alpha, beta):
        """ @brief Generates M matrix for libfab's project and deproject functions.
            @param alpha Rotation about z axis
            @param beta Rotation about x axis.
            @returns (cos(a), sin(a), cos(b), sin(b)) as float array
        """
        return (ctypes.c_float*4)(
            cos(radians(alpha)), sin(radians(alpha)),
            cos(radians(beta)),  sin(radians(beta))
        )

    def project(self, alpha, beta):
        """ @brief Transforms from cell frame to view frame.
            @param alpha Rotation about z axis
            @param beta Rotation about x axis.
            @returns Projected Vec3f object
        """
        return libfab.project(self, self.M(alpha, beta))

    def deproject(self, alpha, beta):
        """ @brief Transforms from view frame to cell frame.
            @param alpha Rotation about z axis
            @param beta Rotation about x axis.
            @returns Deprojected Vec3f object
        """
        return libfab.deproject(self, self.M(alpha, beta))

    def __iter__(self):
        """ @brief Iterates over (x, y, z) list
        """
        return [self.x, self.y, self.z].__iter__()

from koko.c.libfab import libfab
