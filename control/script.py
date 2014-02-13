import base
import colors

class ScriptNodeControl(base.TextLabelControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        s = ScriptNode(get_name('script'), x, y, z)
        return cls(canvas, s)

    def __init__(self, canvas, target):
        super(ScriptNodeControl, self).__init__(
                canvas, target, "Script", colors.green, "Courier")
        self.editor_datums = ['name']

    def cache(self):
        """ Updates datum values then calls the standard cache() function.
        """
        self.node.update_datums()
        return super(ScriptNodeControl, self).cache()

from node.script import ScriptNode
from node.base import get_name
