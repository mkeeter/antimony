from PySide import QtCore, QtGui
from sb.datum import Datum

class Control(QtGui.QGraphicsItem):
    def __init__(self, canvas):
        super(Control, self).__init__()
        self.canvas = canvas
        self.canvas.scene.addItem(self)
        self.canvas.rotated.connect(self.prepareGeometryChange)

        # Set _post_init to be called when control returns to the event loop
        # (which will happen after the superclass finishes its own __init__)
        self._post_init_timer = QtCore.QTimer()
        self._post_init_timer.singleShot(0, self._post_init)

    def _post_init(self):
        del self._post_init_timer
        for d in self.__dict__.itervalues():
            if isinstance(d, Datum):
                print d
                d.changed.connect(self.prepareGeometryChange)

    @property
    def matrix(self):
        return self.canvas.matrix

    def transform_points(self, points):
        """ Transforms a list of QVector3Ds (in world coordinates), returning
            a list of QPointFs in scene coordinates.
        """
        m = self.matrix
        t = [m * p for p in points]
        return [QtCore.QPointF(p.x(), p.y()) for p in t]

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
