import base
import colors

class ScriptNodeControl(base.TextLabelControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        s = ScriptNode(get_name('script'), x, y, z)
        return cls(canvas, s)

    def __init__(self, canvas, target):
        super(ScriptNodeControl, self).__init__(
                canvas, target, "Script", colors.green)
        self.editor_datums = ['name']

from node.script import ScriptNode
from node.base import get_name
