import wx

import canvas

class Frame(wx.Frame):
    def __init__(self, app):
        super(Frame, self).__init__(parent=None)

        self.canvas = canvas.Canvas(self, size=(200,200))
        sizer = wx.BoxSizer()
        sizer.Add(self.canvas, proportion=1, flag=wx.EXPAND)

        self.SetSizerAndFit(sizer)
        self.Show()
        self.Maximize()


