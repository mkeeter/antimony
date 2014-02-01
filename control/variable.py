from PySide import QtCore, QtGui

import colors
import base

class FloatVariableControl(base.TextLabelControl):
    @classmethod
    def new(cls, canvas, x, y, z, scale):
        f = FloatVariable(get_name('f'), x, y, z)
        return cls(canvas, f)

    def __init__(self, canvas, target):
        super(FloatVariableControl, self).__init__(
                canvas, target, "0", colors.yellow)
        self.sync()

        self.editor_datums = ['name','value']
        self.show()
        self.raise_()

    def cache(self):
        """ Overloaded version of cache that also updates text.
        """
        changed = super(FloatVariableControl, self).cache()
        txt = "%s = %g" % (self._cache['name'], self._cache['value'])
        if not hasattr(self, 'text') or self.text != txt:
            self.text = txt
            self.update()
            return True
        return changed

    def reposition(self):
        super(FloatVariableControl, self).reposition()
        self.setFixedSize(self.get_text_size())

from node.variable import FloatVariable
from node.base import get_name
