import ctypes

class Region(ctypes.Structure):
    ''' @class Region
        @brief Class containing lattice bounds and ticks.
        @details Replicates C Region class.
    '''
    _fields_ = [('imin', ctypes.c_uint32),
                ('jmin', ctypes.c_uint32),
                ('kmin', ctypes.c_uint32),
                ('ni', ctypes.c_uint32),
                ('nj', ctypes.c_uint32),
                ('nk', ctypes.c_uint32),
                ('voxels', ctypes.c_uint64),
                ('X', ctypes.POINTER(ctypes.c_float)),
                ('Y', ctypes.POINTER(ctypes.c_float)),
                ('Z', ctypes.POINTER(ctypes.c_float)),
                ('L', ctypes.POINTER(ctypes.c_uint16))]

    def __init__(self, (xmin, ymin, zmin)=(0.,0.,0.),
                       (xmax, ymax, zmax)=(0.,0.,0.),
                       pixels_per_unit=100.):
        """ Constructs a top-level region.
        """

        dx = float(xmax - xmin)
        dy = float(ymax - ymin)
        dz = float(zmax - zmin)

        ni = max(int(round(dx*pixels_per_unit)), 1)
        nj = max(int(round(dy*pixels_per_unit)), 1)
        nk = max(int(round(dz*pixels_per_unit)), 1)

        ctypes.Structure.__init__(
                self, 0, 0, 0, ni, nj, nk, ni*nj*nk,
                None, None, None, None)
        libfab.build_arrays(ctypes.byref(self),
                             xmin, ymin, zmin,
                             xmax, ymax, zmax)
        self.free_arrays = True


    def __del__(self):
        """ @brief Destructor for Region
            @details Frees allocated arrays if free_arrays is True
        """
        if hasattr(self, 'free_arrays') and libfab is not None:
            libfab.free_arrays(self)

    def __repr__(self):
        return ('[(%g, %g), (%g, %g), (%g, %g)]' %
            (self.imin, self.imin + self.ni,
             self.jmin, self.jmin + self.nj,
             self.kmin, self.kmin + self.nk))

    def split(self, count=2):
        """ @brief Repeatedly splits the region along its longest axis
            @param count Number of subregions to generate
            @returns List of regions (could be fewer than requested if the region is indivisible)
        """
        L = (Region*count)()
        count = libfab.split(self, L, count)

        return L[:count]

    def split_xy(self, count=2):
        """ @brief Repeatedly splits the region on the X and Y axes
            @param count Number of subregions to generate
            @returns List of regions (could be fewer than requested if the region is indivisible)
        """
        L = (Region*count)()
        count = libfab.split_xy(self, L, count)

        return L[:count]

    def octsect(self, all=False):
        """ @brief Splits the region into eight subregions
            @param all If true, returns an 8-item array with None in the place of missing subregions.  Otherwise, the output array is culled to only include valid subregions.
            @returns An array of containing regions (and Nones if all is True and the region was indivisible along some axis)
        """
        L = (Region*8)()
        bits = libfab.octsect(self, L)

        if all:
            return [L[i] if (bits & (1 << i)) else None for i in range(8)]
        else:
            return [L[i] for i in range(8) if (bits & (1 << i))]


from libfab import libfab
from vec3f import Vec3f
