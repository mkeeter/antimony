import base
import datum

class ScriptNode(base.Node3D):
    default = """import math

from fab.shapes import *
from fab.expression import Expression

c = circle(0, 0, 1)
output(c)
"""

    def __init__(self, name, x, y, z):
        super(ScriptNode, self).__init__(name, x, y, z)
        self.add_datum('script', datum.ScriptDatum(self, self.default))

    def get_control(self):
        import control.script
        return control.script.ScriptNodeControl

