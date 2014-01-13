import threading

from PySide import QtGui
import numpy as np

class RenderTask(object):
    def __init__(self, expression, transform, resolution, callback):
        """ Creates a render task.
            'expression' is an Expression object.
            'transform' and 'screen' are transform and pixel matrices.
            'callback' is a function to call when we're done rendering.
        """
        self.expression = expression
        self.transform = transform
        self.resolution = resolution
        self.callback = callback

        self.qimage = None

        self.thread = threading.Thread(target=self.run)
        self.thread.daemon = True
        self.thread.start()

    def __eq__(self, other):
        return (self.expression == other[0] and
                self.transform == other[1] and
                self.resolution == other[2])

    def run(self):

        # Transform this image based on our matrix
        self.transformed = self.expression.transform(self.transform.inverted()[0],
                                                     self.transform)
        tree = self.transformed.to_tree()
        self.image = tree.render(self.resolution)

        # Translate to 8-bit greyscale
        scaled = np.array(self.image.array >> 8, dtype=np.uint8)

        # Then make into an RGB image
        rgb = np.dstack([
            scaled, scaled, scaled,
            np.ones(scaled.shape, dtype=np.uint8)*255])

        # Finally, convert into a QImage
        self.pixels = rgb.flatten()
        self.qimage = QtGui.QImage(
                self.pixels, scaled.shape[1], scaled.shape[0],
                QtGui.QImage.Format_ARGB32)

        # Then call the callback (which updates rendering)
        self.callback()

    def join(self):
        """ Attempts to join the thread.
            Has no effect if the thread is already done running.
        """
        if self.thread:
            self.thread.join(0)
            return not self.thread.isAlive()
        return True



