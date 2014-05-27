from sb.nodes.node import Node3D, Node2D
from sb.controls.point import Point3DControl, Point2DControl

class Point3D(Node3D):
    menu_name = "Point (3D)"
    menu_category = "3D"

    name_prefix = 'p'
    _control_type = Point3DControl

    def __init__(self, name, x, y, z, scale):
        super(Point3D, self).__init__(name, x, y, z)

################################################################################

class Point2D(Node2D):
    menu_name = "Point (2D)"
    menu_category = "2D"

    name_prefix = 'p'
    _control = Point2DControl

    def __init__(self, name ,x, y, z, scale):
        super(Point2D, self).__init__(name, x, y)
