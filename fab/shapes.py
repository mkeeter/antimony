import math

from expression import Expression

def circle(x0, y0, r):

    # sqrt((X-x0)**2 + (Y-y0)**2) - r
    r = abs(r)
    s = Expression('-r+q%sq%sf%g' % (('-Xf%g' % x0) if x0 else 'X',
                                     ('-Yf%g' % y0) if y0 else 'Y', r))

    s.xmin, s.xmax = x0-r, x0+r
    s.ymin, s.ymax = y0-r, y0+r

    return s

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
    s = Expression('ni%(e0)si%(e1)s%(e2)s' % locals())

    s.xmin, s.xmax = min(x0, x1, x2), max(x0, x1, x2)
    s.ymin, s.ymax = min(y0, y1, y2), max(y0, y1, y2)

    return s

def right_triangle(x0, y0, h):
    corner = Expression('-f%fX' % x0) & Expression('-f%fY' % y0)
    return corner & Expression('-X-f%f-Yf%f' % (x0+h, y0))

################################################################################

def rectangle(x0, x1, y0, y1):
    # max(max(x0 - X, X - x1), max(y0 - Y, Y - y1)
    s = Expression('aa-f%(x0)gX-Xf%(x1)ga-f%(y0)gY-Yf%(y1)g' % locals())

    s.xmin, s.xmax = x0, x1
    s.ymin, s.ymax = y0, y1

    return s

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
    p = part.map('-Xf%g' % dx if dx else None,
                 '-Yf%g' % dy if dy else None,
                 '-Zf%g' % dz if dz else None)
    p.xmin, p.xmax = part.xmin + dx, part.xmax + dx
    p.ymin, p.ymax = part.ymin + dy, part.ymax + dy
    p.zmin, p.zmax = part.zmin + dz, part.zmax + dz

    return p

translate = move

################################################################################

def rotate(part, angle):

    angle *= math.pi/180
    ca, sa = math.cos(angle), math.sin(angle)
    nsa    = -sa

    p = part.map(X='+*f%(ca)gX*f%(sa)gY'  % locals(),
                 Y='+*f%(nsa)gX*f%(ca)gY' % locals())

    ca, sa = math.cos(-angle), math.sin(-angle)
    nsa    = -sa
    p.set_bounds(*part.remap_bounds(X='+*f%(ca)gX*f%(sa)gY'  % locals(),
                                    Y='+*f%(nsa)gX*f%(ca)gY' % locals()))

    return p

################################################################################

def reflect_x(part, x0=0):
    # X' = 2*x0-X
    p = part.map(X='-*f2f%gX' % x0 if x0 else 'nX')

    # X  = 2*x0-X'
    p.set_bounds(*part.remap_bounds(X='-*f2f%gX' % x0 if x0 else 'nX'))
    return p

def reflect_y(part, y0=0):
    # Y' = 2*y0-Y
    p = part.map(Y='-*f2f%gY' % y0 if y0 else 'nY')

    # Y  = 2*y0-Y'
    p.set_bounds(*part.remap_bounds(Y='-*f2f%gY' % y0 if y0 else 'nY'))
    return p

def reflect_xy(part):
    p = part.map(X='Y', Y='X')
    p.set_bounds(*part.remap_bounds(X='Y', Y='X'))
    return p

################################################################################

def scale_x(part, x0, sx):
    # X' = x0 + (X-x0)/sx
    p = part.map(X='+f%(x0)g/-Xf%(x0)gf%(sx)g' % locals()
                    if x0 else '/Xf%g' % sx)

    # X  = (X'-x0)*sx + x0
    p.set_bounds(*part.remap_bounds(X='+f%(x0))g*f%(sx)g-Xf%(x0)g' % locals()
                                    if x0 else '*Xf%g' % sx))
    return p

def scale_y(part, y0, sy):
    # Y' = y0 + (Y-y0)/sy
    p = part.map(Y='+f%(y0)g/-Yf%(y0)gf%(sy)g' % locals()
                    if y0 else '/Yf%g' % sy)

    # Y  = (Y'-y0)*sy + y0
    p.set_bounds(*part.remap_bounds(Y='+f%(y0))g*f%(sy)g-Yf%(y0)g' % locals()
                                    if y0 else '*Yf%g' % sy))
    return p

def scale_z(part, z0, sz):
    # Z' = z0 + (Y-y0)/sz
    p = part.map(Z='+f%(z0)g/-Zf%(z0)gf%(sz)g' % locals()
                    if z0 else '/Zf%g' % sz)

    # Z  = (Z'-z0)*sz + z0
    p.set_bounds(*part.map_bounds(Z='+f%(z0)g*f%(sz)g-Zf%(z0)g' % locals()
                                  if z0 else '*Zf%g' % sz))
    return p

def scale_xy(part, x0, y0, sxy):
    # X' = x0 + (X-x0)/sx
    # Y' = y0 + (Y-y0)/sy
    p = part.map(X='+f%(x0)g/-Xf%(x0)gf%(sxy)g' % locals()
                    if x0 else '/Xf%g' % sxy,
                 Y='+f%(y0)g/-Yf%(y0)gf%(sxy)g' % locals()
                    if y0 else '/Yf%g' % sxy)

    # X  = (X'-x0)*sx + x0
    # Y  = (Y'-y0)*sy + y0
    p.set_bounds(*part.remap_bounds(X='+f%(x0))g*f%(sxy)g-Xf%(x0)g' % locals()
                                   if x0 else '*Xf%g' % sxy,
                                   Y='+f%(y0)g*f%(sxy)g-Yf%(y0)g' % locals()
                                   if y0 else '*Yf%g' % sxy))
    return p

################################################################################

def shear_x_y(part, y0, y1, dx0, dx1):
    dx = dx1 - dx0
    dy = y1 - y0

    # X' = X-dx0-dx*(Y-y0)/dy
    p = part.map(X='--Xf%(dx0)g/*f%(dx)g-Yf%(y0)gf%(dy)g' % locals())

    # X  = X'+dx0+(dx)*(Y-y0)/dy
    p.set_bounds(*part.remap_bounds(X='++Xf%(dx0))g/*f%(dx)g-Yf%(y0)gf%(dy)g'
                                    % locals()))
    return p

################################################################################

def taper_x_y(part, x0, y0, y1, s0, s1):
    dy = y1 - y0
    ds = s1 - s0
    s0y1 = s0 * y1
    s1y0 = s1 * y0

    #   X'=x0+(X-x0)*(y1-y0)/(Y*(s1-s0)+s0*y1-s1*y0))
    X = '+f%(x0)g/*-Xf%(x0)gf%(dy)g-+*Yf%(ds)gf%(s0y1)gf%(s1y0)g' % locals()
    p = part.map(X=X)

    #   X=(X'-x0)*(Y*(s1-s0)+s0*y1-s1*y0)/(y1-y0)+x0
    p.set_bounds(*part.remap_bounds(
            X='+f%(x0)g*-Xf%(x0)g/-+*Yf%(ds)gf%(s0y1)gf%(s1y0)gf%(dy)g'
            % locals()))

    return p

################################################################################

def blend(p0, p1, amount):
    joint = p0 | p1

    # sqrt(abs(p0)) + sqrt(abs(p1)) - amount
    fillet = Expression('-+rb%srb%sf%g' % (p0.math, p1.math, amount))
    out = joint | fillet
    out.set_bounds(joint.xmin, joint.xmax,
                   joint.ymin, joint.ymax,
                   joint.zmin, joint.zmax)

    return out

################################################################################

def extrusion(part, z0, z1):
    # max(part, max(z0-Z, Z-z1))
    s = Expression('a%sa-f%gZ-Zf%g' % (part.math, z0, z1))
    for i in 'xmin','xmax','ymin','ymax':
        setattr(s, i, getattr(part, i))
    s.zmin, s.zmax = z0, z1
    return s

def cylinder(x0, y0, z0, z1, r):
    return extrusion(circle(x0, y0, r), z0, z1)

def sphere(x0, y0, z0, r):
    s = Expression('-r++q%sq%sq%sf%g' % (('-Xf%g' % x0) if x0 else 'X',
                                         ('-Yf%g' % y0) if y0 else 'Y',
                                         ('-Zf%g' % z0) if z0 else 'Z',
                                         r))
    s.xmin, s.xmax = x0 - r, x0 + r
    s.ymin, s.ymax = y0 - r, y0 + r
    s.zmin, s.zmax = z0 - r, z0 + r
    return s

def cube(x0, x1, y0, y1, z0, z1):
    return extrusion(rectangle(x0, x1, y0, y1), z0, z1)

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
    nsa    = -sa

    p = part.map(Y='+*f%(ca)gY*f%(sa)gZ'  % locals(),
                 Z='+*f%(nsa)gY*f%(ca)gZ' % locals())

    ca, sa = math.cos(-angle), math.sin(-angle)
    nsa    = -sa
    p.set_bounds(*part.remap_bounds(Y='+*f%(ca))gY*f%(sa)gZ' % locals(),
                                    Z='+*f%(nsa)gY*f%(ca)gZ' % locals()))
    return p

def rotate_y(part, angle):

    angle *= math.pi/180
    ca, sa = math.cos(angle), math.sin(angle)
    nsa    = -sa

    p = part.map(X='+*f%(ca)gX*f%(sa)gZ'  % locals(),
                 Z='+*f%(nsa)gX*f%(ca)gZ' % locals())

    ca, sa = math.cos(-angle), math.sin(-angle)
    nsa    = -sa

    p.set_bounds(*part.remap_bounds(X='+*f%(ca))gX*f%(sa)gZ' % locals(),
                                    Z='+*f%(nsa)gX*f%(ca)gZ' % locals()))
    return p

rotate_z = rotate

################################################################################

def reflect_z(part, z0=0):
    p = part.map(Z='-*f2f%gZ' % z0 if z0 else 'nZ')
    p.set_bounds(*part.remap_bounds(Z='-*f2f%gZ' % z0 if z0 else 'nZ'))
    return p

def reflect_xz(part):
    p = part.map(X='Z', Z='X')
    p.set_bounds(*part.remap_bounds(X='Z', Z='X'))
    return p

def reflect_yz(part):
    p = part.map(Y='Z', Z='Y')
    p.set_bounds(*part.remap_bounds(Y='Z', Z='Y'))
    return p

################################################################################

def scale_z(part, z0, sz):
    p = part.map(Z='+f%(z0)g/-Zf%(z0)gf%(sz)g' % locals()
                    if z0 else '/Zf%g' % sz)
    p.set_bounds(*part.remap_bounds(Z='+f%(z0))g*f%(sz)g-Zf%(z0)g' % locals()
                                    if z0 else '*Zf%g' % sz))
    return p

################################################################################

def shear_x_z(part, z0, z1, dx0, dx1):

    #   X' = X-dx0-(dx1-dx0)*(Z-z0)/(z1-z0)
    p = part.map(X='--Xf%(dx0)g/*f%(dx)g-Zf%(z0)gf%(dz)g' % locals())

    #   X = X'+dx0+(dx1-dx0)*(Z-z0)/(z1-z0)
    p.set_bounds(*part.remap_bounds(X='++Xf%(dx0))g/*f%(dx)g-Zf%(z0)gf%(dz)g'
                                    % locals()))
    return p

################################################################################

def taper_xy_z(part, x0, y0, z0, z1, s0, s1):

    dz = z1 - z0

    # X' =  x0 +(X-x0)*dz/(s1*(Z-z0) + s0*(z1-Z))
    # Y' =  y0 +(Y-y0)*dz/(s1*(Z-z0) + s0*(z1-Z))
    p = part.map(
        X='+f%(x0)g/*-Xf%(x0)gf%(dz)g+*f%(s1)g-Zf%(z0)g*f%(s0)g-f%(z1)gZ'
            % locals(),
        Y='+f%(y0)g/*-Yf%(y0)gf%(dz)g+*f%(s1)g-Zf%(z0)g*f%(s0)g-f%(z1)gZ'
            % locals())

    # X  = (X' - x0)*(s1*(Z-z0) + s0*(z1-Z))/dz + x0
    # Y  = (Y' - y0)*(s1*(Z-z0) + s0*(z1-Z))/dz + y0
    p.set_bounds(part.remap_bounds(
        X='+/*-Xf%(x0)g+*f%(s1)g-Zf%(z0)g*f%(s0)g-f%(z1)gZf%(dz)gf%(x0)g'
            % locals(),
        Y='+/*-Yf%(y0)g+*f%(s1)g-Zf%(z0)g*f%(s0)g-f%(z1)gZf%(dz)gf%(y0)g'
            % locals()))

    return p

################################################################################

def revolve_y(part):
    ''' Revolve a part in the XY plane about the Y axis. '''
    #   X' = sqrt(X**2 + Z**2)
    p = part.map(X='r+qXqZ')

    if part.bounds[0] and part.bounds[1]:
        p.xmin = min(-abs(part.xmin), -abs(part.xmax))
        p.xmax = max( abs(part.xmin),  abs(part.xmax))
        p.ymin, p.ymax = part.ymin, part.ymax
        p.zmin, p.zmax = p.xmin, p.xmax
    return p


def revolve_x(part):
    ''' Revolve a part in the XY plane about the X axis. '''
    #   Y' = sqrt(Y**2 + Z**2)
    p = part.map(Y='r+qYqZ')

    if part.bounds[0] and part.bounds[1]:
        p.xmin, p.xmax = part.xmin, part.xmax
        p.ymin = min(-abs(part.ymin), -abs(part.ymax))
        p.ymax = max( abs(part.ymin),  abs(part.ymax))
        p.zmin, p.zmax =  p.ymin, p.ymax
    return p


