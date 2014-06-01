from sb.nodes.node import Node3D, Node2D
from sb.controls.point import Point3DControl, Point2DControl

class Point3D(Node3D):
    menu_name = "Point (3D)"
    menu_category = "3D"
    name_prefix = 'p'

    def __init__(self, canvas, name, x, y, z):
        super(Point3D, self).__init__(canvas, name, x, y, z)
        self.control = Point3DControl(canvas, self)

    @classmethod
    def new(cls, canvas, name, x, y, z, scale):
        return cls(canvas, name, x, y, z)

################################################################################

class Point2D(Node2D):
    menu_name = "Point (2D)"
    menu_category = "2D"
    name_prefix = 'p'

    def __init__(self, canvas, name, x, y):
        super(Point2D, self).__init__(canvas, name, x, y)
        self.control = Point2DControl(canvas, self)

    @classmethod
    def new(cls, canvas, name, x, y, z, scale):
        return cls(canvas, name, x, y)
