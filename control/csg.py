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
        super(UnionControl, self).__init__(
                canvas, target, u"A ∪ B ∪ C...")

class IntersectionControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, scale):
        i = Intersection(get_name('i'), x, y)
        cls(canvas, i)

    def __init__(self, canvas, target):
        super(IntersectionControl, self).__init__(
                canvas, target, u"A ∩ B")

from node.base import get_name
from node.csg import Union, Intersection
