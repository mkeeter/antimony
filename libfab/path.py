import ctypes

class Path(ctypes.Structure):
    """ @class Path
        @brief C data structure containing a doubly linked list.
    """
    def __repr__(self):
        return 'pt(%g, %g) at %s' % \
            (self.x, self.y, hex(ctypes.addressof(self)))
    def __eq__(self, other):
        if other is None:   return False
        return ctypes.addressof(self) == ctypes.addressof(other)
    def __ne__(self, other):
        if other is None:   return False
        return not (self == other)

def p(t):   return ctypes.POINTER(t)
def pp(t):  return p(p(t))

Path._fields_ = [
    ('prev', p(Path)), ('next', p(Path)),
    ('x', ctypes.c_float), ('y', ctypes.c_float), ('z', ctypes.c_float),
    ('ptr', pp(Path*2))
]

del p, pp
