import base
import datum

class Point(base.Node):
    def __init__(self, name, x, y):

        super(Point, self).__init__(name)
        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))

    def get_control(self):
        import control.point
        return control.point.PointControl

class Point3D(base.Node):
    def __init__(self, name, x, y, z):
        super(Point3D, self).__init__(name)
        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('z', datum.FloatDatum(self, z))

    def get_control(self):
        import control.point
        return control.point.Point3DControl

