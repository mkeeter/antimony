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
        self.position = QtCore.QPointF()

        self.make_mask()
        self.sync()
        self.show()
        self.raise_()


    def _sync(self):
        """ Move this control to the appropriate position.
            Use self.position (cached) if eval fails.
        """
        p = QtCore.QPointF(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y())

        changed = (self.position != p)
        self.position = p

        return changed

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

        if mask:
            painter.setBrush(QtGui.QBrush(QtCore.Qt.color1))
            painter.setPen(QtGui.QPen(QtCore.Qt.color1, 2))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(*colors.light_grey)))
            painter.setPen(QtGui.QPen(QtGui.QColor(*colors.dark_grey), 2))

        if mask:
            d = 22
        elif self.drag_control.hover or self.drag_control.drag:
            d = 20
        else:
            d = 14

        painter.drawEllipse((width - d) / 2, (height - d) / 2, d, d)

    def make_mask(self):
        """ Render a mask and set it to this widget's mask.
        """
        painter = QtGui.QPainter()
        bitmap = QtGui.QBitmap(self.size())
        bitmap.clear()

        painter.begin(bitmap)
        self.paint(painter, True)
        painter.end()

        self.drag_control.mask = QtGui.QRegion(bitmap)

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
        self.position = QtGui.QVector3D()

        super(PointControl, self).__init__(canvas, target)
        self.setFixedSize(30, 30)

        self.drag_control = base.DragXYZ(self)
        self.editor_datums = ['name','x','y','z']
        self.position = QtGui.QVector3D()


        self.make_mask()
        self.sync()
        self.show()
        self.raise_()


    def _sync(self):
        """ Move this control to the appropriate position.
            Use self.position (cached) if eval fails.
        """
        v = QtGui.QVector3D(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y(),
                self.node.z if self.node._z.valid() else self.position.z())

        changed = (self.position != v)
        self.position = v

        return changed

    def reposition(self):
        self.move(self.canvas.unit_to_pixel(self.position) -
                  QtCore.QPoint(self.width(), self.height())/2)


from node.base import get_name
from node.point import Point, Point3D
