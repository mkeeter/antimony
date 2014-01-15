import ctypes

import numpy as np
import scipy.ndimage.interpolation

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
        """ Constructs an image and initializes bounds from a Region object.
        """
        i = cls(region.ni, region.nj)

        i.xmin = region.X[0] * mm_per_unit
        i.xmax = region.X[region.ni] * mm_per_unit
        i.ymin = region.Y[0] * mm_per_unit
        i.ymax = region.Y[region.nj] * mm_per_unit
        i.zmin = region.Z[0] * mm_per_unit
        i.zmax = region.Z[region.nk] * mm_per_unit

        return i

    def to_QImage(self):
        """ Converts this image into a QImage.
        """
        from PySide import QtGui

        # Translate to 8-bit greyscale
        scaled = np.array(self.array[::-1,:] >> 8, dtype=np.uint8)

        # Then make into an RGB image
        rgb = np.dstack([
            scaled, scaled, scaled,
            np.ones(scaled.shape, dtype=np.uint8)*255])

        # Finally, convert into a QImage
        pixels = rgb.flatten()
        qimage = QtGui.QImage(
                pixels, scaled.shape[1], scaled.shape[0],
                QtGui.QImage.Format_ARGB32)

        # Put this here to keep it a reference alive
        qimage.pixels = pixels

        return qimage


    def copy(self):
        i = Image(self.width, self.height)
        np.copyto(i.array, self.array)
        for v in ['xmin','ymin','zmin','xmax','ymax','zmax']:
            setattr(i, v, getattr(self, v))
        return i

    @staticmethod
    def blit_onto(source, target):
        """ Blits a source image onto a target image, scaling and properly
            overlapping images of different z heights.
        """
        # Don't modify the original image
        source = source.copy()
        source.array = source.array[::-1, :] # :(
        source.array = scipy.ndimage.interpolation.zoom(
                source.array,
                (target.width / (target.xmax - target.xmin)) /
                    (source.width / (source.xmax - source.xmin)),
                output=np.uint16)

        if target.xmin > source.xmin:
            imin = int(source.width * (target.xmin - source.xmin) /
                                      (source.xmax - source.xmin))
            source.xmin = target.xmin
            source.array = source.array[:,imin:]

        if target.xmax < source.xmax:
            imax = int(source.width * (target.xmax - source.xmax) /
                                      (source.xmax - source.xmin))
            source.xmax = target.xmax
            source.array = source.array[:,:imax]

        if target.ymin > source.ymin:
            jmin = int(source.width * (target.ymin - source.ymin) /
                                      (source.ymax - source.ymin))
            source.ymin = target.ymin
            source.array = source.array[jmin:,:]

        if target.ymax < source.ymax:
            jmax = int(source.width * (target.ymax - source.ymax) /
                                      (source.ymax - source.ymin))
            source.ymax = target.ymax
            source.array = source.array[:jmax,:]

        source.height, source.width = source.array.shape
        source.array = (source.array * ((source.zmax - target.zmin) /
                                        (target.zmax - target.zmin))).astype(
                                               np.uint16)

        imin = int(target.width * (source.xmin - target.xmin) /
                                  (target.xmax - target.xmin))

        jmin = int(target.height * (source.ymin - target.ymin) /
                                   (target.ymax - target.ymin))

        subtarget = target.array[jmin : jmin+source.height,
                                 imin : imin+source.width]
        # Clip to fix pixel precision errors on the edges
        if subtarget.shape != source.array.shape:
            source.array = source.array[:subtarget.shape[0],
                                        :subtarget.shape[1]]

        np.copyto(subtarget, source.array, where=source.array > subtarget)

        return target



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
        """ Saves this image as a 16-bit greyscale png with appropriate metadata.
        """
        bounds = (ctypes.c_float*6)(
            self.xmin, self.ymin, self.zmin,
            self.xmax, self.ymax, self.zmax
        )
        libfab.save_png16L(
                filename, self.width, self.height,
                bounds, self.pixels(flip_y=True))

from libfab import libfab


