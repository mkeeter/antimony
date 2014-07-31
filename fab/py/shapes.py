import math

from fab import Shape, Transform

def circle(x0, y0, r):
    # sqrt((X-x0)**2 + (Y-y0)**2) - r
    r = abs(r)
    return Shape(
            '-r+q%sq%sf%g' % (('-Xf%g' % x0) if x0 else 'X',
                              ('-Yf%g' % y0) if y0 else 'Y', r),
            x0 - r, y0 - r, x0 + r, y0 + r)

################################################################################

def triangle(x0, y0, x1, y1, x2, y2):
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

################################################################################

def rectangle(x0, x1, y0, y1):
    # max(max(x0 - X, X - x1), max(y0 - Y, Y - y1)
    return Shape(
            'aa-f%(x0)gX-Xf%(x1)ga-f%(y0)gY-Yf%(y1)g' % locals(),
            x0, y0, x1, y1)

def rounded_rectangle(x0, x1, y0, y1, r):
    r *= min(x1 - x0, y1 - y0)/2
    return (
        rectangle(x0, x1, y0+r, y1-r) |
        rectangle(x0+r, x1-r, y0, y1) |
        circle(x0+r, y0+r, r) |
        circle(x0+r, y1-r, r) |
        circle(x1-r, y0+r, r) |
        circle(x1-r, y1-r, r)
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

def move(part, dx, dy, dz=0):
    return part.map(Transform(
        '-Xf%g' % dx, '-Yf%g' % dy, '-Zf%g' % dz,
        '+Xf%g' % dx, '+Yf%g' % dy, '+Zf%g' % dz))

translate = move

################################################################################

def rotate(part, angle):

    angle *= math.pi/180
    ca, sa = math.cos(angle), math.sin(angle)
    nca, nsa = math.cos(-angle), math.sin(-angle)

    return part.map(Transform(
        '+*f%(ca)gX*f%(sa)gY'    % locals(),
        '+*f%(nsa)gX*f%(ca)gY'   % locals(),
        '+*f%(nca)gX*f%(nsa)gY'  % locals(),
        '+*f%(sa)gX*f%(nca)gY'   % locals()))

################################################################################

def reflect_x(part, x0=0):
    # X' = 2*x0-X
    # X  = 2*x0-X'
    return part.map(Transform(
        '-*f2f%gX' % x0, '',
        '-*f2f%gX' % x0, ''))

def reflect_y(part, y0=0):
    # Y' = 2*y0-Y
    return part.map(Transform(
        '', '-*f2f%gY' % y0,
        '', '-*f2f%gY' % y0))

def reflect_xy(part):
    return part.map(Transform(
        'Y', 'X', 'Y', 'X'))

################################################################################

def scale_x(part, x0, sx):
    # X' = x0 + (X-x0)/sx
    return part.map(Transform(
        '+f%(x0)g/-Xf%(x0)gf%(sx)g' % locals()
                if x0 else '/Xf%g' % sx,
        'Y',
        '+f%(x0)g*f%(sx)g-Xf%(x0)g' % locals()
                if x0 else '*Xf%g' % sx,
        'Y'))

def scale_y(part, y0, sy):
    # Y' = y0 + (Y-y0)/sy
    return part.map(Transform(
        'X',
        '+f%(y0)g/-Yf%(y0)gf%(sy)g' % locals()
                if y0 else '/Yf%g' % sy,
        'X',
        '+f%(y0)g*f%(sy)g-Yf%(y0)g' % locals()
                if y0 else '*Yf%g' % sy))

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

def scale_xy(part, x0, y0, sxy):
    # X' = x0 + (X-x0)/sx
    # Y' = y0 + (Y-y0)/sy
    # X  = (X'-x0)*sx + x0
    # Y  = (Y'-y0)*sy + y0
    return part.map(Transform(
        '+f%(x0)g/-Xf%(x0)gf%(sxy)g' % locals()
                if x0 else '/Xf%g' % sxy,
        '+f%(y0)g/-Yf%(y0)gf%(sxy)g' % locals()
                if y0 else '/Yf%g' % sxy,
        '+f%(x0))g*f%(sxy)g-Xf%(x0)g' % locals()
                if x0 else '*Xf%g' % sxy,
        '+f%(y0)g*f%(sxy)g-Yf%(y0)g' % locals()
                if y0 else '*Yf%g' % sxy))

################################################################################

def extrude_z(part, z0, z1):
    # max(part, max(z0-Z, Z-z1))
    return Shape(
            'am  f1%sa-f%gZ-Zf%g' % (part.math, z0, z1),
            part.bounds.xmin, part.bounds.ymin, z0,
            part.bounds.xmax, part.bounds.ymax, z1)

################################################################################

def shear_x_y(part, y0, y1, dx0, dx1):
    dx = dx1 - dx0
    dy = y1 - y0

    # X' = X-dx0-dx*(Y-y0)/dy
    # X  = X'+dx0+(dx)*(Y-y0)/dy
    return part.map(Transform(
            '--Xf%(dx0)g/*f%(dx)g-Yf%(y0)gf%(dy)g' % locals(),
            'Y',
            '++Xf%(dx0)g/*f%(dx)g-Yf%(y0)gf%(dy)g' % locals(),
            'Y'))

################################################################################

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
        '+f%(x0)g*-Xf%(x0)g/-+*Yf%(ds)gf%(s0y1)gf%(s1y0)gf%(dy)g' % locals()))

################################################################################

def blend(p0, p1, amount):
    joint = p0 | p1

    # sqrt(abs(p0)) + sqrt(abs(p1)) - amount
    fillet = Shape('-+rb%srb%sf%g' % (p0.math, p1.math, amount),
                       joint.bounds)
    return joint | fillet

################################################################################

def cylinder(x0, y0, z0, z1, r):
    return extrude_z(circle(x0, y0, r), z0, z1)

def sphere(x0, y0, z0, r):
    return Shape(
            '-r++q%sq%sq%sf%g' % (('-Xf%g' % x0) if x0 else 'X',
                                  ('-Yf%g' % y0) if y0 else 'Y',
                                  ('-Zf%g' % z0) if z0 else 'Z',
                                  r),
            x0 - r, y0 - r, z0 - r, x0 + r, y0 + r, z0 + r)

def cube(x0, x1, y0, y1, z0, z1):
    return extrude_z(rectangle(x0, x1, y0, y1), z0, z1)

def cone(x0, y0, z0, z1, r):
    cyl = cylinder(x0, y0, z0, z1, r)
    return taper_xy_z(cyl, x0, y0, z0, z1, 1.0, 0.0)

def pyramid(x0, x1, y0, y1, z0, z1):
    c = cube(x0, x1, y0, y1, z0, z1)
    return taper_xy_z(c, (x0+x1)/2., (y0+y1)/2., z0, z1, 1.0, 0.0)

################################################################################

# 3D shapes and operations

def rotate_x(part, angle):

    angle *= math.pi/180
    ca, sa = math.cos(angle), math.sin(angle)
    nca, nsa = math.cos(-angle), math.sin(-angle)

    return part.map(Transform(
        '', '+*f%(ca)gY*f%(sa)gZ'  % locals(),
            '+*f%(nsa)gY*f%(ca)gZ' % locals(),

        'X', '+*f%(nca)gY*f%(nsa)gZ' % locals(),
             '+*f%(sa)gY*f%(nca)gZ' % locals()))

def rotate_y(part, angle):

    angle *= math.pi/180
    ca, sa = math.cos(angle), math.sin(angle)
    nca, nsa = math.cos(-angle), math.sin(-angle)

    return part.map(Transform(
            '+*f%(ca)gX*f%(sa)gZ'  % locals(), 'Y',
            '+*f%(nsa)gX*f%(ca)gZ' % locals(),
            '+*f%(nca)gX*f%(nsa)gZ' % locals(), 'Y',
            '+*f%(sa)gX*f%(nca)gZ' % locals()))

rotate_z = rotate

################################################################################

def reflect_z(part, z0=0):
    return part.map(Transform(
        'X', 'Y', '-*f2f%gZ' % z0 if z0 else 'nZ',
        'X', 'Y', '-*f2f%gZ' % z0 if z0 else 'nZ'))

def reflect_xz(part):
    return part.map(Transform('Z', 'Y', 'X', 'Z', 'Y', 'X'))

def reflect_yz(part):
    p = part.map(Transform('X', 'Z', 'Y', 'X', 'Z', 'Y'))

################################################################################

def shear_x_z(part, z0, z1, dx0, dx1):
    #   X' = X-dx0-(dx1-dx0)*(Z-z0)/(z1-z0)
    #   X = X'+dx0+(dx1-dx0)*(Z-z0)/(z1-z0)
    return part.map(Transform(
        '--Xf%(dx0)g/*f%(dx)g-Zf%(z0)gf%(dz)g' % locals(), '', '',
        '++Xf%(dx0))g/*f%(dx)g-Zf%(z0)gf%(dz)g' % locals(), '', ''))

################################################################################

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

def revolve_y(part):
    ''' Revolve a part in the XY plane about the Y axis. '''
    #   X' = sqrt(X**2 + Z**2)
    p = part.map('r+qXqZ', '', '', '')
    return Shape(
            p.math,
            min(-abs(part.xmin), -abs(part.xmax)),
            max( abs(part.xmin),  abs(part.xmax)),
            part.ymin, part.ymax,
            part.xmin, part.xmax)


def revolve_x(part):
    ''' Revolve a part in the XY plane about the X axis. '''
    #   Y' = sqrt(Y**2 + Z**2)
    p = part.map('', 'r+qYqZ', '', '')
    return Shape(
            p.math,
            part.xmin, part.xmax,
            min(-abs(part.ymin), -abs(part.ymax)),
            max( abs(part.ymin),  abs(part.ymax)),
            part.ymin, part.ymax)

################################################################################

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
        part.xmin - b, part.ymin - b, part.zmin - b,
        part.xmax + b, part.ymax + b, part.zmax + b), x, y, z)

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
        part.xmin - b, part.ymin - b, part.zmin - b,
        part.xmax + b, part.ymax + b, part.zmax + b),
        x, y, z)

################################################################################

def text(text, x, y, height=1, align='LB'):
    if text == '':
        return Shape("f1.0", 0, 0, 0, 0)
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

