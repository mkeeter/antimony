# coding=utf-8

from PySide import QtCore, QtGui

import base
import colors

class UnionControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, scale):
        u = Union(get_name('u'), x, y)
        cls(canvas, u)

    def __init__(self, canvas, target):
        super(UnionControl, self).__init__(canvas, target, u"A ∪ B ∪ C...")

from node.base import get_name
from node.union import Union
