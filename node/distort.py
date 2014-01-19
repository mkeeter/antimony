import base
import datum

class Repel(base.Node3D):
    def __init__(self, name, x, y, z, r):
        super(Repel, self).__init__(name, x, y, z)
        self.add_datum('r', datum.FloatDatum(self, r))

        self.add_datum('input',
                datum.ExpressionDatum(self, 'None'))
        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.distort
        return control.distort.RepelControl

    def make_shape(self):
        return fab.shapes.repel(self.input, self.x, self.y, self.z, self.r)

class Attract(base.Node3D):
    def __init__(self, name, x, y, z, r):
        super(Attract, self).__init__(name, x, y, z)
        self.add_datum('r', datum.FloatDatum(self, r))

        self.add_datum('input',
                datum.ExpressionDatum(self, 'None'))
        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.distort
        return control.distort.AttractControl

    def make_shape(self):
        return fab.shapes.attract(self.input, self.x, self.y, self.z, self.r)

import fab.shapes

