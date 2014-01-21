import math

import base
import datum

class Reposition(base.Node3D):
    def __init__(self, name, x, y, z):
        super(Reposition, self).__init__(name, x, y, z)
        self.add_datum('input', datum.ExpressionDatum(self, None))
        self.add_datum('shape',
                datum.ExpressionFunctionDatum(self, 'make_shape'))

    def get_control(self):
        import control.reposition
        return control.reposition.RepositionControl

    def make_shape(self):
        """ Moves the shape so that the bottom x,y,z corner is at this node.
        """
        shape = self.input
        if not shape.check():
            raise RuntimeError("Invalid shape!")
        elif (math.isinf(shape.xmin) or
              math.isinf(shape.ymin)):
            raise RuntimeError("Can't move shape with infinite bounds")

        return fab.shapes.move(shape,
                self.x - shape.xmin, self.y - shape.ymin,
                0 if math.isinf(shape.zmin) else self.z - shape.zmin)

import fab.shapes
