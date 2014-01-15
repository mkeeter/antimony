import base
import datum

class GetBounds(base.Node):
    def __init__(self, name, x, y, z):
        super(GetBounds, self).__init__(name)

        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('z', datum.FloatDatum(self, z))

        self.add_datum('input', datum.ExpressionDatum(self, "None"))

        for i in 'xmin','ymin','zmin','xmax','ymax','zmax':
            self.add_datum(i, datum.FloatFunctionDatum(self, i))

    def xmin(self): return self.input.xmin
    def xmax(self): return self.input.xmax
    def ymin(self): return self.input.ymin
    def ymax(self): return self.input.ymax
    def zmin(self): return self.input.zmin
    def zmax(self): return self.input.zmax

    def get_control(self):
        import control.bounds
        return control.bounds.GetBoundsControl


class SetBounds(base.Node):
    def __init__(self, name, x, y, z):
        super(SetBounds, self).__init__(name)

        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('z', datum.FloatDatum(self, z))

        self.add_datum('input', datum.ExpressionDatum(self, "None"))

        for i in 'xmin','ymin','zmin','xmax','ymax','zmax':
            self.add_datum(i, datum.FloatDatum(self, 0))

        self.add_datum('output', datum.ExpressionFunctionDatum(self, 'make_shape'))

    def make_shape(self):
        s = self.input
        for i in 'xmin','ymin','zmin','xmax','ymax','zmax':
            setattr(s, i, getattr(self, i))
        return s

    def get_control(self):
        import control.bounds
        return control.bounds.SetBoundsControl


