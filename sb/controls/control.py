from PySide import QtCore, QtGui
from sb.datum import Datum

class Control(QtGui.QGraphicsItem, QtCore.QObject):

    center_changed = QtCore.Signal(QtCore.QPoint)

    def __init__(self, canvas, node=None):
        QtGui.QGraphicsItem.__init__(self)
        QtCore.QObject.__init__(self)

        self.setFlags(QtGui.QGraphicsItem.ItemIgnoresTransformations)
        self.setAcceptHoverEvents(True)

        self._canvas = canvas
        self._canvas.scene.addItem(self)
        self._canvas.rotated.connect(self.prepareGeometryChange)

        self._hover = False

        self._node = node
        if self._node is not None:
            self._node.changed.connect(self.update_center)
            self._node.changed.connect(self.prepareGeometryChange)
            self._node.changed.connect(self.update)
            self._node.changed.connect(self._canvas.update)
            # something something deletion

    @property
    def matrix(self):
        return self._canvas.matrix

    def transform_points(self, points):
        """ Transforms a list of QVector3Ds (in world coordinates), returning
            a list of QPointFs in scene coordinates.
        """
        m = self.matrix
        t = [m * p for p in points]
        return [QtCore.QPointF(p.x(), p.y()) for p in t]

    def itransform_points(self, points):
        """ Transforms a list of QPointFs in scene coordinates, returning
            a list of QVector3Ds in world coordinates.
        """
        m = self.matrix.inverted()[0]
        return [m * QtGui.QVector3D(t) for t in points]

    def bounding_box(self, points, padding=10):
        """ Returns a QRectF representing the bounding box (in scene coordinates)
            of the given set of points (which are in world coordinates).
        """
        pts = self.transform_points(points)
        xmin = min(p.x() for p in pts)
        ymin = min(p.y() for p in pts)
        xmax = max(p.x() for p in pts)
        ymax = max(p.y() for p in pts)
        return QtCore.QRectF(xmin - padding, ymin - padding,
                             xmax - xmin + 2*padding, ymax - ymin + 2*padding)

    def hoverEnterEvent(self, event):
        if not self._hover:
            self._hover = True
            self.update()

    def hoverLeaveEvent(self, event):
        if self._hover:
            self._hover = False
            self.update()

    def mouseDoubleClickEvent(self, event):
        if self._node is not None:
            NodeViewer(self)
            # Force a recalculation to position the NodeViewer at the right
            # place (since this will cause the control to emit center_changed)
            self._node.changed.emit()


from sb.ui.viewer import NodeViewer
