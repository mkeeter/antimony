from PySide import QtCore, QtGui

import point

class ColorNodeControl(point.Point3DControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Constructs a new point at the given pocation.
        """
        p = ColorNode(get_name('c'), x, y, z)
        return cls(canvas, p)

    def __init__(self, canvas, target):
        super(ColorNodeControl, self).__init__(canvas, target)
        self.editor_datums = ['name','input','R','G','B','shape']

    @property
    def color(self):
        return [self._cache[c]*255 for c in 'RGB']

    def paint(self, painter, mask=False):
        path = QtGui.QPainterPath()
        width, height = self.width(), self.height()

        if mask:                        d = 22.
        elif self.drag_control.active:  d = 20.
        else:                           d = 18.

        path.moveTo(width/2, 0)
        path.arcTo(width/2 - d/2, height - d - 2, d, d, 0., -180.)
        path.closeSubpath()

        self.set_brush(painter, mask, self.color)
        painter.drawPath(path)

    def reposition(self):
        """ Overload reposition so that it also re-renders this widget
            (necessary for color changing).
        """
        super(ColorNodeControl, self).reposition()
        self.update()

    def get_input_pos(self):
        return self.pos() + QtCore.QPoint(2, self.height() - 10)

    def get_output_pos(self):
        return self.pos() + QtCore.QPoint(self.width() - 2, self.height() - 10)

from node.base import get_name
from node.color import ColorNode
