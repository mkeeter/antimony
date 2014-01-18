from PySide import QtGui

from base import TextLabelControl

import re

class EquationViewerControl(TextLabelControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        v = EquationViewer(get_name('eq'), x, y, z)
        return cls(canvas, v)

    def __init__(self, canvas, target):
        super(EquationViewerControl, self).__init__(
                canvas, target, "None")
        self.editor_datums = ['input','output']
        self.text = ''

    @staticmethod
    def trim_text(txt):
        linelen = 60
        out = ''
        while len(txt) > linelen:
            out += txt[:linelen]
            txt = txt[linelen:]
            while txt:
                newline = txt[0] == ')'
                out += txt[0]
                txt = txt[1:]
                if newline and len(txt) and txt[0] != ')':
                    out += '\n'
                    break
        out += txt
        return out


    def _sync(self):
        """ Set self.position and self.equation to an appropriate values.
        """
        v = QtGui.QVector3D(
                self.node.x if self.node._x.valid() else self.position.x(),
                self.node.y if self.node._y.valid() else self.position.y(),
                self.node.z if self.node._z.valid() else self.position.z())

        try:
            eqn = self.trim_text(str(self.node.input.to_tree()))
        except RuntimeError:
            eqn = self.text

        changed = (v != self.position) or (eqn != self.text)

        self.position = v
        self.text = eqn

        return changed

    def reposition(self):
        self.font.setPointSize(min(14, max(6, self.canvas.scale * 800)))
        self.move(self.canvas.unit_to_pixel(self.position))
        self.setFixedSize(self.get_text_size())

from node.equation import EquationViewer
from node.base import get_name
