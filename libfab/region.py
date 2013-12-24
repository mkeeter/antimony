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
                       scale=100., dummy=False, depth=None):
        """ @brief Creates an array.
        """

        dx = float(xmax - xmin)
        dy = float(ymax - ymin)
        dz = float(zmax - zmin)

        if depth is not None:
            scale = 3*(2**6)* 2**(depth/3.) / (dx+dy+dz)

        ni = max(int(round(dx*scale)), 1)
        nj = max(int(round(dy*scale)), 1)
        nk = max(int(round(dz*scale)), 1)

        # Dummy assignments so that Doxygen recognizes these instance variables
        self.ni = self.nj = self.nk = 0
        self.imin = self.jmin = self.kmin = 0
        self.voxels = 0
        self.X = self.Y = self.Z = self.L = None

        ## @var ni
        # Number of ticks along x axis
        ## @var nj
        #Number of points along y axis
        ## @var nk
        # Number of points along z axis

        ## @var imin
        # Minimum i coordinate in global lattice space
        ## @var jmin
        # Minimum j coordinate in global lattice space
        ## @var kmin
        # Minimum k coordinate in global lattice space

        ## @var voxels
        # Voxel count in this section of the lattice

        ## @var X
        # Array of ni+1 X coordinates as floating-point values
        ## @var Y
        # Array of nj+1 Y coordinates as floating-point values
        ## @var Z
        # Array of nk+1 Z coordinates as floating-point values
        ## @var L
        # Array of nk+1 luminosity values as 16-bit integers

        ## @var free_arrays
        # Boolean indicating whether this region dynamically allocated
        # the X, Y, Z, and L arrays.
        #
        # Determines whether these arrays are
        # freed when the structure is deleted.

        ctypes.Structure.__init__(self,
                                  0, 0, 0,
                                  ni, nj, nk,
                                  ni*nj*nk,
                                  None, None, None, None)

        if dummy is False:
            libfab.build_arrays(ctypes.byref(self),
                                 xmin, ymin, zmin,
                                 xmax, ymax, zmax)
            self.free_arrays = True
        else:
            self.free_arrays = False

    def __del__(self):
        """ @brief Destructor for Region
            @details Frees allocated arrays if free_arrays is True
        """
        if hasattr(self, 'free_arrays') and self.free_arrays and libfab is not None:
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

    def octsect(self, all=False, overlap=False):
        """ @brief Splits the region into eight subregions
            @param all If true, returns an 8-item array with None in the place of missing subregions.  Otherwise, the output array is culled to only include valid subregions.
            @returns An array of containing regions (and Nones if all is True and the region was indivisible along some axis)
        """
        L = (Region*8)()
        if overlap:
            bits = libfab.octsect_overlap(self, L)
        else:
            bits = libfab.octsect(self, L)

        if all:
            return [L[i] if (bits & (1 << i)) else None for i in range(8)]
        else:
            return [L[i] for i in range(8) if (bits & (1 << i))]


from koko.c.libfab import libfab
from koko.c.vec3f import Vec3f
