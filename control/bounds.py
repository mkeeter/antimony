from PySide import QtCore, QtGui

import base
import colors

class GetBoundsControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, scale):
        b = GetBounds(get_name('b'), x, y)
        cls(canvas, b)

    def __init__(self, canvas, target):
        super(GetBoundsControl, self).__init__(
                canvas, target, "Get bounds")

class SetBoundsControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, scale):
        b = SetBounds(get_name('b'), x, y)
        cls(canvas, b)

    def __init__(self, canvas, target):
        super(SetBoundsControl, self).__init__(
                canvas, target, "Set bounds")

from node.base import get_name
from node.bounds import GetBounds, SetBounds
