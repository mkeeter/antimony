from PySide import QtCore, QtGui

class Editor(QtGui.QGroupBox):
    def __init__(self, control):
        super(Editor, self).__init__(control.canvas)

        self.node    = control.node
        self.control = control

        grid = QtGui.QGridLayout(self)
        grid.setSpacing(5)
        grid.setContentsMargins(0, 3, 0, 3)

        self.add_header(grid)
        self.lines = []
        for name, dat in self.node.datums:
            self.add_row(grid, name, dat)

        self.setLayout(grid)
        self.sync()

        self.animate_open()
        self.show()

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

    def add_row(self, grid, name, dat):
        """ Adds a datum row to the UI, appending a (text input box, datum)
            tuple to self.lines.
        """
        row = grid.rowCount()
        if not isinstance(dat, NameDatum):
            grid.addWidget(Input(dat, self), row, 0)

        grid.addWidget(QtGui.QLabel(name, self), row, 1, QtCore.Qt.AlignRight)

        txt = QtGui.QLineEdit(self)
        txt.textChanged.connect(lambda t: self.on_change(t, dat))
        grid.addWidget(txt, row, 2)

        if not isinstance(dat, NameDatum):
            grid.addWidget(Output(dat, self), row, 3)

        self.lines.append((txt, dat))

    def sync(self):
        """ Updates position, text, and text box highlighting.
        """

        for t, d in self.lines:
            ss = "QLineEdit:disabled { color: #bbb; }"
            if d.valid():
                t.setStyleSheet(ss)
            else:
                t.setStyleSheet("QLineEdit { background-color: #faa; }")
            e = d.get_expr()
            if e != t.text():
                t.setText(d.get_expr())
                t.setCursorPosition(0)
            t.setEnabled(d.can_edit())

        canvas = self.parentWidget()
        px, py = self.x(), self.y()
        try:    x = canvas.mm_to_pixel(x=self.node.x)
        except: x = px
        try:    y = canvas.mm_to_pixel(y=self.node.y)
        except: y = py

        self.move(x, y)

        for c in self.node.connections():
            c.control.sync()


    def on_change(self, value, dat):
        """ When a text box changes, update the corresponding Datum
            (which triggers a sync for self and self.control)
        """
        dat.set_expr(value)

    def set_mask(self, frac):
        """ Mask a certain percentage of the widget.
        """
        full = self.sizeHint()
        self.setMask(QtGui.QRegion(0, 0, full.width()*frac  + 1,
                                         full.height()*frac + 1))
    mask_size = QtCore.Property(float, lambda self: 0, set_mask)

    def animate_open(self):
        """ Animates the panel sliding open.
        """
        a = QtCore.QPropertyAnimation(self, "mask_size", self)
        a.setDuration(100)
        a.setStartValue(0)
        a.setEndValue(1)
        a.finished.connect(self.finished_open)
        a.start(QtCore.QPropertyAnimation.DeleteWhenStopped)

    def finished_open(self):
        self.control.editor = self
        self.sync()

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

        self.control.editor = None
        self.sync() # This will cause connections not to be drawn.

    def close(self):
        """ Disconnects this widget from the editor and deletes it.
        """
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
from node.datum import NameDatum
