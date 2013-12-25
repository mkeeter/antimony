yellow = (181, 137, 0)
orange = (203, 75, 22)
red = (220, 50, 47)
magenta = (211, 54, 130)
violet = (108, 113, 196)
blue = (38, 139, 210)
green = (133, 153, 0)

def get_color(T):
    if T is float:  return blue
    raise TypeError("No color assigned for type %s" % T)

def highlight(C):
    return (min(255, c+60) for c in C)
