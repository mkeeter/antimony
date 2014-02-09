yellow = (181, 137, 0)
orange = (203, 75, 22)
red = (220, 50, 47)
magenta = (211, 54, 130)
violet = (108, 113, 196)
blue = (38, 139, 210)
green = (133, 153, 0)
cyan = (42, 161, 152)

base03 = (0, 43, 54)
base02 = (7, 54, 66)
base01 = (88, 110, 117)
base00 = (101, 123, 131)
base0 = (131, 148, 150)
base1 = (147, 161, 161)
base2 = (238, 232, 213)
base3 = (253, 246, 227)

dark_grey = (100, 100, 100)
light_grey = (200, 200, 200)
grey = (150, 150, 150)
white = (255, 255, 255)


from fab.expression import Expression

def get_color(T):
    if T is float:          return yellow
    elif T is int:          return magenta
    elif T is Expression:   return blue
    elif T is str:          return violet
    raise TypeError("No color assigned for type %s" % T)

def highlight(C):
    return (min(255, c+60) for c in C)
