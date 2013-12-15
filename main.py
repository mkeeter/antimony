#!/usr/bin/env python
import sys

import app

try:
    import wx
except ImportError:
    print "Error: wx import failed!"
    sys.exit(1)
else:
    if wx.version()[:3] != '2.9':
        print "\n\nError: wxPython2.9 required."
        sys.exit(1)

if __name__ == '__main__':
    app.App().MainLoop()
