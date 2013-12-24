import ctypes

from koko.c.interval import Interval

class ASDF(ctypes.Structure):
    """ @class ASDF
        @brief C data structure describing an ASDF.
    """
    pass
ASDF._fields_ = [('state', ctypes.c_int),
                 ('X', Interval), ('Y', Interval), ('Z', Interval),
                 ('branches', ctypes.POINTER(ASDF)*8),
                 ('d', ctypes.c_float*8),
                 ('data', ctypes.c_void_p)]
