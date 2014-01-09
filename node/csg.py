import base
import datum

class Union(base.Node):
    def __init__(self, name, x, y, z):
        super(Union, self).__init__(name)

        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('z', datum.FloatDatum(self, z))

        self.add_datum('A', datum.ExpressionDatum(self, "'f1'"))
        self.add_datum('B', datum.ExpressionDatum(self, "'f1'"))

        self.add_datum('shape',
                        datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.csg
        return control.csg.UnionControl

    def make_shape(self):
        """ Computes the union of A and B
            (which are already auto-summing ExpressionDatum objects)
        """
        shape = self.A | self.B
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape

################################################################################

class Intersection(base.Node):
    def __init__(self, name, x, y, z):
        super(Intersection, self).__init__(name)

        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('z', datum.FloatDatum(self, z))

        self.add_datum('A', datum.ExpressionDatum(self, "'f1'"))
        self.add_datum('B', datum.ExpressionDatum(self, "'f1'"))

        self.add_datum('shape',
                        datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.csg
        return control.csg.Intersection

    def make_shape(self):
        """ Computes the intersection of A and B
            (which are lovely auto-summing ExpressionDatum objects)
        """
        shape = self.A & self.B
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape

################################################################################

class Cutout(base.Node):
    def __init__(self, name, x, y, z):
        super(Cutout, self).__init__(name)

        self.add_datum('x', datum.FloatDatum(self, x))
        self.add_datum('y', datum.FloatDatum(self, y))
        self.add_datum('z', datum.FloatDatum(self, z))

        self.add_datum('A', datum.ExpressionDatum(self, "'f1'"))
        self.add_datum('B', datum.ExpressionDatum(self, "'f1'"))

        self.add_datum('shape',
                        datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.csg
        return control.csg.Cutout

    def make_shape(self):
        """ Computes A & ~B
        """
        shape = self.A & ~self.B
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape
