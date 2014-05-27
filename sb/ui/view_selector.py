from PySide import QtGui
from .ui_view_selector import Ui_ViewSelector

class ViewSelector(QtGui.QWidget):
    def __init__(self, parent=None):
        super(ViewSelector, self).__init__(parent)
        self.ui = Ui_ViewSelector()
        self.ui.setupUi(self)
