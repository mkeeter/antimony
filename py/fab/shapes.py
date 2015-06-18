import math
import functools
import operator

from fab.types import Shape, Transform

def preserve_color(f):
    """ Function decorator that preserves the color of the first input
        (which is expected to be a fab.types.Shape)
    """
    def p(s, *args, **kwargs):
        color = s._r, s._g, s._b
        return set_color(f(s, *args, **kwargs), s._r, s._g, s._b)
    return p

def union(a, b):
    return a | b

def intersection(a, b):
    return a & b

def difference(a, b):
    return a & ~b

@preserve_color
def offset(a, o):
    """ Assumes a linear distance field for bounds calculations!
    """
    if o < 0:
        return Shape('-%sf%g' % (a.math, o),
                a.bounds.xmin, a.bounds.ymin, a.bounds.zmin,
                a.bounds.xmax, a.bounds.ymax, a.bounds.zmax)
    else:
        return Shape('-%sf%g' % (a.math, o),
                a.bounds.xmin - o, a.bounds.ymin - o, a.bounds.zmin - o,
                a.bounds.xmax + o, a.bounds.ymax + o, a.bounds.zmax + o)

def clearance(a, b, o):
    return b | (a & ~offset(b, o))

@preserve_color
def shell(a, o):
    return a & ~offset(a, -o)

@preserve_color
def buffer(a):
    return a

def set_color(a, r, g, b):
    """ Applies a given color to an input shape a and returns it.
    """
    q = Shape(a.math, a.bounds)
    q._r, q._g, q._b = r, g, b
    return q

@preserve_color
def invert(a):
    """ Inverts a shape within its existing bounds.
    """
    if a.bounds.is_bounded_xyz():
        return cube(a.bounds.xmin, a.bounds.xmax,
                    a.bounds.ymin, a.bounds.zmax,
                    a.bounds.zmin, a.bounds.zmax) & (~a)
    elif a.bounds.is_bounded_xy():
        return rectangle(a.bounds.xmin, a.bounds.xmax,
                         a.bounds.ymin, a.bounds.ymax) & (~a)
    else:
        return Shape()

################################################################################

def circle(x, y, r):
    """ Defines a circle from a center point and radius.
    """
    # sqrt((X-x)**2 + (Y-y)**2) - r
    r = abs(r)
    return Shape(
            '-r+q%sq%sf%g' % (('-Xf%g' % x) if x else 'X',
                              ('-Yf%g' % y) if y else 'Y', r),
            x - r, y - r, x + r, y + r)

def circle_edge(x0, y0, x1, y1):
    """ Defines a circle from two points on its radius.
    """
    xmid = (x0+x1)/2.0
    ymid = (y0+y1)/2.0
    r = math.sqrt((xmid-x0)**2 +(ymid-y0)**2)
    return circle(xmid, ymid, r)

def polygon_radius(x, y, r, N):
    """ Makes a polygon with a center-to-vertex distance r
        The polygon is oriented so that the bottom is always flat.
    """
    # Find the center-to-edge distance
    r_ = -r * math.cos(math.pi / N)
    # Make an offset half-region shape
    half = Shape('-f%gY' % r_)
    # Take the union of a bunch of rotated half-region shapes
    p = functools.reduce(operator.and_,
            [rotate(half, 360./N * i) for i in range(N)])
    # Apply appropriate bounds and return
    return Shape(p.math, -r, -r, r, r)

################################################################################

def triangle(x0, y0, x1, y1, x2, y2):
    """ Defines a triangle from three points.
    """
    # Find the angles of the points about the center
    xm = (x0 + x1 + x2) / 3.
    ym = (y0 + y1 + y2) / 3.
    angles = [math.atan2(y - ym, x - xm) for x, y in [(x0,y0), (x1,y1), (x2,y2)]]

    # Sort the angles so that the smallest one is first
    if angles[1] < angles[0] and angles[1] < angles[2]:
        angles = [angles[1], angles[2], angles[0]]
    elif angles[2] < angles[0] and angles[2] < angles[1]:
        angles = [angles[2], angles[0], angles[1]]

    # Enforce that points must be in clockwise order by swapping if necessary
    if angles[2] > angles[1]:
        x0, y0, x1, y1 = x1, y1, x0, y0

    def edge(x, y, dx, dy):
        # dy*(X-x)-dx*(Y-y)
        return '-*f%(dy)g-Xf%(x)g*f%(dx)g-Yf%(y)g' % locals()

    e0 = edge(x0, y0, x1-x0, y1-y0)
    e1 = edge(x1, y1, x2-x1, y2-y1)
    e2 = edge(x2, y2, x0-x2, y0-y2)

    # -min(e0, min(e1, e2))
    return Shape(
            'ni%(e0)si%(e1)s%(e2)s' % locals(),
            min(x0, x1, x2), min(y0, y1, y2),
            max(x0, x1, x2), max(y0, y1, y2))

def right_triangle(x, y, w, h):
   # max(max(x-X,y-Y),X-(x*(Y-y)+(x+w)*(y+h-Y))/h)
   ws = math.copysign(1,w)
   hs = math.copysign(1,h)
   return Shape(
      'aa*f%(ws)g-f%(x)gX*f%(hs)g-f%(y)gY*f%(ws)g-X/+*f%(x)g-Yf%(y)g*+f%(x)gf%(w)g-+f%(y)gf%(h)gYf%(h)g' % locals(),
       x, y, x + w, y + h)

################################################################################

def rectangle(xmin, xmax, ymin, ymax):
    # max(max(xmin - X, X - xmax), max(ymin - Y, Y - ymax)
    return Shape(
            'aa-f%(xmin)gX-Xf%(xmax)ga-f%(ymin)gY-Yf%(ymax)g' % locals(),
            xmin, ymin, xmax, ymax)

def rounded_rectangle(xmin, xmax, ymin, ymax, r):
    """ Returns a rectangle with rounded corners.
        r is a roundedness fraction between 0 (not rounded)
        and 1 (completely rounded)
    """
    r *= min(xmax - xmin, ymax - ymin)/2
    return (
        rectangle(xmin, xmax, ymin+r, ymax-r) |
        rectangle(xmin+r, xmax-r, ymin, ymax) |
        circle(xmin+r, ymin+r, r) |
        circle(xmin+r, ymax-r, r) |
        circle(xmax-r, ymin+r, r) |
        circle(xmax-r, ymax-r, r)
    )

################################################################################

def tab(x, y, width, height, angle=0, chamfer=0.2):
    tab = rectangle(-width/2, width/2, 0, height)
    cutout = triangle(width/2 - chamfer*height, height,
                      width/2, height,
                      width/2, height - chamfer*height)
    tab &= ~(cutout | reflect_x(cutout))

    return move(rotate(tab, angle), x, y)

################################################################################

def slot(x, y, width, height, angle=0, chamfer=0.2):
    slot = rectangle(-width/2, width/2, -height, 0)
    inset = triangle(width/2, 0,
                     width/2 + height * chamfer, 0,
                     width/2, -chamfer*height)
    slot |= inset | reflect_x(inset)

    return move(rotate(slot, angle), x, y)

################################################################################

@preserve_color
def move(part, dx, dy, dz=0):
    return part.map(Transform(
        '-Xf%g' % dx, '-Yf%g' % dy, '-Zf%g' % dz,
        '+Xf%g' % dx, '+Yf%g' % dy, '+Zf%g' % dz))

translate = move

@preserve_color
def origin_xy(a, x0, y0, x1, y1):
    return move(a, x1 - x0, y1 - y0)

@preserve_color
def origin_xyz(a, x0, y0, z0, x1, y1, z1):
    return move(a, x1 - x0, y1 - y0, z1 - z0)

################################################################################

@preserve_color
def recenter(part, x, y, z):
    if not math.isinf(part.bounds.xmax) and not math.isinf(part.bounds.xmin):
        dx = x - (part.bounds.xmax + part.bounds.xmin) / 2
    else:
        dx = 0
    if not math.isinf(part.bounds.ymax) and not math.isinf(part.bounds.ymin):
        dy = y - (part.bounds.ymax + part.bounds.ymin) / 2
    else:
        dy = 0
    if not math.isinf(part.bounds.zmax) and not math.isinf(part.bounds.zmin):
        dz = z - (part.bounds.zmax + part.bounds.zmin) / 2
    else:
        dz = 0
    return move(part, dx, dy, dz)

################################################################################

@preserve_color
def rotate(part, angle, x0=0, y0=0):
    p = move(part, -x0, -y0, 0)
    angle *= math.pi/180
    ca, sa = math.cos(angle), math.sin(angle)
    nca, nsa = math.cos(-angle), math.sin(-angle)

    return move(p.map(Transform(
        '+*f%(ca)gX*f%(sa)gY'    % locals(),
        '+*f%(nsa)gX*f%(ca)gY'   % locals(),
        '+*f%(nca)gX*f%(nsa)gY'  % locals(),
        '+*f%(sa)gX*f%(nca)gY'   % locals())),
        x0, y0, 0)

################################################################################

@preserve_color
def reflect_x(part, x0=0):
    # X' = 2*x0-X
    # X  = 2*x0-X'
    return part.map(Transform(
        '-*f2f%gX' % x0, '',
        '-*f2f%gX' % x0, ''))

@preserve_color
def reflect_y(part, y0=0):
    # Y' = 2*y0-Y
    return part.map(Transform(
        '', '-*f2f%gY' % y0,
        '', '-*f2f%gY' % y0))

@preserve_color
def reflect_z(part, z0=0):
    # Z' = 2*z0-Z
    return part.map(Transform(
        '', '', '-*f2f%gZ' % z0,
        '', '', '-*f2f%gZ' % z0))

@preserve_color
def reflect_xy(part, x0=0, y0=0):
   # X' = x0 + (Y-y0)
   # Y' = y0 + (X-x0)
   # X = x0 + (Y'-y0)
   # Y = y0 + (X'-x0)
   return part.map(Transform(
      '+f%(x0)g-Yf%(y0)g' % locals(),
      '+f%(y0)g-Xf%(x0)g' % locals(),
      '+f%(x0)g-Yf%(y0)g' % locals(),
      '+f%(y0)g-Xf%(x0)g' % locals()))

@preserve_color
def reflect_yz(part, y0=0, z0=0):
   # Y' = y0 + (Z-z0)
   # Z' = z0 + (Y-y0)
   # Y = y0 + (Z'-z0)
   # Z = z0 + (Y'-y0)
   return part.map(Transform(
      'X',
      '+f%(y0)g-Zf%(z0)g' % locals(),
      '+f%(z0)g-Yf%(y0)g' % locals(),
      'X',
      '+f%(y0)g-Zf%(z0)g' % locals(),
      '+f%(z0)g-Yf%(y0)g' % locals()))

@preserve_color
def reflect_xz(part, x0=0, z0=0):
   # X' = x0 + (Z-z0)
   # Z' = z0 + (X-x0)
   # X = x0 + (Z'-z0)
   # Z = z0 + (X'-x0)
   return part.map(Transform(
      '+f%(x0)g-Zf%(z0)g' % locals(),
      'Y',
      '+f%(z0)g-Xf%(x0)g' % locals(),
      '+f%(x0)g-Zf%(z0)g' % locals(),
      'Y',
      '+f%(z0)g-Xf%(x0)g' % locals()))

################################################################################

@preserve_color
def scale_x(part, x0, sx):
    # X' = x0 + (X-x0)/sx
    return part.map(Transform(
        '+f%(x0)g/-Xf%(x0)gf%(sx)g' % locals()
                if x0 else '/Xf%g' % sx,
        'Y',
        '+f%(x0)g*f%(sx)g-Xf%(x0)g' % locals()
                if x0 else '*Xf%g' % sx,
        'Y'))

@preserve_color
def scale_y(part, y0, sy):
    # Y' = y0 + (Y-y0)/sy
    return part.map(Transform(
        'X',
        '+f%(y0)g/-Yf%(y0)gf%(sy)g' % locals()
                if y0 else '/Yf%g' % sy,
        'X',
        '+f%(y0)g*f%(sy)g-Yf%(y0)g' % locals()
                if y0 else '*Yf%g' % sy))

@preserve_color
def scale_z(part, z0, sz):
    # Z' = z0 + (Y-y0)/sz
    # Z  = (Z'-z0)*sz + z0
    return part.map(Transform(
        'X', 'Y',
        '+f%(z0)g/-Zf%(z0)gf%(sz)g' % locals()
                if z0 else '/Zf%g' % sz,
        'X', 'Y',
        '+f%(z0)g*f%(sz)g-Zf%(z0)g' % locals()
                if z0 else '*Zf%g' % sz))

@preserve_color
def scale_xy(part, x0, y0, sx, sy=None):
    # X' = x0 + (X-x0)/sx
    # Y' = y0 + (Y-y0)/sy
    # X  = (X'-x0)*sx + x0
    # Y  = (Y'-y0)*sy + y0
    if sy is None:
        sy = sx
    return part.map(Transform(
        '+f%(x0)g/-Xf%(x0)gf%(sx)g' % locals(),
        '+f%(y0)g/-Yf%(y0)gf%(sy)g' % locals(),
        '+f%(x0)g*f%(sx)g-Xf%(x0)g' % locals(),
        '+f%(y0)g*f%(sy)g-Yf%(y0)g' % locals()))

@preserve_color
def scale_xyz(part, x0, y0, z0, sx, sy, sz):
   # X' = x0 + (X-x0)/sx
   # Y' = y0 + (Y-y0)/sy
   # Z' = z0 + (Z-z0)/sz
   # X = x0 + (X'-x0)*sx
   # Y = y0 + (Y'-y0)*sy
   # Z = z0 + (Z'-z0)*sz
   return part.map(Transform(
      '+f%(x0)g/-Xf%(x0)gf%(sx)g' % locals(),
      '+f%(y0)g/-Yf%(y0)gf%(sy)g' % locals(),
      '+f%(z0)g/-Zf%(z0)gf%(sz)g' % locals(),
      '+f%(x0)g*-Xf%(x0)gf%(sx)g' % locals(),
      '+f%(y0)g*-Yf%(y0)gf%(sy)g' % locals(),
      '+f%(z0)g*-Zf%(z0)gf%(sz)g' % locals()))

@preserve_color
def scale_cos_xy_z(part, x0, y0, z0, z1, amp, off, t0, t1):
   dz = z1-z0
   t0 = math.radians(t0)
   t1 = math.radians(t1)
   # X' = x0 + (X-x0)/(off+amp*math.cos(theta0+(theta1-theta0)*(Z-z0)/dz))
   # X = x0 + (X'-x0)*(off+amp*math.cos(theta0+(theta1-theta0)*(Z-z0)/dz))
   # Y' = y0 + (Y-y0)/(off+amp*math.cos(theta0+(theta1-theta0)*(Z-z0)/dz))
   # Y = y0 + (Y'-y0)*(off+amp*math.cos(theta0+(theta1-theta0)*(Z-z0)/dz))
   return part.map(Transform(
      '/+f%(x0)g-Xf%(x0)g+f%(off)g*f%(amp)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Zf%(z0)gf%(dz)g' % locals(),
      '/+f%(y0)g-Yf%(y0)g+f%(off)g*f%(amp)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Zf%(z0)gf%(dz)g' % locals(),
      'Z',
      '*+f%(x0)g-Xf%(x0)g+f%(off)g*f%(amp)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Zf%(z0)gf%(dz)g' % locals(),
      '*+f%(y0)g-Yf%(y0)g+f%(off)g*f%(amp)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Zf%(z0)gf%(dz)g' % locals(),
      'Z'))

@preserve_color
def scale_cos_x_y(part, x0, y0, y1, amp, off, t0, t1):
   dy = y1 - y0
   t0 = math.radians(t0)
   t1 = math.radians(t1)
   # X' = x0 + (X-x0)/(off+amp*math.cos(theta0+(theta1-theta0)*(Y-y0)/dy))
   # X = x0 + (X'-x0)*(off+amp*math.cos(theta0+(theta1-theta0)*(Y-y0)/dy))
   return part.map(Transform(
      '/+f%(x0)g-Xf%(x0)g+f%(off)g*f%(amp)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Yf%(y0)gf%(dy)g' % locals(),
      'Y',
      '*+f%(x0)g-Xf%(x0)g+f%(off)g*f%(amp)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Yf%(y0)gf%(dy)g' % locals(),
      'Y'))

def scale_z_r(part, x0, y0, z0, r0, s0, r1, s1):
   dr = r1 - r0
   # Z' = z0 + (Z-z0)*dr/((s1-s0)*sqrt((X-x0)^2+(Y-y0)^2)-s1*r0+s0*r1)
   # Z = z0 + (Z'-z0)*((s1-s0)*sqrt((X-x0)^2+(Y-y0)^2)-s1*r0+s0*r1)/dr
   return part.map(Transform(
      'X', 'Y',
      '+f%(z0)g/*-Zf%(z0)gf%(dr)g+-*-f%(s1)gf%(s0)gr+q-Xf%(x0)gq-Yf%(y0)g*f%(s1)gf%(r0)g*f%(s0)gf%(r1)g' % locals(),
      'X', 'Y',
      '+f%(z0)g/*-Zf%(z0)g+-*-f%(s1)gf%(s0)gr+q-Xf%(x0)gq-Yf%(y0)g*f%(s1)gf%(r0)g*f%(s0)gf%(r1)gf%(dr)g' % locals()))

################################################################################

@preserve_color
def extrude_z(part, zmin, zmax):
    # max(part, max(zmin-Z, Z-zmax))
    return Shape(
            'am__f1%sa-f%gZ-Zf%g' % (part.math, zmin, zmax),
            part.bounds.xmin, part.bounds.ymin, zmin,
            part.bounds.xmax, part.bounds.ymax, zmax)

################################################################################

def loft_xy_z(a, b, zmin, zmax):
    """ Creates a blended loft between two shapes.

        Input shapes should be 2D (in the XY plane).
        The resulting loft will be shape a at zmin and b at zmax.
    """
    # ((z-zmin)/(zmax-zmin))*b + ((zmax-z)/(zmax-zmin))*a
    # In the prefix string below, we add caps at zmin and zmax then
    # factor out the division by (zmax - zmin)
    dz = zmax - zmin
    a_, b_ = a.math, b.math
    return Shape(('aa-Zf%(zmax)g-f%(zmin)g' +
                  'Z/+*-Zf%(zmin)g%(b_)s' +
                     '*-f%(zmax)gZ%(a_)sf%(dz)g') % locals(),
                min(a.bounds.xmin, b.bounds.xmin),
                min(a.bounds.ymin, b.bounds.ymin), zmin,
                max(a.bounds.xmax, b.bounds.xmax),
                max(a.bounds.ymax, b.bounds.ymax), zmax)

################################################################################

@preserve_color
def shear_x_y(part, ymin, ymax, dx0, dx1):
    dx = dx1 - dx0
    dy = ymax - ymin

    # X' = X-dx0-dx*(Y-ymin)/dy
    # X  = X'+dx0+(dx)*(Y-ymin)/dy
    return part.map(Transform(
            '--Xf%(dx0)g/*f%(dx)g-Yf%(ymin)gf%(dy)g' % locals(),
            'Y',
            '++Xf%(dx0)g/*f%(dx)g-Yf%(ymin)gf%(dy)g' % locals(),
            'Y'))

@preserve_color
def shear_xy_z(part, zmin, zmax, dx0, dy0, dx1, dy1):
    dx = dx1 - dx0
    dy = dy1 - dy0
    dz = zmax - zmin

    # X' = X-dx0-dx*(Z-zmin)/dz
    # Y' = Y-dy0-dy*(Z-zmin)/dz
    # X  = X'+dx0+(dx)*(Y-ymin)/dy
    # Y  = Y'+dy0+(dy)*(Y-ymin)/dy
    return part.map(Transform(
            '--Xf%(dx0)g/*f%(dx)g-Zf%(zmin)gf%(dz)g' % locals(),
            '--Yf%(dy0)g/*f%(dy)g-Zf%(zmin)gf%(dz)g' % locals(),
            '++Xf%(dx0)g/*f%(dx)g-Zf%(zmin)gf%(dz)g' % locals(),
            '++Yf%(dy0)g/*f%(dy)g-Zf%(zmin)gf%(dz)g' % locals()))

@preserve_color
def shear_cos_xy_z(part, z0, z1, ampx, offx, ampy, offy, t0, t1):
   dz = z1-z0
   t0 = math.radians(t0)
   t1 = math.radians(t1)
   # X' = X-(offx+ampx*math.cos(theta0+(theta1-theta0)*(Z-z0)/dz))
   # X = X'+(offx+ampx*math.cos(theta0+(theta1-theta0)*(Z-z0)/dz))
   # Y' = Y-(offy+ampy*math.cos(theta0+(theta1-theta0)*(Z-z0)/dz))
   # Y = Y'+(offy+ampy*math.cos(theta0+(theta1-theta0)*(Z-z0)/dz))
   return part.map(Transform(
      '-X+f%(offx)g*f%(ampx)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Zf%(z0)gf%(dz)g' % locals(),
      '-Y+f%(offy)g*f%(ampy)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Zf%(z0)gf%(dz)g' % locals(),
      'Z',
      '+X+f%(offx)g*f%(ampx)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Zf%(z0)gf%(dz)g' % locals(),
      '+Y+f%(offy)g*f%(ampy)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Zf%(z0)gf%(dz)g' % locals(),
      'Z'))

@preserve_color
def shear_cos_x_y(part,y0,y1,amp,off,t0,t1):
   dy = y1-y0
   t0 = math.radians(t0)
   t1 = math.radians(t1)
   # X' = X-(off+amp*math.cos(theta0+(theta1-theta0)*(Y-y0)/dy))
   # X = X'+(off+amp*math.cos(theta0+(theta1-theta0)*(Y-y0)/dy))
   return part.map(Transform(
      '-X+f%(off)g*f%(amp)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Yf%(y0)gf%(dy)g' % locals(),
      'Y',
      '+X+f%(off)g*f%(amp)gc+f%(t0)g/*-f%(t1)gf%(t0)g-Yf%(y0)gf%(dy)g' % locals(),
      'Y'))
################################################################################

@preserve_color
def taper_x_y(part, x0, y0, y1, s0, s1):
    dy = y1 - y0
    ds = s1 - s0
    s0y1 = s0 * y1
    s1y0 = s1 * y0

    #   X'=x0+(X-x0)*(y1-y0)/(Y*(s1-s0)+s0*y1-s1*y0))
    #   X=(X'-x0)*(Y*(s1-s0)+s0*y1-s1*y0)/(y1-y0)+x0
    return part.map(Transform(
        '+f%(x0)g/*-Xf%(x0)gf%(dy)g-+*Yf%(ds)gf%(s0y1)gf%(s1y0)g' % locals(),
        'Y',
        '+f%(x0)g*-Xf%(x0)g/-+*Yf%(ds)gf%(s0y1)gf%(s1y0)gf%(dy)g' % locals(),
        'Y'))


@preserve_color
def iterate2d(part, i, j, dx, dy):
    """ Tiles a part in the X and Y directions.
    """
    if i < 1 or j < 1:
        raise ValueError("Invalid value for iteration")

    return functools.reduce(operator.or_,
            [move(functools.reduce(operator.or_,
                    [move(part, a*dx, 0, 0) for a in range(i)]), 0, b*dy, 0)
                for b in range(j)])


@preserve_color
def iterate_polar(part, x, y, n):
    """ Tiles a part by rotating it n times about x,y
    """

    if n < 1:
        raise ValueError("Invalid count for iteration")

    return functools.reduce(operator.or_,
            [rotate(part, 360./n * i, x, y)
             for i in range(n)])

################################################################################

def blend(a, b, amount):
    joint = a | b

    # sqrt(abs(a)) + sqrt(abs(b)) - amount
    fillet = Shape('-+rb%srb%sf%g' % (a.math, b.math, amount),
                   joint.bounds)
    return joint | fillet

def morph(a, b, weight):
    """ Morphs between two shapes.
    """
    # shape = weight*a+(1-weight)*b
    s = "+*f%g%s*f%g%s" % (weight, a.math, 1-weight, b.math)
    return Shape(s, (a | b).bounds)

################################################################################

def cylinder(x, y, zmin, zmax, r):
    return extrude_z(circle(x, y, r), zmin, zmax)

def cylinder_x(xmin, xmax, y, z,r):
   from fab.types import Shape, Transform
   # max(sqrt((Y-y)^2+(Z-z)^2)-r,max(xmin-X,X-xmax))
   return Shape(
      'a-r+q-Yf%(y)gq-Zf%(z)gf%(r)ga-f%(xmin)gX-Xf%(xmax)g' % locals(),
      xmin, y-r, z-r, xmax, y+r, z+r)

def cylinder_y(x, ymin, ymax, z, r):
   from fab.types import Shape, Transform
   # max(sqrt((X-x)^2+(Z-z)^2)-r,max(ymin-Y,Y-ymax))
   return Shape(
      'a-r+q-Xf%(x)gq-Zf%(z)gf%(r)ga-f%(ymin)gY-Yf%(ymax)g' % locals(),
      x-r, ymin, z-r, x+r, ymax,z+r)

def sphere(x, y, z, r):
    return Shape('-r++q%sq%sq%sf%g' % (('-Xf%g' % x) if x else 'X',
                                  ('-Yf%g' % y) if y else 'Y',
                                  ('-Zf%g' % z) if z else 'Z',
                                  r),
            x - r, y - r, z - r, x + r, y + r, z + r)

def cube(xmin, xmax, ymin, ymax, zmin, zmax):
    return extrude_z(rectangle(xmin, xmax, ymin, ymax), zmin, zmax)

def rounded_cube(xmin, xmax, ymin, ymax, zmin, zmax, r):
    """ Returns a cube with rounded corners and edges
        r is a roundedness fraction between 0 (not rounded)
        and 1 (completely rounded)
    """
    r *= min([xmax - xmin, ymax - ymin, zmax - zmin])/2
    s = (
        extrude_z(rectangle(xmin + r, xmax - r, ymin + r, ymax - r),
                  zmin, zmax) |
        extrude_z(rectangle(xmin, xmax, ymin + r, ymax - r) |
                  rectangle(xmin + r, xmax - r, ymin, ymax),
                  zmin + r, zmax - r)
    )
    for i in range(8):
        s |= sphere((xmin + r) if (i & 1) else (xmax - r),
                    (ymin + r) if (i & 2) else (ymax - r),
                    (zmin + r) if (i & 4) else (zmax - r), r)
    for i in range(4):
        s |= cylinder(
                (xmin + r) if (i & 1) else (xmax - r),
                (ymin + r) if (i & 2) else (ymax - r),
                zmin + r, zmax - r, r)
        s |= cylinder_x(
                xmin + r, xmax - r,
                (ymin + r) if (i & 1) else (ymax - r),
                (zmin + r) if (i & 2) else (zmax - r), r)
        s |= cylinder_y(
                (xmin + r) if (i & 1) else (xmax - r),
                ymin + r, ymax - r,
                (zmin + r) if (i & 2) else (zmax - r), r)
    return s

def cone(x, y, z0, z1, r):
    flipped = z1 < z0
    if flipped:
        z1 = 2*z0 - z1
    cyl = cylinder(x, y, z0, z1, r)
    out = taper_xy_z(cyl, x, y, z0, z1, 1.0, 0.0)
    return reflect_z(out, z0) if flipped else out

def pyramid(xmin, xmax, ymin, ymax, z0, z1):
    flipped = z1 < z0
    if flipped:
        z1 = 2*z0 - z1
    c = cube(xmin, xmax, ymin, ymax, z0, z1)
    out = taper_xy_z(c, (xmin+xmax)/2, (ymin+ymax)/2, z0, z1, 1, 0)
    return reflect_z(out, z0) if flipped else out

def torus_x(x, y, z, R, r):
   # sqrt((R - sqrt((Y-y)^2+(Z-z)^2))^2 + (X-x)^2)-r
   return move(Shape(
      '-r+q-f%(R)gr+qYqZqXf%(r)g' % locals(),
       -r, -(R + r), -(R + r), r, R + r, R + r), x, y, z)

def torus_y(x, y, z, R, r):
   # sqrt((R - sqrt((X-x)^2+(Z-z)^2))^2 + (Y-y)^2)-r
   return move(Shape(
      '-r+q-f%(R)gr+qXqZqYf%(r)g' % locals(),
       -(R+r), -r, -(R+r), R + r, r, R + r), x, y, z)

def torus_z(x, y, z, R, r):
   return move(Shape(
      '-r+q-f%(R)gr+qXqYqZf%(r)g' % locals(),
       -(R+r), -(R+r), -r, R + r, R + r, r), x, y, z)
################################################################################

# 3D shapes and operations

@preserve_color
def rotate_x(part, angle, y0=0, z0=0):
    p = move(part, 0, -y0, -z0)
    angle *= math.pi/180
    ca, sa = math.cos(angle), math.sin(angle)
    nca, nsa = math.cos(-angle), math.sin(-angle)

    return move(p.map(Transform(
        '', '+*f%(ca)gY*f%(sa)gZ'  % locals(),
            '+*f%(nsa)gY*f%(ca)gZ' % locals(),

        'X', '+*f%(nca)gY*f%(nsa)gZ' % locals(),
             '+*f%(sa)gY*f%(nca)gZ' % locals())),
        0, y0, z0)

@preserve_color
def rotate_y(part, angle, x0=0, z0=0):

    p = move(part, -x0, 0, -z0)
    angle *= math.pi/180
    ca, sa = math.cos(angle), math.sin(angle)
    nca, nsa = math.cos(-angle), math.sin(-angle)

    return move(p.map(Transform(
            '+*f%(ca)gX*f%(sa)gZ'  % locals(), 'Y',
            '+*f%(nsa)gX*f%(ca)gZ' % locals(),
            '+*f%(nca)gX*f%(nsa)gZ' % locals(), 'Y',
            '+*f%(sa)gX*f%(nca)gZ' % locals())),
            x0, 0, z0)

rotate_z = rotate

################################################################################

@preserve_color
def shear_x_z(part, z0, z1, dx0, dx1):
    #   X' = X-dx0-(dx1-dx0)*(Z-z0)/(z1-z0)
    #   X = X'+dx0+(dx1-dx0)*(Z-z0)/(z1-z0)
    return part.map(Transform(
        '--Xf%(dx0)g/*f%(dx)g-Zf%(z0)gf%(dz)g' % locals(), '', '',
        '++Xf%(dx0))g/*f%(dx)g-Zf%(z0)gf%(dz)g' % locals(), '', ''))

################################################################################

@preserve_color
def taper_xy_z(part, x0, y0, z0, z1, s0, s1):

    dz = z1 - z0

    # X' =  x0 +(X-x0)*dz/(s1*(Z-z0) + s0*(z1-Z))
    # Y' =  y0 +(Y-y0)*dz/(s1*(Z-z0) + s0*(z1-Z))
    # X  = (X' - x0)*(s1*(Z-z0) + s0*(z1-Z))/dz + x0
    # Y  = (Y' - y0)*(s1*(Z-z0) + s0*(z1-Z))/dz + y0
    return part.map(Transform(
        '+f%(x0)g/*-Xf%(x0)gf%(dz)g+*f%(s1)g-Zf%(z0)g*f%(s0)g-f%(z1)gZ'
            % locals(),
        '+f%(y0)g/*-Yf%(y0)gf%(dz)g+*f%(s1)g-Zf%(z0)g*f%(s0)g-f%(z1)gZ'
            % locals(),
        '',
        '+/*-Xf%(x0)g+*f%(s1)g-Zf%(z0)g*f%(s0)g-f%(z1)gZf%(dz)gf%(x0)g'
            % locals(),
        '+/*-Yf%(y0)g+*f%(s1)g-Zf%(z0)g*f%(s0)g-f%(z1)gZf%(dz)gf%(y0)g'
            % locals(),
        ''))

################################################################################

@preserve_color
def revolve_y(a):
    ''' Revolve a part in the XY plane about the Y axis. '''
    #   X' = +/- sqrt(X**2 + Z**2)
    pos = a.map(Transform('r+qXqZ', '', '', '', '', ''))
    neg = a.map(Transform('nr+qXqZ', '', '', '', '', ''))
    m = max(abs(a.bounds.xmin), abs(a.bounds.xmax))
    return Shape((pos | neg).math, -m, a.bounds.ymin, -m,
                                    m, a.bounds.ymax,  m)


@preserve_color
def revolve_x(a):
    ''' Revolve a part in the XY plane about the X axis. '''
    #   Y' = +/- sqrt(Y**2 + Z**2)
    pos = a.map(Transform('', 'r+qYqZ', '', '', '', ''))
    neg = a.map(Transform('', 'nr+qYqZ', '', '', '', ''))
    m = max(abs(a.bounds.ymin), abs(a.bounds.ymax))
    return Shape((pos | neg).math, a.bounds.xmin, -m, -m,
                                   a.bounds.xmax,  m,  m)

@preserve_color
def revolve_xy_x(a, y):
    """ Revolves the given shape about the x-axis
        (offset by the given y value)
    """
    return move(revolve_x(move(a, 0, -y)), 0, y)

@preserve_color
def revolve_xy_y(a, x):
    """ Revolves the given shape about the y-axis
        (offset by the given x value)
    """
    return move(revolve_y(move(a, -x, 0)), x, 0)

################################################################################

@preserve_color
def attract(part, x, y, z, r):

    # Shift the part so that it is centered
    part = move(part, -x, -y, -z)

    # exponential fallout value
    # x*(1 - exp(-sqrt(x**2 + y**2 + z**2) / r))
    d = '+f1xn/r++qXqYqZf%g' % r
    p = part.map(Transform(
        '*X'+d, '*Y'+d, '*Z'+d, '', '', ''))

    b = r/math.e
    return move(Shape(
        p.math,
        part.bounds.xmin - b, part.bounds.ymin - b, part.bounds.zmin - b,
        part.bounds.xmax + b, part.bounds.ymax + b, part.bounds.zmax + b),
        x, y, z)

@preserve_color
def repel(part, x, y, z, r):
    # Shift the part so that it is centered
    part = move(part, -x, -y, -z)

    # exponential fallout value
    # x*(1 - exp(-sqrt(x**2 + y**2 + z**2) / r))
    d = '-f1xn/r++qXqYqZf%g' % r
    p = part.map(Transform('*X'+d, '*Y'+d, '*Z'+d, '', '', ''))

    b = r/math.e
    return move(Shape(
        p.math,
        part.bounds.xmin - b, part.bounds.ymin - b, part.bounds.zmin - b,
        part.bounds.xmax + b, part.bounds.ymax + b, part.bounds.zmax + b),
        x, y, z)

################################################################################

@preserve_color
def twist_xy_z(part, x, y, z0, z1, t0, t1):
    # First, we'll move and scale so that the relevant part of the model
    # is at x=y=0 and scaled so that z is between 0 and 1.
    p1 = scale_z(move(part, -x, -y, -z0), 0, 1.0/(z1 - z0))

    t0 = math.pi * t0 / 180.0
    t1 = math.pi * t1 / 180.0

    # X' =  X*cos(t1*z + t0*(1-z)) + Y*sin(t1*z + t0*(1-z))
    # Y' = -X*sin(t1*z + t0*(1-z)) + Y*cos(t1*z + t0*(1-z))
    # X =  X*cos(t1*z + t0*(1-z)) - Y*sin(t1*z + t0*(1-z))
    # Y =  X*sin(t1*z + t0*(1-z)) + Y*cos(t1*z + t0*(1-z))
    p2 = p1.map(Transform(
        '+*Xc+*f%(t1)gZ*f%(t0)g-f1Z*Ys+*f%(t1)gZ*f%(t0)g-f1Z' % locals(),
        '+n*Xs+*f%(t1)gZ*f%(t0)g-f1Z*Yc+*f%(t1)gZ*f%(t0)g-f1Z' % locals(),
        '-*Xc+*f%(t1)gZ*f%(t0)g-f1Z*Ys+*f%(t1)gZ*f%(t0)g-f1Z' % locals(),
        '+*Xs+*f%(t1)gZ*f%(t0)g-f1Z*Yc+*f%(t1)gZ*f%(t0)g-f1Z' % locals()))

    return move(scale_z(p2, 0, z1 - z0), x, y, z0)

################################################################################

def function_prefix_xy(fn, xmin, xmax, ymin, ymax):
    """ Takes an arbitrary prefix math-string and makes it a function.
        Returns the function intersected with the given bounding rectangle.
    """
    return Shape(fn) & rectangle(xmin, xmax, ymin, ymax)

def function_prefix_xyz(fn, xmin, xmax, ymin, ymax, zmin, zmax):
    """ Takes an arbitrary prefix math-string and makes it a function.
        Returns the function intersected with the given bounding cube.
    """
    return Shape(fn) & cube(xmin, xmax, ymin, ymax, zmin, zmax)

################################################################################

def text(text, x, y, height=1, align='LB'):
    if text == '':
        return Shape()
    dx, dy = 0, -1
    text_shape = None

    for line in text.split('\n'):
        line_shape = None

        for c in line:
            if not c in _glyphs.keys():
                print('Warning:  Unknown character "%s"' % c)
            else:
                chr_math = move(_glyphs[c], dx, dy)
                if line_shape is None:  line_shape  = chr_math
                else:                   line_shape |= chr_math
                dx += _widths[c] + 0.1
        dx -= 0.1

        if line_shape is not None:
            if align[0] == 'L':
                pass
            elif align[0] == 'C':
                line_shape = move(line_shape, -dx / 2, 0)
            elif align[0] == 'R':
                line_shape = move(line_shape, -dx, 0)

            if text_shape is None:  text_shape  = line_shape
            else:                   text_shape |= line_shape

        dy -= 1.55
        dx = 0
    dy += 1.55
    if text_shape is None:  return None

    if align[1] == 'T':
        pass
    elif align[1] == 'B':
        text_shape = move(text_shape, 0, -dy,)
    elif align[1] == 'C':
        text_shape = move(text_shape, 0, -dy/2)

    if height != 1:
        text_shape = scale_xy(text_shape, 0, 0, height)

    return move(text_shape, x, y)


_glyphs = {}
_widths = {}

shape = triangle(0, 0, 0.35, 1, 0.1, 0)
shape |= triangle(0.1, 0, 0.35, 1, 0.45, 1)
shape |= triangle(0.35, 1, 0.45, 1, 0.8, 0)
shape |= triangle(0.7, 0, 0.35, 1, 0.8, 0)
shape |= rectangle(0.2, 0.6, 0.3, 0.4)
_widths['A'] = 0.8
_glyphs['A'] = shape


shape = circle(0.25, 0.275, 0.275)
shape &= ~circle(0.25, 0.275, 0.175)
shape = shear_x_y(shape, 0, 0.35, 0, 0.1)
shape |= rectangle(0.51, 0.61, 0, 0.35)
shape = move(shape, -0.05, 0)
_widths['a'] = 0.58
_glyphs['a'] = shape


shape = circle(0.3, 0.725, 0.275)
shape &= ~circle(0.3, 0.725, 0.175)
shape |= circle(0.3, 0.275, 0.275)
shape &= ~circle(0.3, 0.275, 0.175)
shape &= rectangle(0.3, 1, 0, 1)
shape |= rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.1, 0.3, 0, 0.1)
shape |= rectangle(0.1, 0.3, 0.45, 0.55)
shape |= rectangle(0.1, 0.3, 0.9, 1)
_widths['B'] = 0.575
_glyphs['B'] = shape


shape = circle(0.25, 0.275, 0.275)
shape &= ~circle(0.25, 0.275, 0.175)
shape &= rectangle(0.25, 1, 0, 0.275) | rectangle(0, 1, 0.275, 1)
shape |= rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.1, 0.25, 0, 0.1)
_widths['b'] = 0.525
_glyphs['b'] = shape


shape = circle(0.3, 0.7, 0.3) & ~circle(0.3, 0.7, 0.2)
shape |= circle(0.3, 0.3, 0.3) & ~circle(0.3, 0.3, 0.2)
shape &= ~rectangle(0, 0.6, 0.3, 0.7)
shape &= ~triangle(0.3, 0.5, 1, 1.5, 1, -0.5)
shape &= ~rectangle(0.3, 0.6, 0.2, 0.8)
shape |= rectangle(0, 0.1, 0.3, 0.7)
_widths['C'] = 0.57
_glyphs['C'] = shape


shape = circle(0.275, 0.275, 0.275)
shape &= ~circle(0.275, 0.275, 0.175)
shape &= ~triangle(0.275, 0.275, 0.55, 0.55, 0.55, 0)
_widths['c'] = 0.48
_glyphs['c'] = shape


shape = circle(0.1, 0.5, 0.5) & ~circle(0.1, 0.5, 0.4)
shape &= rectangle(0, 1, 0, 1)
shape |= rectangle(0, 0.1, 0, 1)
_widths['D'] = 0.6
_glyphs['D'] = shape


shape = reflect_x(_glyphs['b'], _widths['b']/2)
_widths['d'] = _widths['b']
_glyphs['d'] = shape


shape = rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.1, 0.6, 0.9, 1)
shape |= rectangle(0.1, 0.6, 0, 0.1)
shape |= rectangle(0.1, 0.5, 0.45, 0.55)
_widths['E'] = 0.6
_glyphs['E'] = shape


shape = circle(0.275, 0.275, 0.275)
shape &= ~circle(0.275, 0.275, 0.175)
shape &= ~triangle(0.1, 0.275, 0.75, 0.275, 0.6, 0)
shape |= rectangle(0.05, 0.55, 0.225, 0.315)
shape &=  circle(0.275, 0.275, 0.275)
_widths['e'] = 0.55
_glyphs['e'] = shape


shape = rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.1, 0.6, 0.9, 1)
shape |= rectangle(0.1, 0.5, 0.45, 0.55)
_widths['F'] = 0.6
_glyphs['F'] = shape


shape = circle(0.4, 0.75, 0.25) & ~circle(0.4, 0.75, 0.15)
shape &= rectangle(0, 0.4, 0.75, 1)
shape |= rectangle(0, 0.4, 0.45, 0.55)
shape |= rectangle(0.15, 0.25, 0, 0.75)
_widths['f'] = 0.4
_glyphs['f'] = shape


shape = circle(0.275, -0.1, 0.275)
shape &= ~circle(0.275, -0.1, 0.175)
shape &= rectangle(0, 0.55, -0.375, -0.1)
shape |= circle(0.275, 0.275, 0.275) & ~circle(0.275, 0.275, 0.175)
shape |= rectangle(0.45, 0.55, -0.1, 0.55)
_widths['g'] = 0.55
_glyphs['g'] = shape


shape = circle(0.3, 0.7, 0.3) & ~circle(0.3, 0.7, 0.2)
shape |= circle(0.3, 0.3, 0.3) & ~circle(0.3, 0.3, 0.2)
shape &= ~rectangle(0, 0.6, 0.3, 0.7)
shape |= rectangle(0, 0.1, 0.3, 0.7)
shape |= rectangle(0.5, 0.6, 0.3, 0.4)
shape |= rectangle(0.3, 0.6, 0.4, 0.5)
_widths['G'] = 0.6
_glyphs['G'] = shape


shape = rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.5, 0.6, 0, 1)
shape |= rectangle(0.1, 0.5, 0.45, 0.55)
_widths['H'] = 0.6
_glyphs['H'] = shape


shape = circle(0.275, 0.275, 0.275)
shape &= ~circle(0.275, 0.275, 0.175)
shape &= rectangle(0, 0.55, 0.275, 0.55)
shape |= rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.45, 0.55, 0, 0.275)
_widths['h'] = 0.55
_glyphs['h'] = shape


shape = rectangle(0, 0.5, 0, 0.1)
shape |= rectangle(0, 0.5, 0.9, 1)
shape |= rectangle(0.2, 0.3, 0.1, 0.9)
_widths['I'] = 0.5
_glyphs['I'] = shape


shape = rectangle(0.025, 0.125, 0, 0.55)
shape |= circle(0.075, 0.7, 0.075)
_widths['i'] = 0.15
_glyphs['i'] = shape


shape = circle(0.275, 0.275, 0.275)
shape &= ~circle(0.275, 0.275, 0.175)
shape &= rectangle(0, 0.55, 0, 0.275)
shape |= rectangle(0.45, 0.55, 0.275, 1)
_widths['J'] = 0.55
_glyphs['J'] = shape


shape = circle(0.0, -0.1, 0.275)
shape &= ~circle(0.0, -0.1, 0.175)
shape &= rectangle(0, 0.55, -0.375, -0.1)
shape |= rectangle(0.175, 0.275, -0.1, 0.55)
shape |= circle(0.225, 0.7, 0.075)
_widths['j'] = 0.3
_glyphs['j'] = shape


shape = rectangle(0, 0.6, 0, 1)
shape &= ~triangle(0.1, 1, 0.5, 1, 0.1, 0.6)
shape &= ~triangle(0.5, 0, 0.1, 0, 0.1, 0.4)
shape &= ~triangle(0.6, 0.95, 0.6, 0.05, 0.18, 0.5)
_widths['K'] = 0.6
_glyphs['K'] = shape


shape = rectangle(0, 0.5, 0, 1)
shape &= ~triangle(0.1, 1, 0.5, 1, 0.1, 0.45)
shape &= ~triangle(0.36, 0, 0.1, 0, 0.1, 0.25)
shape &= ~triangle(0.6, 1, 0.5, 0.0, 0.18, 0.35)
shape &= ~triangle(0.1, 1, 0.6, 1, 0.6, 0.5)
_widths['k'] = 0.5
_glyphs['k'] = shape


shape = rectangle(0, 0.6, 0, 0.1)
shape |= rectangle(0, 0.1, 0, 1)
_widths['L'] = 0.6
_glyphs['L'] = shape


shape = rectangle(0.025, 0.125, 0, 1)
_widths['l'] = 0.15
_glyphs['l'] = shape


shape = rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.7, 0.8, 0, 1)
shape |= triangle(0, 1, 0.1, 1, 0.45, 0)
shape |= triangle(0.45, 0, 0.35, 0, 0, 1)
shape |= triangle(0.7, 1, 0.8, 1, 0.35, 0)
shape |= triangle(0.35, 0, 0.8, 1, 0.45, 0)
_widths['M'] = 0.8
_glyphs['M'] = shape


shape = circle(0.175, 0.35, 0.175) & ~circle(0.175, 0.35, 0.075)
shape |= circle(0.425, 0.35, 0.175) & ~circle(0.425, 0.35, 0.075)
shape &= rectangle(0, 0.65, 0.35, 0.65)
shape |= rectangle(0, 0.1, 0, 0.525)
shape |= rectangle(0.25, 0.35, 0, 0.35)
shape |= rectangle(0.5, 0.6, 0, 0.35)
_widths['m'] = 0.6
_glyphs['m'] = shape


shape = rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.5, 0.6, 0, 1)
shape |= triangle(0, 1, 0.1, 1, 0.6, 0)
shape |= triangle(0.6, 0, 0.5, 0, 0, 1)
_widths['N'] = 0.6
_glyphs['N'] = shape


shape = circle(0.275, 0.275, 0.275)
shape &= ~circle(0.275, 0.275, 0.175)
shape &= rectangle(0, 0.55, 0.325, 0.55)
shape |= rectangle(0, 0.1, 0, 0.55)
shape |= rectangle(0.45, 0.55, 0, 0.325)
_widths['n'] = 0.55
_glyphs['n'] = shape


shape = circle(0.3, 0.7, 0.3) & ~circle(0.3, 0.7, 0.2)
shape |= circle(0.3, 0.3, 0.3) & ~circle(0.3, 0.3, 0.2)
shape &= ~rectangle(0, 0.6, 0.3, 0.7)
shape |= rectangle(0, 0.1, 0.3, 0.7)
shape |= rectangle(0.5, 0.6, 0.3, 0.7)
_widths['O'] = 0.6
_glyphs['O'] = shape


shape = circle(0.275, 0.275, 0.275)
shape &= ~circle(0.275, 0.275, 0.175)
_widths['o'] = 0.55
_glyphs['o'] = shape


shape = circle(0.3, 0.725, 0.275)
shape &= ~circle(0.3, 0.725, 0.175)
shape &= rectangle(0.3, 1, 0, 1)
shape |= rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.1, 0.3, 0.45, 0.55)
shape |= rectangle(0.1, 0.3, 0.9, 1)
_widths['P'] = 0.575
_glyphs['P'] = shape


shape = circle(0.275, 0.275, 0.275)
shape &= ~circle(0.275, 0.275, 0.175)
shape |= rectangle(0, 0.1, -0.375, 0.55)
_widths['p'] = 0.55
_glyphs['p'] = shape


shape = circle(0.3, 0.7, 0.3) & ~circle(0.3, 0.7, 0.2)
shape |= circle(0.3, 0.3, 0.3) & ~circle(0.3, 0.3, 0.2)
shape &= ~rectangle(0, 0.6, 0.3, 0.7)
shape |= rectangle(0, 0.1, 0.3, 0.7)
shape |= rectangle(0.5, 0.6, 0.3, 0.7)
shape |= triangle(0.5, 0.1, 0.6, 0.1, 0.6, 0)
shape |= triangle(0.5, 0.1, 0.5, 0.3, 0.6, 0.1)
_widths['Q'] = 0.6
_glyphs['Q'] = shape


shape = circle(0.275, 0.275, 0.275) & ~circle(0.275, 0.275, 0.175)
shape |= rectangle(0.45, 0.55, -0.375, 0.55)
_widths['q'] = 0.55
_glyphs['q'] = shape


shape = circle(0.3, 0.725, 0.275)
shape &= ~circle(0.3, 0.725, 0.175)
shape &= rectangle(0.3, 1, 0, 1)
shape |= rectangle(0, 0.1, 0, 1)
shape |= rectangle(0.1, 0.3, 0.45, 0.55)
shape |= rectangle(0.1, 0.3, 0.9, 1)
shape |= triangle(0.3, 0.5, 0.4, 0.5, 0.575, 0)
shape |= triangle(0.475, 0.0, 0.3, 0.5, 0.575, 0)
_widths['R'] = 0.575
_glyphs['R'] = shape


shape = circle(0.55, 0, 0.55) & ~scale_x(circle(0.55, 0, 0.45), 0.55, 0.8)
shape &= rectangle(0, 0.55, 0, 0.55)
shape = scale_x(shape, 0, 0.7)
shape |= rectangle(0, 0.1, 0, 0.55)
_widths['r'] = 0.385
_glyphs['r'] = shape


shape = circle(0.275, 0.725, 0.275)
shape &= ~circle(0.275, 0.725, 0.175)
shape &= ~rectangle(0.275, 0.55, 0.45, 0.725)
shape |= reflect_x(reflect_y(shape, 0.5), .275)
_widths['S'] = 0.55
_glyphs['S'] = shape


shape = circle(0.1625, 0.1625, 0.1625)
shape &= ~scale_x(circle(0.165, 0.165, 0.0625), 0.165, 1.5)
shape &= ~rectangle(0, 0.1625, 0.1625, 0.325)
shape |= reflect_x(reflect_y(shape, 0.275), 0.1625)
shape = scale_x(shape, 0, 1.5)
_widths['s'] = 0.4875
_glyphs['s'] = shape


shape = rectangle(0, 0.6, 0.9, 1) | rectangle(0.25, 0.35, 0, 0.9)
_widths['T'] = 0.6
_glyphs['T'] = shape


shape = circle(0.4, 0.25, 0.25) & ~circle(0.4, 0.25, 0.15)
shape &= rectangle(0, 0.4, 0, 0.25)
shape |= rectangle(0, 0.4, 0.55, 0.65)
shape |= rectangle(0.15, 0.25, 0.25, 1)
_widths['t'] = 0.4
_glyphs['t'] = shape


shape = circle(0.3, 0.3, 0.3) & ~circle(0.3, 0.3, 0.2)
shape &= rectangle(0, 0.6, 0, 0.3)
shape |= rectangle(0, 0.1, 0.3, 1)
shape |= rectangle(0.5, 0.6, 0.3, 1)
_widths['U'] = 0.6
_glyphs['U'] = shape


shape = circle(0.275, 0.275, 0.275) & ~circle(0.275, 0.275, 0.175)
shape &= rectangle(0, 0.55, 0, 0.275)
shape |= rectangle(0, 0.1, 0.275, 0.55)
shape |= rectangle(0.45, 0.55, 0, 0.55)
_widths['u'] = 0.55
_glyphs['u'] = shape


shape = triangle(0, 1, 0.1, 1, 0.35, 0)
shape |= triangle(0.35, 0, 0.25, 0, 0, 1)
shape |= reflect_x(shape, 0.3)
_widths['V'] = 0.6
_glyphs['V'] = shape


shape = triangle(0, 0.55, 0.1, 0.55, 0.35, 0)
shape |= triangle(0.35, 0, 0.25, 0, 0, 0.55)
shape |= reflect_x(shape, 0.3)
_widths['v'] = 0.6
_glyphs['v'] = shape


shape = triangle(0, 1, 0.1, 1, 0.25, 0)
shape |= triangle(0.25, 0, 0.15, 0, 0, 1)
shape |= triangle(0.15, 0, 0.35, 1, 0.45, 1)
shape |= triangle(0.45, 1, 0.25, 0, 0.15, 0)
shape |= reflect_x(shape, 0.4)
_widths['W'] = 0.8
_glyphs['W'] = shape


shape = triangle(0, 0.55, 0.1, 0.55, 0.25, 0)
shape |= triangle(0.25, 0, 0.15, 0, 0, 0.55)
shape |= triangle(0.15, 0, 0.35, 0.5, 0.45, 0.5)
shape |= triangle(0.45, 0.5, 0.25, 0, 0.15, 0)
shape |= reflect_x(shape, 0.4)
_widths['w'] = 0.8
_glyphs['w'] = shape


shape = triangle(0, 1, 0.125, 1, 0.8, 0)
shape |= triangle(0.8, 0, 0.675, 0, 0, 1)
shape |= reflect_x(shape, 0.4)
_widths['X'] = 0.8
_glyphs['X'] = shape


shape = triangle(0, 0.55, 0.125, 0.55, 0.55, 0)
shape |= triangle(0.55, 0, 0.425, 0, 0, 0.55)
shape |= reflect_x(shape, 0.275)
_widths['x'] = 0.55
_glyphs['x'] = shape


shape = triangle(0, 1, 0.1, 1, 0.45, 0.5)
shape |= triangle(0.45, 0.5, 0.35, 0.5, 0, 1)
shape |= reflect_x(shape, 0.4)
shape |= rectangle(0.35, 0.45, 0, 0.5)
_widths['Y'] = 0.8
_glyphs['Y'] = shape


shape = triangle(0, 0.55, 0.1, 0.55, 0.325, 0)
shape |= triangle(0.325, 0, 0.225, 0, 0, 0.55)
shape |= reflect_x(shape, 0.275) | move(reflect_x(shape, 0.275), -0.225, -0.55)
shape &= rectangle(0, 0.55, -0.375, 0.55)
_widths['y'] = 0.55
_glyphs['y'] = shape


shape = rectangle(0, 0.6, 0, 1)
shape &= ~triangle(0, 0.1, 0, 0.9, 0.45, 0.9)
shape &= ~triangle(0.6, 0.1, 0.15, 0.1, 0.6, 0.9)
_widths['Z'] = 0.6
_glyphs['Z'] = shape


shape = rectangle(0, 0.6, 0, 0.55)
shape &= ~triangle(0, 0.1, 0, 0.45, 0.45, 0.45)
shape &= ~triangle(0.6, 0.1, 0.15, 0.1, 0.6, 0.45)
_widths['z'] = 0.6
_glyphs['z'] = shape


shape = Shape("f1.0", 0, 0.55, 0, 1)
_widths[' '] = 0.55
_glyphs[' '] = shape


shape = circle(0.075, 0.075, 0.075)
shape = scale_y(shape, 0.075, 3)
shape &= rectangle(0.0, 0.15, -0.15, 0.075)
shape &= ~triangle(0.075, 0.075, 0.0, -0.15, -0.5, 0.075)
shape |= circle(0.1, 0.075, 0.075)
_widths[','] = 0.175
_glyphs[','] = shape


shape = circle(0.075, 0.075, 0.075)
_widths['.'] = 0.15
_glyphs['.'] = shape


shape = rectangle(0, 0.1, 0.55, 0.8)
_widths["'"] = 0.1
_glyphs["'"] = shape

shape = rectangle(0, 0.1, 0.55, 0.8) | rectangle(0.2, 0.3, 0.55, 0.8)
_widths['"'] = 0.3
_glyphs['"'] = shape


shape = circle(0.075, 0.15, 0.075) | circle(0.075, 0.45, 0.075)
_widths[':'] = 0.15
_glyphs[':'] = shape


shape = circle(0.075, 0.15, 0.075)
shape = scale_y(shape, 0.15, 3)
shape &= rectangle(0.0, 0.15, -0.075, 0.15)
shape &= ~triangle(0.075, 0.15, 0.0, -0.075, -0.5, 0.15)
shape |= circle(0.075, 0.45, 0.075)
shape |= circle(0.1, 0.15, 0.075)
_widths[';'] = 0.15
_glyphs[';'] = shape


shape = rectangle(0.025, 0.125, 0.3, 1)
shape |= circle(0.075, 0.075, 0.075)
_widths['!'] = 0.1
_glyphs['!'] = shape


shape = rectangle(0.05, 0.4, 0.35, 0.45)
_widths['-'] = 0.45
_glyphs['-'] = shape


shape = circle(0, 0.4, 0.6) & ~scale_x(circle(0, 0.4, 0.5), 0, 0.7)
shape &= rectangle(0, 0.6, -0.2, 1)
shape = scale_x(shape, 0, 1/2.)
_widths[')'] = 0.3
_glyphs[')'] = shape


shape = circle(0.6, 0.4, 0.6) & ~scale_x(circle(0.6, 0.4, 0.5), 0.6, 0.7)
shape &= rectangle(0, 0.6, -0.2, 1)
shape = scale_x(shape, 0, 1/2.)
_widths['('] = 0.3
_glyphs['('] = shape


shape = rectangle(0, 0.3, 0, 1)
shape &= ~circle(0, 1, 0.2)
shape &= ~rectangle(0, 0.2, 0, 0.7)
_widths['1'] = 0.3
_glyphs['1'] = shape


shape = circle(0.275, .725, .275)
shape &= ~circle(0.275, 0.725, 0.175)
shape &= ~rectangle(0, 0.55, 0, 0.725)
shape |= rectangle(0, 0.55, 0, 0.1)
shape |= triangle(0, 0.1, 0.45, 0.775, 0.55, 0.725)
shape |= triangle(0, 0.1, 0.55, 0.725, 0.125, 0.1)
_widths['2'] = 0.55
_glyphs['2'] = shape


shape = circle(0.3, 0.725, 0.275)
shape &= ~circle(0.3, 0.725, 0.175)
shape |= circle(0.3, 0.275, 0.275)
shape &= ~circle(0.3, 0.275, 0.175)
shape &= ~rectangle(0, 0.275, 0.275, 0.725)
_widths['3'] = 0.55
_glyphs['3'] = shape


shape = triangle(-0.10, 0.45, 0.4, 1, 0.4, 0.45)
shape |= rectangle(0.4, 0.5, 0, 1)
shape &= ~triangle(0.4, 0.85, 0.4, 0.55, 0.1, 0.55)
shape &= rectangle(0, 0.5, 0, 1)
_widths['4'] = 0.5
_glyphs['4'] = shape


shape = circle(0.325, 0.325, 0.325) & ~circle(0.325, 0.325, 0.225)
shape &= ~rectangle(0, 0.325, 0.325, 0.65)
shape |= rectangle(0, 0.325, 0.55, 0.65)
shape |= rectangle(0, 0.1, 0.55, 1)
shape |= rectangle(0.1, 0.65, 0.9, 1)
_widths['5'] = 0.65
_glyphs['5'] = shape


shape = circle(0.275, 0.725, 0.275) & ~scale_y(circle(0.275, 0.725, 0.175), .725, 1.2)
shape &= rectangle(0, 0.55, 0.725, 1)
shape &= ~triangle(0.275, 0.925, 0.55, 0.9, 0.55, 0.725)
shape = scale_y(shape, 1, 2)
shape = scale_x(shape, 0, 1.1)
shape &= ~rectangle(0.275, 0.65, 0., 0.7)
shape |= rectangle(0, 0.1, 0.275, 0.45)
shape |= circle(0.275, 0.275, 0.275) & ~circle(0.275, 0.275, 0.175)
_widths['6'] = 0.55
_glyphs['6'] = shape


shape = rectangle(0, 0.6, 0.9, 1)
shape |= triangle(0, 0, 0.475, 0.9, 0.6, 0.9)
shape |= triangle(0, 0, 0.6, 0.9, 0.125, 0)
_widths['7'] = 0.6
_glyphs['7'] = shape


shape = circle(0.3, 0.725, 0.275)
shape &= ~circle(0.3, 0.725, 0.175)
shape |= circle(0.3, 0.275, 0.275)
shape &= ~circle(0.3, 0.275, 0.175)
_widths['8'] = 0.55
_glyphs['8'] = shape


shape = reflect_x(reflect_y(_glyphs['6'], 0.5), _widths['6']/2)
_widths['9'] = _widths['6']
_glyphs['9'] = shape


shape = circle(0.5, 0.5, 0.5) & ~scale_x(circle(0.5, 0.5, 0.4), 0.5, 0.7**0.5)
shape = scale_x(shape, 0, 0.7)
_widths['0'] = 0.7
_glyphs['0'] = shape


shape = rectangle(0., 0.5, 0.45, 0.55)
shape |= rectangle(0.2, 0.3, 0.25, 0.75)
_widths['+'] = 0.55
_glyphs['+'] = shape


shape = triangle(0, 0, 0.425, 1, 0.55, 1)
shape |= triangle(0, 0, 0.55, 1, 0.125, 0)
_widths['/'] = 0.55
_glyphs['/'] = shape


shape = circle(0.275, 0.725, 0.275) & ~circle(0.275, 0.725, 0.175)
shape &= ~rectangle(0, 0.275, 0.45, 0.725)
shape |= rectangle(0.225, 0.325, 0.3, 0.55)
shape |= circle(0.275, 0.075, 0.075)
_widths['?'] = 0.55
_glyphs['?'] = shape

del shape
