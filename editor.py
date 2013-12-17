import wx

import io
import node
import datum

class Editor(wx.Panel):
    def __init__(self, parent, target):
        wx.Panel.__init__(self, parent)

        self.target = target

        sizer = wx.FlexGridSizer(rows=len(target.inputs) + 1, cols=4)

        self.add_header(sizer, target)

        self.data = []
        for n, d in target.inputs:
            self.add_row(sizer, n, d)

        self.SetBackgroundColour((200, 200, 200))
        self.SetSizerAndFit(sizer)

        # Check that variables are valid before painting.
        self.Bind(wx.EVT_PAINT, self.predraw)

        self.native_size = self.Size
        self.expand = 0

    def hand_cursor(self, event=None):
        wx.SetCursor(wx.StockCursor(wx.CURSOR_HAND))

    def mouse_cursor(self, event=None):
        wx.SetCursor(wx.StockCursor(wx.CURSOR_ARROW))

    def add_header(self, sizer, target):
        sizer.Add(wx.Panel(self))

        # Add a button to close the window
        txt = wx.StaticText(self, label='[-]')
        sizer.Add(txt, border=5, flag=wx.EXPAND|wx.TOP)
        txt.Bind(wx.EVT_MOTION, self.hand_cursor)
        txt.Bind(wx.EVT_LEAVE_WINDOW, self.mouse_cursor)
        txt.Bind(wx.EVT_LEFT_UP, self.start_hide)

        label = type(target).__name__
        base = type(target).__bases__[0]
        if base is not node.Node:
            label += ' (%s)' % (base.__name__)
        txt = wx.StaticText(self, label=label, size=(-1, 25),
                            style=wx.ST_NO_AUTORESIZE)
        txt.SetFont(wx.Font(14, family=wx.FONTFAMILY_DEFAULT,
                         style=wx.ITALIC, weight=wx.BOLD))
        sizer.Add(txt, border=5, flag=wx.TOP|wx.RIGHT|wx.EXPAND)

        sizer.Add(io.IO(self, base if base is not node.Node else type(target)),
                  border=3, flag=wx.BOTTOM|wx.TOP|wx.LEFT|wx.ALIGN_CENTER)


    def add_row(self, sizer, name, dat):
        sizer.Add(io.IO(self, dat.type),
                  border=3, flag=wx.BOTTOM|wx.TOP|wx.RIGHT|wx.ALIGN_CENTER)

        sizer.Add(wx.StaticText(self, label=name,
                                style=wx.ALIGN_RIGHT|wx.ST_NO_AUTORESIZE,
                                size=(-1, 25)),
                  border=3, flag=wx.ALL|wx.EXPAND)

        txt = wx.TextCtrl(self, size=(150, 25),
                          style=wx.NO_BORDER|wx.TE_PROCESS_ENTER)
        txt.datum = dat
        if isinstance(dat, datum.NameDatum):    txt.SetValue(dat.expr[1:-1])
        else:                                   txt.SetValue(dat.expr)

        txt.Bind(wx.EVT_TEXT, self.on_change)
        sizer.Add(txt, border=3, flag=wx.ALL|wx.EXPAND)
        self.data.append(txt)

        sizer.Add(io.IO(self, dat.type),
                  border=3, flag=wx.BOTTOM|wx.TOP|wx.LEFT|wx.ALIGN_CENTER)


    @staticmethod
    def on_change(event):
        """ When a text box changes, update the corresponding Datum
        """
        txt = event.GetEventObject()

        # Special case for Name datum
        if isinstance(txt.datum, datum.NameDatum):
            txt.datum.expr = "'%s'" % txt.GetValue()
        else:
            txt.datum.expr = txt.GetValue()


    def predraw(self, event):
        """ Check all datums for validity and change text color if invalid.
        """
        for txt in self.data:
            if txt.datum.valid():
                txt.SetForegroundColour(wx.NullColour)
            else:
                txt.SetForegroundColour(wx.Colour(255, 0, 0))

    def start_hide(self, event):
        self.expand = -1
        wx.CallAfter(self.Refresh)

    def reposition(self):
        """ Move this panel to the appropriate position and zoom as needed.
        """
        time = 5
        x = self.target.x if self.target._x.valid() else 0
        y = self.target.y if self.target._y.valid() else 0

        x, y = self.Parent.mm_to_pixel(x, y)
        if not self.target._x.valid():  x = self.GetPosition().x
        if not self.target._y.valid():  y = self.GetPosition().y

        self.MoveXY(x, y)
        if self.expand >= 0 and self.expand <= time:
            self.Size = (self.native_size.x * self.expand / time,
                         self.native_size.y * self.expand / time)
            self.expand += 1
            wx.CallAfter(self.Refresh)
        elif self.expand < 0:
            self.Size = (self.native_size.x * (self.expand + time) / time,
                         self.native_size.y * (self.expand + time) / time)
            self.expand -= 1
            wx.CallAfter(self.Refresh)
            if self.expand < -time:
                wx.CallAfter(self.Destroy)
                self.mouse_cursor()

_editors = {}

def MakeEditor(parent, target):
    return _editors.get(type(target), Editor)(parent, target)
