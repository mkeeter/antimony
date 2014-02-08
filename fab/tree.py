import ctypes
import os
import math
import threading

import numpy as np

class MathTree(object):
    def __init__(self, expr, ptr):
        self.ptr  = ptr
        self.expr = expr

    def __del__(self):
        if libfab:
            libfab.free_tree(self.ptr)

    @classmethod
    def from_expression(cls, expr):
        ptr = libfab.parse(expr.math)
        if ptr.value is None:
            raise RuntimeError("Math expression parsing failed.")
        return cls(expr, ptr)

    def __str__(self):
        if not hasattr(self, '_str'):
            self._str = self.make_str()
        return self._str

    def clone(self):
        return self.__class__(self.expr, libfab.clone_tree(self.ptr))

    def make_str(self):
        """ @brief Converts the object into an infix-notation string

            @details
            Creates a OS pipe, instructs the object to print itself into the
            pipe, and reads the output in chunks of maximum size 1024.
        """

        # Create a pipe to get the printout
        read, write = os.pipe()

        # Start the print function running in a separate thread
        # (so that we can eat the output and avoid filling the pipe)
        t = threading.Thread(target=libfab.fdprint_tree,
                             args=(self.ptr, write))
        t.daemon = True
        t.start()

        s = r = os.read(read, 1024)
        while r:
            r = os.read(read, 1024)
            s += r
        t.join()

        os.close(read)

        return s


    def render(self, region_or_resolution):
        """ Renders a math tree, returning an Image.
        """

        # If we're given a region, set it as our render region.
        # Otherwise, get a render region from our expression bounds
        # (raising an exception if that isn't possible)
        if isinstance(region_or_resolution, Region):
            region = region_or_resolution
        else:
            if self.expr.has_xy_bounds():
                region = self.expr.get_xy_region(region_or_resolution)
            else:
                raise Exception('Unknown render region!')

        image = Image.from_region(region)
        image.color = self.expr.color
        halt = ctypes.c_int(0)

        regions = region.split_xy(8)
        pixels = image.pixels()
        clones = [self.clone() for r in regions]

        threads = [threading.Thread(
            target=libfab.render16,
            args=(c.ptr, r, pixels, halt)) for c, r in zip(clones, regions)]

        for t in threads:
            t.daemon = True
            t.start()
        for t in threads:
            t.join()

        return image


    def triangulate(self, resolution, filename, abort=threading.Event()):
        """ Renders a tree into a mesh at the given resolution,
            saving as a .stl file.
        """
        if self.expr.has_xyz_bounds():
            region = self.expr.get_xyz_region(resolution)
        else:
            raise Exception("Unknown render region!")

        verts = ctypes.POINTER(ctypes.c_float)()
        count = ctypes.c_uint(0)
        libfab.triangulate(self.ptr, region,
                           ctypes.byref(verts), ctypes.byref(count))
        v = np.array([verts[i] for i in range(count.value)])
        libfab.free_mesh(verts)

        if not abort.is_set():  write_stl(v, filename)


    def eval_i(self, X, Y, Z):
        """ Evaluates this tree on the given interval.
            (used in remapping bounds).
        """
        return libfab.eval_i(self.ptr, X, Y, Z)


from expression import Expression
from libfab import libfab
from region import Region
from image import Image
from mesh import write_stl
