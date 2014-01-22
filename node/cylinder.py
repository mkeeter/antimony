import datum
import circle

class Cylinder(circle.Circle):
    def __init__(self, name, x, y, z, r, h):
        super(Cylinder, self).__init__(name, x, y, r)
        self.add_datum('z', datum.FloatDatum(self, z))
        self.add_datum('h', datum.FloatDatum(self, h))

    def get_control(self):
        import control.cylinder
        return control.cylinder.CylinderControl

    def make_shape(self):
        return fab.shapes.cylinder(
                self.x, self.y, min(self.z, self.z + self.h),
                max(self.z, self.z + self.h), self.r)

import fab.shapes
