import base
import datum

class ScriptNode(base.Node3D):
    def __init__(self, name, x, y, z):
        super(ScriptNode, self).__init__(name, x, y, z)
        self.add_datum('script', datum.ScriptDatum(self, 'print "hello"'))

    def get_control(self):
        import control.script
        return control.script.ScriptNodeControl

