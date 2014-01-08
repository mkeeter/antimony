from PySide import QtCore, QtGui

import base
import colors

class GetBoundsControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        b = GetBounds(get_name('b'), x, y, z)
        cls(canvas, b)

    def __init__(self, canvas, target):
        super(GetBoundsControl, self).__init__(
                canvas, target, "Get bounds")
        self.editor_datums = ['name','input','xmin','xmax',
                              'ymin','ymax','zmin','zmax']

class SetBoundsControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        b = SetBounds(get_name('b'), x, y, z)
        cls(canvas, b)

    def __init__(self, canvas, target):
        super(SetBoundsControl, self).__init__(
                canvas, target, "Set bounds")
        self.editor_datums = ['name','input','xmin','xmax',
                              'ymin','ymax', 'zmin','zmax', 'output']

from node.base import get_name
from node.bounds import GetBounds, SetBounds
