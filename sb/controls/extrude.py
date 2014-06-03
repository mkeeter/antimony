import math

from PySide import QtCore, QtGui

from sb.controls.control import DummyControl
from sb.controls.multiline import MultiLineControl

class _ExtrudeBase(MultiLineControl):
    def __init__(self, canvas, node, parent):
        super().__init__(canvas, node, parent)
        self.watch_datums('x','y','zmin','_scale')

    def _lines(self):
        x = self._cache['x']
        y = self._cache['y']
        z = self._cache['zmin']
        s = self._cache['_scale'] / 5

        return [
            [QtGui.QVector3D(x + s, y, z), QtGui.QVector3D(x - s, y, z)],
            [QtGui.QVector3D(x, y + s, z), QtGui.QVector3D(x, y - s, z)]]

    def drag(self, p, d):
        self._node.get_datum('x').increment(d.x())
        self._node.get_datum('y').increment(d.y())
        self._node.get_datum('zmin').increment(d.z())

class _ExtrudeTower(MultiLineControl):
    def __init__(self, canvas, node, parent):
        super().__init__(canvas, node, parent)
        self.watch_datums('x','y','zmin','zmax')

    def _lines(self):
        x = self._cache['x']
        y = self._cache['y']
        z0 = self._cache['zmin']
        z1 = self._cache['zmax']
        return [[QtGui.QVector3D(x, y, z0), QtGui.QVector3D(x, y, z1)]]

    def drag(self, p, d):
        self._node.get_datum('x').increment(d.x())
        self._node.get_datum('y').increment(d.y())
        self._node.get_datum('zmin').increment(d.z())
        self._node.get_datum('zmax').increment(d.z())

class _ExtrudeTop(MultiLineControl):

    def __init__(self, canvas, node, parent):
        super().__init__(canvas, node, parent)
        self.watch_datums('x','y','zmax','_scale')

    def _lines(self):
        x = self._cache['x']
        y = self._cache['y']
        z = self._cache['zmax']
        s = self._cache['_scale'] / 5

        return [
            [QtGui.QVector3D(x + s, y, z - s),
            QtGui.QVector3D(x, y, z),
             QtGui.QVector3D(x - s, y, z - s)],
            [QtGui.QVector3D(x, y + s, z - s),
             QtGui.QVector3D(x, y, z),
             QtGui.QVector3D(x, y - s, z - s)]]

    def drag(self, p, d):
        self._node.get_datum('zmax').increment(d.z())

class ExtrudeControl(DummyControl):
    def __init__(self, canvas, node):
        super().__init__(canvas, node)
        self._base = _ExtrudeBase(canvas, node, self)
        self._tower = _ExtrudeTower(canvas, node, self)
        self._top = _ExtrudeTop(canvas, node, self)
        self.watch_datums('x','y','zmin')

    def center_pos(self):
        pt = self.transform_points([QtGui.QVector3D(
            self._cache['x'],
            self._cache['y'],
            self._cache['zmin'])])[0]
        return pt.toPoint()

    def drag(self, p, d):
        self._base.drag(p, d)
        self._top.drag(p, d)
