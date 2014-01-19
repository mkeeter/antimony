import base
import datum

class Point(base.Node2D):
    def __init__(self, name, x, y):
        super(Point, self).__init__(name, x, y)

    def get_control(self):
        import control.point
        return control.point.PointControl

class Point3D(base.Node3D):
    def __init__(self, name, x, y, z):
        super(Point3D, self).__init__(name, x, y, z)

    def get_control(self):
        import control.point
        return control.point.Point3DControl

