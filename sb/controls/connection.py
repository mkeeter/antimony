from PySide import QtCore, QtGui

import sb.colors

class Connection(QtGui.QGraphicsObject):
    def __init__(self, output_datum, output_control,
                       input_datum=None, input_control=None):
        super(Connection, self).__init__()
        output_control._canvas.scene.addItem(self)

        self._canvas = output_control._canvas
        self._output_datum = output_datum
        self._output_control = output_control
        self._input_datum = input_datum
        self._input_control = input_control

        self._base_color = sb.colors.get_color(self._output_datum.data_type)
        self.color = self._base_color
        self.setZValue(-1)
        self.setFlags(QtGui.QGraphicsItem.ItemIsSelectable)

    @property
    def _start_pos(self):
        return self._output_control.datum_output_pos(self._output_datum)

    @property
    def _end_pos(self):
        if self._input_datum:
            return self._input_control.datum_input_pos(self._input_datum)
        else:
            return getattr(self, '_drag_pos', self._start_pos)

    def _connect_to(self, input_box):
        self._input_datum = input_box._datum
        self._input_datum.input_handler.connect(self._output_datum)
        self._input_control = input_box.parent()._control

        self._input_control.io_pos_changed.connect(
                self.prepareGeometryChange)
        self._output_control.io_pos_changed.connect(
                self.prepareGeometryChange)

        self.color = self._base_color

        self.prepareGeometryChange()
        self._input_datum.destroyed.connect(self.deleteLater)
        self._output_datum.destroyed.connect(self.deleteLater)

        self._input_datum.update()

    def delete_connection(self):
        self._input_datum.input_handler.disconnect(self._output_datum)
        self.deleteLater()

    def mouseReleaseEvent(self, event):
        if self._input_datum is None:
            b = self._canvas.datum_input_box_at(event.pos())
            if b is None or not b._datum.can_connect(self._output_datum):
                self.deleteLater()
            else:
                self._connect_to(b)
        elif event.button() == QtCore.Qt.LeftButton:
                self.setSelected(True)
        self.ungrabMouse()

    def mouseMoveEvent(self, event):
        if self._input_datum is not None:
            return
        self.prepareGeometryChange()

        self._drag_pos = event.pos()
        b = self._canvas.datum_input_box_at(event.pos())
        if b is None:
            self.setSelected(False)
        elif b._datum.can_connect(self._output_datum):
            self.setSelected(True)
        else:
            self.color = sb.colors.red


    def _path(self):
        """ Returns a smooth curve from the start to end position.
        """
        start = self._start_pos
        end = self._end_pos

        if end.x() > start.x():
            length = 50
        else:
            length = (start.x() - end.x())/2 + 50

        path = QtGui.QPainterPath()
        path.moveTo(start)
        path.cubicTo(start + QtCore.QPoint(length, 0),
                     end - QtCore.QPoint(length, 0), end)
        return path

    def boundingRect(self):
        return self._path().boundingRect()

    def paint(self, painter, options, widget):
        if self.isSelected():
            c = sb.colors.highlight(self._base_color)
        else:
            c = self._base_color

        painter.setPen(QtGui.QPen(QtGui.QColor(c), 4))
        painter.drawPath(self._path())

    def shape(self):
        stroker = QtGui.QPainterPathStroker()
        stroker.setWidth(4)
        return stroker.createStroke(self._path())

