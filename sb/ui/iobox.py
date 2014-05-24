from PySide import QtCore, QtGui
import sb.colors

class IObox(QtGui.QPushButton):
    def __init__(self, datum, parent):
        super(IObox, self).__init__(parent)
        self.setFixedSize(10, 10)

        self._datum = datum
        color = sb.colors.get_color(self._datum.data_type)
        self.setStyleSheet("""
QPushButton {
    background-color: "%s";
    border-width: 0;
    margin: 0px;
    padding: 0px;
 }

QPushButton:hover {
    background-color: "%s";
}""" % (color, sb.colors.highlight(color)))

class InputBox(IObox):
    pass

class OutputBox(IObox):
    pass

