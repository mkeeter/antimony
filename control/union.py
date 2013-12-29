from PySide import QtCore, QtGui

import base

class UnionControl(base.DraggableNodeControl):
    @classmethod
    def new(cls, canvas, x, y, scale):
        u = Union(get_name('u'), x, y)
        cls(canvas, u)

    def __init__(self, canvas, target):
        super(UnionControl, self).__init__(canvas, target)

        self.sync()
        self.show()
        self.raise_()


from node.base import get_name
from node.union import Union
