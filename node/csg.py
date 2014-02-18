import base
import datum

class Union(base.Node3D):
    def __init__(self, name, x, y, z):
        super(Union, self).__init__(name, x, y, z)

        self.add_datum('A', datum.ExpressionDatum(self, "None"))
        self.add_datum('B', datum.ExpressionDatum(self, "None"))

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

class Intersection(base.Node3D):
    def __init__(self, name, x, y, z):
        super(Intersection, self).__init__(name, x, y, z)

        self.add_datum('A', datum.ExpressionDatum(self, "None"))
        self.add_datum('B', datum.ExpressionDatum(self, "None"))

        self.add_datum('shape',
                        datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.csg
        return control.csg.IntersectionControl

    def make_shape(self):
        """ Computes the intersection of A and B
            (which are lovely auto-summing ExpressionDatum objects)
        """
        shape = self.A & self.B
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape

################################################################################

class Cutout(base.Node3D):
    def __init__(self, name, x, y, z):
        super(Cutout, self).__init__(name, x, y, z)

        self.add_datum('A', datum.ExpressionDatum(self, "None"))
        self.add_datum('B', datum.ExpressionDatum(self, "None"))

        self.add_datum('shape',
                        datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.csg
        return control.csg.CutoutControl

    def make_shape(self):
        """ Computes A & ~B
        """
        shape = self.A & ~self.B
        if not shape.check():
            raise RuntimeError("Constructed invalid shape!")
        return shape
