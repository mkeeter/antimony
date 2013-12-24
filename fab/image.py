import ctypes

import numpy as np

class Image(object):
    """ Wraps a 16-bit 1-channel numpy array.
    """

    def __init__(self, w, h):
        self.array = np.zeros((h, w), dtype=np.uint16 )

        self.width = w
        self.height = h

        # These values should be in mm (and will be set by the renderer)
        self.xmin = self.ymin = self.zmin = None
        self.xmax = self.ymax = self.zmax = None

    @classmethod
    def from_region(cls, region, mm_per_unit=1):
        i = cls(region.ni, region.nj)
        i.xmin = region.X[0] * mm_per_unit
        i.xmax = region.X[region.ni] * mm_per_unit
        i.ymin = region.Y[0] * mm_per_unit
        i.ymax = region.Y[region.nj] * mm_per_unit
        i.zmin = region.Z[0] * mm_per_unit
        i.zmax = region.Z[region.nk] * mm_per_unit

        return i


    def pixels(self, flip_y=False):
        """ Creates a ctypes pixel array that looks into the NumPy array.
            Returns an object of type uint16_t**.
        """
        if not self.array.flags['C_CONTIGUOUS']:
            self.array = np.ascontiguousarray(self.array)

        pixels = (ctypes.POINTER(ctypes.c_uint16) * self.height)()

        start = self.array.ctypes.data
        stride = self.array.ctypes.strides[0]

        for j in range(self.height):
            index = j if flip_y else self.height - j - 1
            pixels[index] = ctypes.cast(
                    start + j*stride, ctypes.POINTER(ctypes.c_uint16))

        return pixels


    def save(self, filename):
        """ Saves this image as a 16-bit greyscale png.
        """
        bounds = (ctypes.c_float*6)(
            self.xmin, self.ymin, self.zmin if self.zmin else float('nan'),
            self.xmax, self.ymax, self.zmax if self.zmax else float('nan')
        )
        libfab.save_png16L(
                filename, self.width, self.height,
                bounds, self.pixels(flip_y=True))

from libfab import libfab

