from PySide import QtCore, QtGui

import base
import colors

class PointControl(base.NodeControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Construct a new point at the given location
            Location should be specified in units.
        """
        p = Point(get_name('p'), x, y)
        return cls(canvas, p)

    def __init__(self, canvas, target):
        super(PointControl, self).__init__(canvas, target)
        self.setFixedSize(30, 30)

        self.drag_control = base.DragXY(self)

        self.editor_datums = ['name','x','y']

        self.make_mask()
        self.sync()
        self.show()
        self.raise_()

    @property
    def position(self):
        return QtCore.QPointF(self._cache['x'], self._cache['y'])


    def reposition(self):
        self.move(self.canvas.unit_to_pixel(self.position) -
                  QtCore.QPoint(self.width(), self.height())/2)

    def paintEvent(self, paintEvent):
        """ On paint event, paint oneself.
        """
        self.paint(QtGui.QPainter(self))

    def paint(self, painter, mask=False):
        """ Paint either the point or a mask for the point.
        """
        width, height = self.width(), self.height()

        self.set_brush(painter, mask, colors.dark_grey)

        if mask:                                                d = 22
        elif self.drag_control.hover or self.drag_control.drag: d = 20
        else:                                                   d = 14

        painter.drawEllipse((width - d) / 2, (height - d) / 2, d, d)

    def make_mask(self):
        """ Render a mask and set it to this widget's mask.
        """
        self.drag_control.mask = self.paint_mask(self.paint)


    def get_input_pos(self):
        """ Returns a position to which we should attach input wires.
        """
        return self.geometry().center() - QtCore.QPoint(12, 0)

    def get_output_pos(self):
        """ Returns a position to which we should attach input wires.
        """
        return self.geometry().center() + QtCore.QPoint(12, 0)


################################################################################

class Point3DControl(PointControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        """ Construct a new point at the given location
            Location should be specified in units.
        """
        p = Point3D(get_name('p'), x, y, z)
        return cls(canvas, p)

    def __init__(self, canvas, target):
        super(PointControl, self).__init__(canvas, target)
        self.setFixedSize(30, 30)

        self.drag_control = base.DragXYZ(self)
        self.editor_datums = ['name','x','y','z']

        self.make_mask()
        self.sync()
        self.show()
        self.raise_()


    @property
    def position(self):
        return QtGui.QVector3D(self._cache['x'],
                               self._cache['y'],
                               self._cache['z'])


    def reposition(self):
        self.move(self.canvas.unit_to_pixel(self.position) -
                  QtCore.QPoint(self.width(), self.height())/2)


from node.base import get_name
from node.point import Point, Point3D
