from sb.nodes.node import Node3D
from sb.controls.point import Point3DControl

class Point3D(Node3D):
    menu_name = "Point (3D)"
    menu_category = "3D"

    name_prefix = 'p'

    def __init__(self, name, x, y, z, scale):
        super(Point3D, self).__init__(name, x, y, z)

    def make_controls(self, canvas):
        return Point3DControl(self, canvas)
