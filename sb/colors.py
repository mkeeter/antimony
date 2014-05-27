from fab.expression import Expression

# Standard Solarized color scheme
base03  = "#002b36"
base02  = "#073642"
base01  = "#586e75"
base00  = "#657b83"
base0   = "#839496"
base1   = "#93a1a1"
base2   = "#eee8d5"
base3   = "#fdf6e3"

yellow  = "#b58900"
orange  = "#cb4b16"
red     = "#dc322f"
magenta = "#d33682"
violet  = "#6c71c4"
blue    = "#268bd2"
cyan    = "#2aa198"
green   = "#859900"

def highlight(color, scale=1.2):
    r = min(255, int(eval('0x%s' % color[1:3]) * scale))
    g = min(255, int(eval('0x%s' % color[3:5]) * scale))
    b = min(255, int(eval('0x%s' % color[5:7]) * scale))

    return ('#%2x%2x%2x' % (r, g, b)).replace(' ', '0')

# Automatically generate highlighted and dimmed versions
# of the colors
colors = list(locals().keys())
for c in colors:
    if not c.startswith('__') and isinstance(eval(c), str):
        exec('%s_h = "%s"' % (c, highlight(eval(c))))
        exec('%s_d = "%s"' % (c, highlight(eval(c), 0.8)))

def get_color(T):
    if T is float:          return yellow
    elif T is int:          return magenta
    elif T is Expression:   return blue
    elif T is str:          return violet
    raise TypeError("No color assigned for type %s" % T)
