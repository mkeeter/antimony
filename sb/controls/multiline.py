import operator
from functools import reduce

from PySide import QtCore, QtGui

from sb.controls.control import Control

class MultiLineControl(Control):
    """ A control that draws multiple lines in 3D space.
        Subclasses must define a function _lines() which returns
        a list of list of QVector3Ds (representing a set of lines).
    """
    def __init__(self, canvas, node, parent=None):
        super(MultiLineControl, self).__init__(canvas, node, parent)

    def boundingRect(self):
        return self.bounding_box(reduce(operator.add, self._lines()))

    def _path(self):
        """ Generates a QPainterPath from self._lines
        """
        path = QtGui.QPainterPath()
        lines = [self.transform_points(line) for line in self._lines()]
        for line in lines:
            path.moveTo(line[0])
            for pt in line[1:]:
                path.lineTo(pt)
        return path

    def shape(self):
        stroker = QtGui.QPainterPathStroker()
        stroker.setWidth(4)
        return stroker.createStroke(self._path())

    def paint(self, painter, options, widget):
        self.set_default_pen(painter)
        painter.drawPath(self._path())
