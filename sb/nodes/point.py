from sb.nodes.node import Node3D
from sb.controls.point import Point3DControl

class Point3D(Node3D):
    def __init__(self, name, x, y, z):
        super(Point3D, self).__init__(name, x, y, z)

    def make_controls(self, canvas):
        Point3DControl(
                canvas,
                self.object_datums['x'],
                self.object_datums['y'],
                self.object_datums['z'])
