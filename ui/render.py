import threading

from PySide import QtGui
import numpy as np

class RenderTask(object):
    def __init__(self, expression, pix_per_unit, callback):
        self.expression = expression
        self.resolution = pix_per_unit
        self.callback = callback

        self.qimage = None

        self.thread = threading.Thread(target=self.run)
        self.thread.daemon = True
        self.thread.start()

    def run(self):
        tree = self.expression.to_tree()
        self.image = tree.render(self.resolution)
        scaled = np.array(self.image.array >> 8, dtype=np.uint8)
        rgb = np.dstack([
            scaled, scaled, scaled,
            np.ones(scaled.shape, dtype=np.uint8)*255])
        self.pixels = rgb.flatten()
        self.qimage = QtGui.QImage(
                self.pixels, scaled.shape[1], scaled.shape[0],
                QtGui.QImage.Format_ARGB32)
        self.callback()

    def join(self):
        """ Attempts to join the thread.
            Has no effect if the thread is already done running.
        """
        if self.thread:
            self.thread.join(0)
            return not self.thread.isAlive()
        return True



