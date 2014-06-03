from PySide import QtCore, QtGui

from sb.datum import Datum
import sb.colors

class Control(QtGui.QGraphicsObject):

    """ Signal emitted when center (for editor) changes. """
    center_changed = QtCore.Signal(QtCore.QPoint)

    """ Signal called when position for IO connections changes. """
    io_pos_changed = QtCore.Signal()

    def __init__(self, canvas, node=None, parent=None):
        super(Control, self).__init__(parent)

        self.setFlags(QtGui.QGraphicsItem.ItemIsSelectable |
                      QtGui.QGraphicsItem.ItemIgnoresTransformations)
        self.setAcceptHoverEvents(True)

        self._canvas = canvas
        if parent is None:
            self._canvas.scene.addItem(self)

        self._hover = False
        self._dragged = False

        if parent is not None:
            canvas.zoomed.connect(self.prepareGeometryChange)
            canvas.rotated.connect(self.prepareGeometryChange)

        self.viewer = None

        if node is not None and parent is None:
            # Where there's a node, there could be a node viewer, so
            # update the center when any view parameters change.
            canvas.rotated.connect(self.update_center)
            canvas.zoomed.connect(self.update_center)
            canvas.panned.connect(self.update_center)

            # Finally, when the node is destroyed, delete ourself.
            node.destroyed.connect(self.deleteLater)

        self.center_changed.connect(self.io_pos_changed)

        self._cache = {}
        self._node = node

    def delete_node(self):
        self._node.deleteLater()

    def watch_datums(self, *args):
        """ Marks a list of datums (by name) as ones to be watched,
            connected their changed signal to update_cache.
        """
        for name in args:
            d = self._node.get_datum(name)
            d.changed.connect(self.update_cache)
            self._cache[d.name] = d._value

    def update_cache(self, d, value, valid):
        self._cache[d.name] = value
        self.update_center()

    def _viewer_destroyed(self):
        self.viewer = None

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
        if self.parentObject():
            return self.parentObject().mouseDoubleClickEvent(event)
        elif not self.viewer:
            self.viewer = NodeViewer(self)
            self.update_center()

    def mousePressEvent(self, event):
        """ Saves a mouse click position (in scene coordinates) to
            self._mouse_click_pos
        """
        self._dragged = False
        self._mouse_click_pos = event.pos()

    def mouseReleaseEvent(self, event):
        """ On mouse release, call ungrabMouse (only matters after initial
            construction, when the control called grabMouse, but does no
            harm here).
        """
        if not self._dragged and event.button() == QtCore.Qt.LeftButton:
            self.setSelected(True)
        self.ungrabMouse()

    def mouseMoveEvent(self, event):
        """ On mouse move, call self.drag with the given drag
            (in world coordinates).
        """
        self.drag(*self.itransform_points([
            event.pos(),
            event.pos() - self._mouse_click_pos]))
        self._mouse_click_pos = event.pos()
        self._dragged = True

    def set_default_pen(self, painter):
        if self.isSelected() or self._hover:
            painter.setPen(QtGui.QPen(QtGui.QColor(sb.colors.base3), 2))
        else:
            painter.setPen(QtGui.QPen(QtGui.QColor(sb.colors.base3_d), 2))

    def set_default_brush(self, painter):
        if self.isSelected() or self._hover:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(sb.colors.base1_h)))
        else:
            painter.setBrush(QtGui.QBrush(QtGui.QColor(sb.colors.base1)))

    def datum_input_pos(self, d):
        """ Returns a position (in scene coordinates) where inputs to the
            given datum should be connected.
        """
        a = self.center_pos()
        if self.viewer is not None:
            b = self.viewer.mapToParent(
                    self.viewer.datum_input_box(d).geometry().center())
            m = self.viewer.mask_size
            return a * (1 - m) + b * m
        else:
            return a

    def datum_output_pos(self, d):
        """ Returns a position (in scene coordinates) where outputs to the
            given datum should be connected.
        """
        a = self.center_pos()
        if self.viewer is not None:
            b = self.viewer.mapToParent(
                    self.viewer.datum_output_box(d).geometry().center())
            m = self.viewer.mask_size
            return a * (1 - m) + b * m
        else:
            return a

    def update_center(self):
        """ Recalculates viewport coordinates where the node viewer should be
            positioned, then emits center_changed with that position.
        """
        self.prepareGeometryChange()
        self.center_changed.emit(self.center_pos())

################################################################################

class DummyControl(Control):
    """ Represents a control that cannot be drawn or selected.
    """

    def boundingRect(self):
        return QtCore.QRectF()

    def paint(self, painter, widget, object):
        return

    def shape(self):
        """ By default, controls have no selection region.
            It's up to subclasses to define this function.
        """
        return QtGui.QPainterPath()
from sb.ui.viewer import NodeViewer
