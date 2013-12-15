import wx

import frame

class App(wx.App):
    def OnInit(self):
        self.frame = frame.Frame(self)
        return True
