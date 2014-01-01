import base
import datum

class Point(base.Node):
    def __init__(self, name, x, y):

        super(Point, self).__init__(name)
        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))

    def get_control(self, is_child):
        import control.point
        if is_child:    return control.point.ChildPointControl
        else:           return control.point.PointControl

