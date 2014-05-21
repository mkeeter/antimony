from PySide import QtCore, QtGui

from ui_viewer import Ui_Viewer

class _DatumLineEdit(QtGui.QLineEdit):
    """ A class derived from QLineEdit used specifically for editing datums.
    """
    def __init__(self, datum, parent=None):
        super(_DatumLineEdit, self).__init__(parent)
        self._datum = datum
        self.textEdited.connect(self._datum.set_expr)
        self.setText(self._datum.display_str())
        self._datum.changed.connect(self.on_datum_changed)

    def on_datum_changed(self):
        self.setText(self._datum.display_str())
        if self._datum.valid:
            self.setStyleSheet("")
        else:
            self.setStyleSheet("QLineEdit { background-color: #faa; }")

################################################################################

class NodeViewer(QtGui.QWidget):
    def __init__(self, control):
        """ control should be a sb.controls.control.Control instance
        """
        super(NodeViewer, self).__init__()
        self.ui = Ui_Viewer()
        self.ui.setupUi(self)
        self._populate_grid(control._node)
        self.move(100, 100)

        self.ui.title.setText("<b>%s</b>" % type(control._node).__name__)
        self.ui.closeButton.pressed.connect(self.animate_close)

        # If the node has changed, reposition based on the
        # control's viewer_position() function
        control.center_changed.connect(self.move)

        proxy = control._canvas.scene.addWidget(self)
        proxy.setZValue(-1)

        self._mask_size = 0
        self.animate_open()
        self.show()

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
        for name, datum in node.object_datums.items():
            row = self.ui.grid.rowCount()
            self.ui.grid.addWidget(QtGui.QLabel(name),
                                   row, 1, QtCore.Qt.AlignRight)
            self.ui.grid.addWidget(_DatumLineEdit(datum), row, 2)

    def _set_mask(self, frac):
        """ Mask a certain percentage of the widget.
            (used in open/close animations).
        """
        self._mask_size = frac
        full = self.sizeHint()
        self.setMask(QtGui.QRegion(0, 0, full.width()*frac  + 1,
                                         full.height()*frac + 1))
        self.update()

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
