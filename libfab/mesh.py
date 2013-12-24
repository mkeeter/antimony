import ctypes

from koko.c.interval import Interval

class Mesh(ctypes.Structure):
    """ @class Mesh
        @brief C data structure describing a Mesh
    """
    _fields_ = [
        ('vdata', ctypes.POINTER(ctypes.c_float)),
        ('vcount', ctypes.c_uint32),
        ('valloc', ctypes.c_uint32),
        ('tdata', ctypes.POINTER(ctypes.c_uint32)),
        ('tcount', ctypes.c_uint32),
        ('talloc', ctypes.c_uint32),
        ('X', Interval), ('Y', Interval), ('Z', Interval)
    ]

