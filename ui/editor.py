from PySide import QtCore, QtGui

class Editor(QtGui.QGroupBox):
    def __init__(self, control):
        super(Editor, self).__init__(control.canvas)

        self.node    = control.node
        self.control = control

        self.grid = QtGui.QGridLayout()
        self.grid.setSpacing(5)
        self.grid.setContentsMargins(0, 3, 0, 3)

        self.populate_grid()

        self.setLayout(self.grid)

        self.sync()

        self._mask_size = 0
        self.animate_open()
        self.show()


    def populate_grid(self):
        self.add_header(self.grid)
        self.lines = []
        for name in self.control.editor_datums:
            self.add_row(name, getattr(self.node, '_'+name))

        if isinstance(self.control.node, ScriptNode):
            button = QtGui.QPushButton('[ open script ]', self)
            button.setFlat(True)
            button.clicked.connect(lambda:
                    QtGui.QApplication.instance().script.open(
                        self.control.node._script))
            self.grid.addWidget(button, self.grid.rowCount(), 2)

    def clear_grid(self):
        """ Deletes all widgets from the grid.
        """
        while not self.grid.isEmpty():
            self.grid.takeAt(0).widget().deleteLater()

    def regenerate_grid(self):
        self.hide() # hide/show are needed to update widget size.
        self.clear_grid()
        self.populate_grid()
        self.set_mask(1)
        self.show()

    def mousePressEvent(self, event):
        """ On mouse click, raise this window.
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.raise_()
            self.update()


    def mouseDoubleClickEvent(self, event):
        """ On mouse double-click, lower this window.
        """
        if event.button() == QtCore.Qt.LeftButton:
            self.lower()
            self.update()


    def raise_(self):
        """ Overload raise_ so that controls stay above editors.
        """
        super(Editor, self).raise_()
        self.control.raise_()


    def add_header(self, grid):
        """ Adds a header to the UI, including a push button to close
            and this node's type.
        """
        button = QtGui.QPushButton('[ - ]', self)
        button.setFlat(True)
        button.clicked.connect(self.animate_close)
        grid.addWidget(button, 0, 1)

        label = type(self.node).__name__
        grid.addWidget(QtGui.QLabel("<b>%s</b>" % label, self), 0, 2)

    def add_row(self, name, dat):
        """ Adds a datum row to the UI, appending a (text input box, datum)
            tuple to self.lines.
        """
        row = self.grid.rowCount()
        if (not isinstance(dat, NameDatum) and
            not isinstance(dat, FunctionDatum) and
            not isinstance(dat, OutputDatum)):
            self.grid.addWidget(Input(dat, self), row, 0)

        self.grid.addWidget(QtGui.QLabel(name, self),
                            row, 1, QtCore.Qt.AlignRight)
        txt = QtGui.QLineEdit(self)
        txt.textChanged.connect(lambda t: self.on_change(t, dat))
        self.grid.addWidget(txt, row, 2)
        self.lines.append((txt, dat))

        if not isinstance(dat, NameDatum):
            self.grid.addWidget(Output(dat, self), row, 3)


    def sync(self):
        """ Updates position, text, and text box highlighting.
        """

        ss_valid = """
                QLineEdit:disabled { color: #bbb; }"""
        ss_invalid  = """
                QLineEdit { background-color: #faa; }
                QLineEdit:disabled { color: #fcc; }
                QLabel { background-color: #faa; }"""

        for t, d in self.lines:
            if d.valid():   t.setStyleSheet(ss_valid)
            else:           t.setStyleSheet(ss_invalid)

            if isinstance(t, QtGui.QLineEdit):
                e = d.get_expr()
                if e != t.text():
                    t.setText(d.get_expr())
                    t.setCursorPosition(0)
                t.setEnabled(d.can_edit())

        self.move(self.control.editor_position())


    def on_change(self, value, dat):
        """ When a text box changes, update the corresponding Datum
            (which triggers a sync for self and self.control)
        """
        if dat.can_edit():  dat.set_expr(value)

    def set_mask(self, frac):
        """ Mask a certain percentage of the widget.
            (used in open/close animations).
        """
        self._mask_size = frac
        full = self.sizeHint()
        self.setMask(QtGui.QRegion(0, 0, full.width()*frac  + 1,
                                         full.height()*frac + 1))
        # Sync the control (used to adjust connections)
        self.control.sync()

    def get_mask(self):
        """ Find what fraction of the widget is masked
            (used in open/close animations).
        """
        return self._mask_size

    # Store this as a QProperty so that we can animate it automatically.
    mask_size = QtCore.Property(float, get_mask, set_mask)

    def animate_open(self):
        """ Animates the panel sliding open.
        """
        a = QtCore.QPropertyAnimation(self, "mask_size", self)
        a.setDuration(100)
        a.setStartValue(0)
        a.setEndValue(1)
        self.control.editor = self
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
        a.finished.connect(self.close)
        a.start(QtCore.QPropertyAnimation.DeleteWhenStopped)

    def close(self):
        """ Disconnects this widget from the editor and deletes it.
        """
        self.control.editor = None
        self.sync()
        self.deleteLater()

    def get_datum_output(self, dat):
        """ For a given datum, returns its output connector position
            (in canvas pixels)
        """
        o = [io for io in self.findChildren(Output)
                       if io.datum == dat][0]
        return o.geometry().center() + o.parentWidget().pos()


    def get_datum_input(self, dat):
        """ For a given datum, returns its input connector position
            (in canvas pixels)
        """
        i = [io for io in self.findChildren(Input)
                       if io.datum == dat][0]
        return i.geometry().center() + i.parentWidget().pos()


    def find_input(self, pos):
        """ Searches among children for io.Input controls that
            the mouse cursor hits.  Returns None if none are found.
        """
        for c in self.findChildren(Input):
            if c.mouse_hit(pos):    return c
        return None

_editors = {}

def MakeEditor(control):
    ed = _editors.get(type(control.node), Editor)(control)
    control.raise_()
    return ed


from control.connection import Input, Output
from node.datum import NameDatum, FunctionDatum, OutputDatum
from node.script import ScriptNode
