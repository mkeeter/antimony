from PySide import QtCore, QtGui

from sb.ui.iobox import InputBox, OutputBox
from .ui_viewer import Ui_Viewer

class _DatumLineEdit(QtGui.QLineEdit):
    """ A class derived from QLineEdit used specifically for editing datums.
    """
    def __init__(self, datum, parent=None):
        super(_DatumLineEdit, self).__init__(parent)
        self._datum = datum

        if hasattr(self._datum, 'set_expr'):
            self.textEdited.connect(self._datum.set_expr)
        else:
            self.setEnabled(False)
        self.setText(self._datum.display_str())
        self._datum.changed.connect(self.on_datum_changed)
        self.on_datum_changed()

    def on_datum_changed(self):
        if self._datum.input_handler is not None:
            self.setEnabled(not bool(self._datum.input_handler))

        p = self.cursorPosition()
        self.setText(self._datum.display_str())
        self.setCursorPosition(p)

        if self._datum.valid:
            self.setStyleSheet("""
                QLineEdit:disabled { color: #ccc; }
                """)
        else:
            self.setStyleSheet("""
                QLineEdit { background-color: #faa; }
                QLineEdit:disabled { color: #fcc; }
            """)

################################################################################

class NodeViewer(QtGui.QWidget):

    io_pos_changed = QtCore.Signal()

    def __init__(self, control):
        """ control should be a sb.controls.control.Control instance
        """
        super(NodeViewer, self).__init__()
        self._control = control

        self.ui = Ui_Viewer()
        self.ui.setupUi(self)
        self._populate_grid(control._node)
        self.move(100, 100)

        self.ui.title.setText("<b>%s</b>" % type(control._node).__name__)
        self.ui.closeButton.pressed.connect(self.animate_close)

        # If the node has changed, reposition
        control.center_changed.connect(self.move)
        control.center_changed.connect(self._prepare_geometry_change)

        control.destroyed.connect(self.animate_close)
        self.io_pos_changed.connect(control.io_pos_changed)

        self.proxy = control._canvas.scene.addWidget(self)
        self.proxy.setZValue(-2)
        self.proxy.setFocusPolicy(QtCore.Qt.TabFocus)

        self._mask_size = 0
        self.animate_open()
        self.show()

    def _prepare_geometry_change(self, pt):
        self.proxy.prepareGeometryChange()

    def paintEvent(self, event):
        """ Override paintEvent so that the widget can be styled using qss.
        """
        o = QtGui.QStyleOption()
        o.initFrom(self)
        p = QtGui.QPainter(self)
        self.style().drawPrimitive(QtGui.QStyle.PE_Widget, o, p, self)

    def _populate_grid(self, node):
        """ Adds a set of grid rows with editor panels connected to datums.
        """
        for datum in node.datums:
            name = datum.name
            if name[0] == '_':
                continue
            row = self.ui.grid.rowCount()
            if datum.input_handler is not None:
                self.ui.grid.addWidget(InputBox(datum, self), row, 0)

            self.ui.grid.addWidget(QtGui.QLabel(name),
                                   row, 1, QtCore.Qt.AlignRight)
            self.ui.grid.addWidget(_DatumLineEdit(datum), row, 2)

            if datum.has_output:
                self.ui.grid.addWidget(OutputBox(datum, self), row, 3)

    def _set_mask(self, frac):
        """ Mask a certain percentage of the widget.
            (used in open/close animations).
        """
        self._mask_size = frac
        full = self.sizeHint()
        self.setMask(QtGui.QRegion(0, 0, full.width()*frac  + 1,
                                         full.height()*frac + 1))
        self.update()
        self.io_pos_changed.emit()

    def _get_mask(self):
        """ Find what fraction of the widget is masked
            (used in open/close animations).
        """
        return self._mask_size

    # Make mask_size a QProperty so that we can animate it automatically.
    mask_size = QtCore.Property(float, _get_mask, _set_mask)

    def animate_open(self):
        """ Animates the panel sliding open.
        """
        a = QtCore.QPropertyAnimation(self, "mask_size", self)
        a.setDuration(100)
        a.setStartValue(0)
        a.setEndValue(1)
        a.start(QtCore.QPropertyAnimation.DeleteWhenStopped)

    def animate_close(self):
        """ Animates the panel sliding closed.
            Calls self.Destroy when the panel is completely closed.
        """
        self.setFocus() # Take focus away from text entry box
                        # to prevent certain graphics artifacts
        a = QtCore.QPropertyAnimation(self, "mask_size", self)
        a.setDuration(100)
        a.setStartValue(1)
        a.setEndValue(0)
        a.finished.connect(self.deleteLater)
        a.start(QtCore.QPropertyAnimation.DeleteWhenStopped)

    def datum_input_box_at(self, p):
        """ Looks for an input box at the given position (in NodeViewer
            viewport coordinates).  Returns None if no such box is found.
        """
        for c in self.findChildren(InputBox):
            if c.geometry().contains(p):
                return c
        return None

    def datum_input_box(self, d):
        """ Finds an input box connected to the given datum.
            Returns None if no such box is found.
        """
        for c in self.findChildren(InputBox):
            if c._datum == d:   return c
        return None

    def datum_output_box(self, d):
        """ Finds an output box connected to the given datum.
            Returns None if no such box is found.
        """
        for c in self.findChildren(OutputBox):
            if c._datum == d:   return c
        return None
