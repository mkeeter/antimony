""" Module to import the libfab shared C library. """

import ctypes
import os
import sys

# Here are a few likely filenames
base = os.path.abspath(sys.argv[0])
if sys.argv[0]: base = os.path.dirname(base)

libname = 'libcfab' + ('.dylib' if 'Darwin' in os.uname() else '.so')
filenames =[
    os.path.join(base, 'fab/', libname),
    os.path.join(base, '../lib/', libname),
    os.path.join(base, '../Frameworks/', libname),
    libname
]

for filename in filenames:
    try:
        libfab = ctypes.CDLL(filename)
    except OSError:
        continue
    else:
        break
else:
    raise OSError('libcfab not found')


# Helper functions for pointer and pointer to pointer
def p(t):   return ctypes.POINTER(t)
def pp(t):  return p(p(t))

################################################################################

# util/region.h
from region import Region

libfab.split.argtypes = [Region, p(Region), ctypes.c_int]
libfab.split.restype  = ctypes.c_int

libfab.split_xy.argtypes = [Region, p(Region), ctypes.c_int]
libfab.split_xy.restype  = ctypes.c_int

libfab.octsect.argtypes = [Region, Region*8]
libfab.octsect.restype  = ctypes.c_uint8

libfab.build_arrays.argtypes = [p(Region)] + [ctypes.c_float]*6
libfab.free_arrays.argtypes  = [p(Region)]

################################################################################

class MathTreeP(ctypes.c_void_p):   pass

# tree/solver.h
libfab.render8.argtypes  = [
    MathTreeP, Region, pp(ctypes.c_uint8), p(ctypes.c_int)
]
libfab.render16.argtypes = [
    MathTreeP, Region, pp(ctypes.c_uint16), p(ctypes.c_int)
]

# tree/tree.h
libfab.free_tree.argtypes = [MathTreeP]

libfab.print_tree.argtypes = [MathTreeP]
libfab.fdprint_tree.argtypes = [MathTreeP, ctypes.c_int]

libfab.clone_tree.argtypes = [MathTreeP]
libfab.clone_tree.restype  =  MathTreeP

libfab.count_nodes.argtypes = [MathTreeP]
libfab.count_nodes.restype  = ctypes.c_uint

# tree/eval.h
from interval import Interval

libfab.eval_i.argtypes = [MathTreeP, Interval, Interval, Interval]
libfab.eval_i.restype  =  Interval

# tree/parser.h
libfab.parse.argtypes = [p(ctypes.c_char)]
libfab.parse.restype  =  MathTreeP

################################################################################

# formats/png.c
libfab.save_png16L.argtypes = [p(ctypes.c_char), ctypes.c_int,
                                ctypes.c_int, ctypes.c_float*6,
                                pp(ctypes.c_uint16)]


libfab.depth_blit.argtypes = (
    [pp(ctypes.c_uint8), pp(ctypes.c_uint8), pp(ctypes.c_uint8*3)] +
    [ctypes.c_int]*4 + [ctypes.c_float]*3
)


libfab.load_png_stats.argtypes = (
    [p(ctypes.c_char)] + [p(ctypes.c_int)]*2 + [p(ctypes.c_float)]*3
)

libfab.load_png.argtypes = [p(ctypes.c_char), pp(ctypes.c_uint16)]

del p, pp
