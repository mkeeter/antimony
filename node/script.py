import base
import datum

class ScriptNode(base.Node3D):
    default = """import math

from fab.shapes import *
from fab.expression import Expression

c = circle(0, 0, 10)
output('c', c)
"""

    def __init__(self, name, x, y, z):
        super(ScriptNode, self).__init__(name, x, y, z)
        self.add_datum('script', datum.ScriptDatum(self, self.default))

        self.default_datums = [d[0] for d in self.datums]
        self._updating = False

    def get_control(self):
        import control.script
        return control.script.ScriptNodeControl


    def update_datums(self):
        """ Updates datums based on script inputs and outputs.
        """

        # Prevent recursive calls
        # (e.g. when we delete a datum with a Connection, which
        #  then calls sync on connected controls)
        if self._updating:   return

        self._updating = True
        # Force a script re-eval (to populate _inputs and _outputs)
        try:
            self.script
        except:
            self._updating = False
            return

        # Keep track of whether anything has changed.
        changed = False

        # Make a [name, type] array for dynamic datums
        dynamic = [d[0:2] for d in self._script._inputs +
                                   self._script._outputs]

        # Check to make sure that each existing datum is either one of the
        # defaults or defined by this set of dynamic datums.
        for name, d in self.datums:
            if not (name in self.default_datums or [name, d.type] in dynamic):
                changed = True
                self.del_datum(name)

        # Add new input datums as needed.
        existing = {(name, d.type): d for name, d in self.datums}
        for name, t in self._script._inputs:
            if (name, t) in existing:
                continue
            elif t is float:
                d = datum.FloatDatum(self, 0)
            elif t is Expression:
                d = datum.ExpressionDatum(self, "None")
            self.add_datum(name, d)
            changed = True

        for name, t, value in self._script._outputs:
            if (name, t) in existing:
                existing[(name, t)].set_value(value)
                continue
            elif t is float:
                d = datum.FloatOutputDatum(self)
            elif t is Expression:
                d = datum.ExpressionOutputDatum(self)
            d.set_value(value)
            self.add_datum(name, d)
            changed = True

        old_editor_datums = self.control.editor_datums

        new_editor_datums = [d[0] for d in self._script._inputs +
                                           self._script._outputs]

        # Update editor datums
        for d in new_editor_datums:
            if d in self.control.editor_datums:
                self.control.editor_datums.remove(d)
        self.control.editor_datums += new_editor_datums
        if self.control.editor_datums != old_editor_datums:
            changed = True

        if changed and self.control.editor:
            self.control.editor.regenerate_grid()

        self._updating = False


from fab.expression import Expression
