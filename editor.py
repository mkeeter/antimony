from PySide import QtCore, QtGui

import connector
import node

class Editor(QtGui.QGroupBox):
    def __init__(self, control):
        super(Editor, self).__init__(control.canvas)

        self.node    = control.node
        self.control = control

        grid = QtGui.QGridLayout(self)
        grid.setSpacing(5)
        grid.setContentsMargins(2, 2, 2, 2)

        self.add_header(grid)
        self.lines = []
        for name, datum in self.node.inputs:
            self.add_row(grid, name, datum)

        self.setLayout(grid)
        self.sync()
        self.show()

    def add_header(self, grid):
        button = QtGui.QPushButton('[ - ]', self)
        button.setFlat(True)
        button.clicked.connect(self.deleteLater)
        grid.addWidget(button, 0, 1)

        label = type(self.node).__name__
        grid.addWidget(QtGui.QLabel("<b>%s</b>" % label, self), 0, 2)

    def add_row(self, grid, name, datum):
        row = grid.rowCount()
        grid.addWidget(QtGui.QLabel(name, self), row, 1, QtCore.Qt.AlignRight)

        txt = QtGui.QLineEdit(self)
        txt.textChanged.connect(lambda t: self.on_change(t, datum))
        grid.addWidget(txt, row, 2)

        self.lines.append((txt, datum))

    def sync(self):
        """ Updates position, text, and text box highlighting.
        """
        for t, d in self.lines:
            if d.valid():
                t.setStyleSheet("")
            else:
                t.setStyleSheet("QLineEdit { background-color: #faa; }")
            t.setText(d.get_expr())

    def on_change(self, value, datum):
        """ When a text box changes, update the corresponding Datum
            (which triggers a sync for self and self.control)
        """
        datum.set_expr(value)

'''
        # Add a button to close the window
        txt = wx.StaticText(self, label='[-]')
        sizer.Add(txt, border=5, flag=wx.EXPAND|wx.TOP)
        txt.Bind(wx.EVT_MOTION, self.hand_cursor)
        txt.Bind(wx.EVT_LEAVE_WINDOW, self.mouse_cursor)
        txt.Bind(wx.EVT_LEFT_UP, self.start_close)

        label = type(target).__name__
        txt = wx.StaticText(self, label=label, size=(-1, 25),
                            style=wx.ST_NO_AUTORESIZE)
        txt.SetFont(wx.Font(14, family=wx.FONTFAMILY_DEFAULT,
                         style=wx.ITALIC, weight=wx.BOLD))
        sizer.Add(txt, border=5, flag=wx.TOP|wx.RIGHT|wx.EXPAND)

        sizer.Add(wx.Panel(self))'''


'''
class Editor(wx.Window):
    def __init__(self, control):
        super(Editor, self).__init__(control.canvas)

        self.target = control.node
        self.control = control

        sizer = wx.FlexGridSizer(rows=len(self.target.inputs) + 1, cols=4)

        self.add_header(sizer, self.target)

        # Store datum editors and connections
        self.txtctrls = []
        self.io_inputs = []
        self.io_outputs = []

        for n, d in self.target.inputs:
            self.add_row(sizer, n, d)

        self.SetBackgroundColour((200, 200, 200))
        self.SetSizerAndFit(sizer)

        self.native_size = self.Size
        self.Size = (0, 0)
        self.animate(self.animate_open)

        # Run through these functions once to put the editor in
        # the right place and make text correct.
        self.update()
        self.sync_text()
        self.check_datums()

    def animate_open(self, event):
        """ Animates the panel sliding open (takes 5 frames)
            Halts the timer when the panel is completely open.
        """
        f = self.timer.tick / 5.0
        self.timer.tick += 1
        self.Size = (self.native_size.x * f,
                     self.native_size.y * f)
        if f == 1:
            self.timer.Stop()
            self.timer = None

    def animate_close(self, event):
        """ Animates the panel sliding closed.
            Calls self.Destroy when the panel is completely closed.
        """
        f = 1 - self.timer.tick / 5.0
        self.timer.tick += 1
        self.Size = (self.native_size.x * f,
                     self.native_size.y * f)
        if f == 0:
            self.timer.Stop()
            self.timer = None
            self.mouse_cursor()
            wx.CallAfter(self.Destroy)

    def animate(self, callback):
        """ Starts a timer that calls the given callback every 10 ms.
        """
        self.timer = wx.Timer()
        self.timer.Bind(wx.EVT_TIMER, callback)
        self.timer.Start(10)
        self.timer.tick = 0

    def start_close(self, event=None):
        """ Starts the closing animation.
        """
        self.animate(self.animate_close)

    def hand_cursor(self, event=None):
        """ Sets the mouse cursor to a hand.
        """
        wx.SetCursor(wx.StockCursor(wx.CURSOR_HAND))

    def mouse_cursor(self, event=None):
        """ Sets the mouse cursor to the standard cursor.
        """
        wx.SetCursor(wx.StockCursor(wx.CURSOR_ARROW))

    def add_header(self, sizer, target):
        """ Adds a title header.
        """
        sizer.Add(wx.Panel(self))

        # Add a button to close the window
        txt = wx.StaticText(self, label='[-]')
        sizer.Add(txt, border=5, flag=wx.EXPAND|wx.TOP)
        txt.Bind(wx.EVT_MOTION, self.hand_cursor)
        txt.Bind(wx.EVT_LEAVE_WINDOW, self.mouse_cursor)
        txt.Bind(wx.EVT_LEFT_UP, self.start_close)

        label = type(target).__name__
        txt = wx.StaticText(self, label=label, size=(-1, 25),
                            style=wx.ST_NO_AUTORESIZE)
        txt.SetFont(wx.Font(14, family=wx.FONTFAMILY_DEFAULT,
                         style=wx.ITALIC, weight=wx.BOLD))
        sizer.Add(txt, border=5, flag=wx.TOP|wx.RIGHT|wx.EXPAND)

        sizer.Add(wx.Panel(self))


    def add_row(self, sizer, name, dat):
        """ Adds a row with a particular datum.
        """
        i = io.Input(self, dat)
        self.io_inputs.append(i)
        sizer.Add(i, border=3, flag=wx.BOTTOM|wx.TOP|wx.RIGHT|wx.ALIGN_CENTER)

        sizer.Add(wx.StaticText(self, label=name,
                                style=wx.ALIGN_RIGHT|wx.ST_NO_AUTORESIZE,
                                size=(-1, 25)),
                  border=3, flag=wx.ALL|wx.EXPAND)

        txt = wx.TextCtrl(self, size=(150, 25),
                          style=wx.NO_BORDER|wx.TE_PROCESS_ENTER)
        txt.datum = dat

        txt.Bind(wx.EVT_TEXT, self.on_change)
        sizer.Add(txt, border=3, flag=wx.ALL|wx.EXPAND)
        self.txtctrls.append(txt)

        o = io.Output(self, dat)
        self.io_outputs.append(o)
        sizer.Add(o, border=3, flag=wx.BOTTOM|wx.TOP|wx.LEFT|wx.ALIGN_CENTER)


    def on_change(self, event):
        """ When a text box changes, update the corresponding Datum
            and trigger an update for self and self.control
        """
        txt = event.GetEventObject()

        # Set datum expression to text value
        # (this automatically updates children)
        txt.datum.set_expr(txt.GetValue())

    def check_datums(self):
        """ Check all datums for validity and change text color if invalid.
        """
        for txt in self.txtctrls:
            if txt.datum.valid():
                txt.SetForegroundColour(wx.NullColour)
            else:
                txt.SetForegroundColour(wx.Colour(255, 0, 0))


    def sync_text(self):
        """ Update the text fields to reflect the underlying datums.
        """
        for txt in self.txtctrls:
            txt.SetValue(txt.datum.get_expr())

    def get_datum_output(self, datum):
        """ For a given datum, returns the io.Output object.
        """
        return [io for io in self.io_outputs if io.datum == datum][0]

    def get_datum_input(self, datum):
        """ For a given datum, returns the io.Input object.
        """
        return [io for io in self.io_inputs if io.datum == datum][0]

    def update(self):
        """ Move this panel to the appropriate position and zoom as needed.
        """
        time = 5

        px, py = self.GetPosition()
        try:    x = self.Parent.mm_to_pixel(x=self.target.x)
        except: x = px

        try:    y = self.Parent.mm_to_pixel(y=self.target.y)
        except: y = py

        if x != px or y != py:  self.MoveXY(x, y)

        self.check_datums()

    def find_input(self, pos):
        """ Searches among children for io.Input controls that
            the mouse cursor hits.  Returns None if none are found.
        """
        for c in self.Children:
            if isinstance(c, io.Input) and c.mouse_hit(pos):
                return c
        return None


_editors = {}

def MakeEditor(control):
    return _editors.get(type(control.node), Editor)(control)'''
