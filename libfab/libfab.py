""" Module to import the libfab shared C library. """

import ctypes
import os
import sys

# Here are a few likely filenames
base = os.path.abspath(sys.argv[0])
if sys.argv[0]: base = os.path.dirname(base)

libname = 'libfab' + ('.dylib' if 'Darwin' in os.uname() else '.so')
filenames =[
    os.path.join(base, 'libfab/', libname),
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
    print 'Error: libfab not found'
    sys.exit(1)


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

libfab.octsect_overlap.argtypes = [Region, Region*8]
libfab.octsect_overlap.restype  = ctypes.c_uint8

libfab.build_arrays.argtypes = [p(Region)] + [ctypes.c_float]*6
libfab.free_arrays.argtypes  = [p(Region)]

# util/vec3f.h
from vec3f import Vec3f
libfab.deproject.argtypes = [Vec3f, ctypes.c_float*4]
libfab.deproject.restype  =  Vec3f

libfab.project.argtypes = [Vec3f, ctypes.c_float*4]
libfab.project.restype  =  Vec3f

################################################################################

class MathTreeP(ctypes.c_void_p):   pass
class PackedTreeP(ctypes.c_void_p): pass

# tree/solver.h
libfab.render8.argtypes  = [
    PackedTreeP, Region, pp(ctypes.c_uint8), p(ctypes.c_int)
]
libfab.render16.argtypes = [
    PackedTreeP, Region, pp(ctypes.c_uint16), p(ctypes.c_int)
]

# tree/tree.h

libfab.free_tree.argtypes = [MathTreeP]

libfab.print_tree.argtypes = [MathTreeP]
libfab.fdprint_tree.argtypes = [MathTreeP, ctypes.c_int]
libfab.fdprint_tree_verbose.argtypes = [MathTreeP, ctypes.c_int]

libfab.clone_tree.argtypes = [MathTreeP]
libfab.clone_tree.restype  =  MathTreeP

libfab.count_nodes.argtypes = [MathTreeP]
libfab.count_nodes.restype  = ctypes.c_uint

libfab.dot_arrays.argtypes = [MathTreeP, p(ctypes.c_char)]
libfab.dot_tree.argtypes = [MathTreeP, p(ctypes.c_char)]

# tree/packed.h
libfab.make_packed.argtypes = [MathTreeP]
libfab.make_packed.restype  =  PackedTreeP

libfab.free_packed.argtypes = [PackedTreeP]

# tree/eval.h
from interval import Interval

libfab.eval_i.argtypes = [PackedTreeP, Interval, Interval, Interval]
libfab.eval_i.restype  =  Interval

# tree/parser.h
libfab.parse.argtypes = [p(ctypes.c_char)]
libfab.parse.restype  =  MathTreeP

################################################################################

# asdf/asdf.h
from koko.c.asdf import ASDF

libfab.build_asdf.argtypes = [
    PackedTreeP, Region, ctypes.c_bool, p(ctypes.c_int)
]
libfab.build_asdf.restype  =  p(ASDF)

libfab.free_asdf.argtypes = [p(ASDF)]

libfab.asdf_root.argtypes = [PackedTreeP, Region]
libfab.asdf_root.restype  =  p(ASDF)

libfab.count_leafs.argtypes = [p(ASDF)]
libfab.count_leafs.restype  = ctypes.c_int

libfab.count_cells.argtypes = [p(ASDF)]
libfab.count_cells.restype = ctypes.c_int

libfab.asdf_scale.argtypes = [p(ASDF), ctypes.c_float]

libfab.asdf_slice.argtypes = [p(ASDF), ctypes.c_float]
libfab.asdf_slice.restype  =  p(ASDF)

libfab.find_dimensions.argtypes = [p(ASDF)] + [p(ctypes.c_int)]*3
libfab.get_d_from_children.argtypes = [p(ASDF)]

libfab.interpolate.argtypes = [p(ctypes.c_float)] + [ctypes.c_float]*9
libfab.interpolate.restype  = ctypes.c_float

libfab.get_depth.argtypes = [p(ASDF)]
libfab.get_depth.restype  = ctypes.c_int

libfab.asdf_get_max.argtypes = [p(ASDF)]
libfab.asdf_get_max.restype  = ctypes.c_float

libfab.asdf_get_min.argtypes = [p(ASDF)]
libfab.asdf_get_min.restype  = ctypes.c_float

libfab.asdf_histogram.argtypes = [
    p(ASDF), p(ctypes.c_int*4), ctypes.c_int
]

libfab.simplify.argtypes = [p(ASDF), ctypes.c_bool]


# asdf/import.h
libfab.import_vol_region.argtypes = (
    [p(ctypes.c_char)] + [ctypes.c_int]*3 +
    [Region, ctypes.c_int, ctypes.c_float,
     ctypes.c_bool, ctypes.c_bool]
)
libfab.import_vol_region.restype  = p(ASDF)

libfab.import_vol.argtypes = [
    p(ctypes.c_char), ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_float, ctypes.c_float, ctypes.c_bool, ctypes.c_bool
]
libfab.import_vol.restype  = p(ASDF)

libfab.import_lattice.argtypes = [
    pp(ctypes.c_float), ctypes.c_int, ctypes.c_int,
    ctypes.c_float, ctypes.c_float, ctypes.c_bool
]
libfab.import_lattice.restype = p(ASDF)


# asdf/render.h
libfab.render_asdf.argtypes = [
    p(ASDF), Region, ctypes.c_float*4, pp(ctypes.c_uint16)
]
libfab.render_asdf_shaded.argtypes = [
    p(ASDF), Region, ctypes.c_float*4,
    pp(ctypes.c_uint16), pp(ctypes.c_uint16), pp(ctypes.c_uint8*3)
]
libfab.draw_asdf_cells.argtypes = [p(ASDF), Region, pp(ctypes.c_uint8*3)]

libfab.draw_asdf_distance.argtypes = [
    p(ASDF), Region, ctypes.c_float, ctypes.c_float, pp(ctypes.c_uint16)
]


# asdf/file_io.h
libfab.asdf_write.argtypes = [p(ASDF), p(ctypes.c_char)]

libfab.asdf_read.argtypes = [p(ctypes.c_char)]
libfab.asdf_read.restype  =  p(ASDF)


# asdf/triangulate.h
from koko.c.mesh import Mesh
libfab.triangulate.argtypes = [
    p(ASDF), p(ctypes.c_int)
]
libfab.triangulate.restype = p(Mesh)

# asdf/cms.c
libfab.triangulate_cms.argtypes = [p(ASDF)]
libfab.triangulate_cms.restype = p(Mesh)

# asdf/contour.h
from koko.c.path import Path

libfab.contour.argtypes = [
    p(ASDF), p(pp(Path)), p(ctypes.c_int)
]
libfab.contour.restype  = ctypes.c_int


# asdf/distance.c
libfab.asdf_offset.argtypes = [
    p(ASDF), ctypes.c_float, ctypes.c_float
]
libfab.asdf_offset.restype  =  p(ASDF)

################################################################################

# cam/slices.h
libfab.find_support.argtypes = [
    ctypes.c_int, ctypes.c_int, pp(ctypes.c_uint8), pp(ctypes.c_uint8)
]
libfab.colorize_slice.argtypes = [
    ctypes.c_int, ctypes.c_int, pp(ctypes.c_uint8), pp(ctypes.c_uint8*3)
]
libfab.next_slice.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_float, ctypes.c_float,
    pp(ctypes.c_uint8), pp(ctypes.c_uint8)
]


# cam/distance.h
libfab.distance_transform1.argtypes = (
    [ctypes.c_int]*4 +
    [pp(ctypes.c_uint8), pp(ctypes.c_uint32)]
)

libfab.distance_transform2.argtypes = (
    [ctypes.c_int]*3 +
    [ctypes.c_float, pp(ctypes.c_uint32), pp(ctypes.c_float)]
)

libfab.distance_transform.argtypes = [
    ctypes.c_int, ctypes.c_int, ctypes.c_float,
    pp(ctypes.c_uint8), pp(ctypes.c_float)
]

# formats/png.c
libfab.save_png16L.argtypes = [p(ctypes.c_char), ctypes.c_int,
                                ctypes.c_int, ctypes.c_float*6,
                                pp(ctypes.c_uint16)]

libfab.count_by_color.argtypes = [p(ctypes.c_char), ctypes.c_int,
                                   ctypes.c_int, ctypes.c_uint32,
                                   p(ctypes.c_uint32)]

libfab.depth_blit.argtypes = (
    [pp(ctypes.c_uint8), pp(ctypes.c_uint8), pp(ctypes.c_uint8*3)] +
    [ctypes.c_int]*4 + [ctypes.c_float]*3
)


libfab.load_png_stats.argtypes = (
    [p(ctypes.c_char)] + [p(ctypes.c_int)]*2 + [p(ctypes.c_float)]*3
)

libfab.load_png.argtypes = [p(ctypes.c_char), pp(ctypes.c_uint16)]

# formats/mesh.h
libfab.free_mesh.argtypes = [p(Mesh)]

libfab.increase_indices.argtypes = [p(Mesh), ctypes.c_uint32]

libfab.save_mesh.argtypes = [p(ctypes.c_char), p(Mesh)]

libfab.load_mesh.argtypes = [p(ctypes.c_char)]
libfab.load_mesh.restype = p(Mesh)

libfab.merge_meshes.argtypes = [ctypes.c_uint32, pp(Mesh)]
libfab.merge_meshes.restype = p(Mesh)

# formats/stl.c
libfab.save_stl.argtypes = [p(Mesh), p(ctypes.c_char)]

libfab.load_stl.argtypes = [p(ctypes.c_char)]
libfab.load_stl.restype = p(Mesh)


# cam/toolpath.c
from koko.c.path import Path

libfab.find_paths.argtypes = [
    ctypes.c_int, ctypes.c_int, pp(ctypes.c_float),
    ctypes.c_float, ctypes.c_int, p(ctypes.c_float), p(pp(Path))
]
libfab.find_paths.restype = ctypes.c_int

libfab.free_paths.argtypes = [pp(Path), ctypes.c_int]

libfab.sort_paths.argtypes = [pp(Path), ctypes.c_int, p(ctypes.c_int)]

libfab.finish_cut.argtypes = (
    [ctypes.c_int]*2+[pp(ctypes.c_uint16)]+[ctypes.c_float]*4+
    [ctypes.c_int, p(pp(Path))]
)

del p, pp
