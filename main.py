#!/usr/bin/env python
import sys
import app

try:
    from PySide import QtGui
except ImportError:
    print "Error: PySide import failed!"
    sys.exit(1)

if __name__ == '__main__':
    app = app.App()
    sys.exit(app.exec_())
