from PySide import QtGui

import colors
from base import TextLabelControl

import re

class EquationViewerControl(TextLabelControl):

    @classmethod
    def new(cls, canvas, x, y, z, scale):
        v = EquationViewer(get_name('eq'), x, y, z)
        return cls(canvas, v)

    def __init__(self, canvas, target):
        super(EquationViewerControl, self).__init__(
                canvas, target, "None", colors.blue)
        self.editor_datums = ['input','output']

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


    def cache(self):
        """ Slightly fancier version of 'cache' that also
            saves the old equation and only regenerates the
            text if needed.
        """
        if 'input' in self._cache:  old_math = self._cache['input'].math
        else:                       old_math = ''

        changed = super(EquationViewerControl, self).cache()

        if old_math != self._cache['input'].math:
            self.text = self.trim_text(
                    str(self.node.input.to_tree()))

        return changed


    def reposition(self):
        self.font.setPointSize(min(14, max(6, self.canvas.scale * 800)))
        self.move(self.canvas.unit_to_pixel(self.position))
        self.setFixedSize(self.get_text_size())

from node.equation import EquationViewer
from node.base import get_name
