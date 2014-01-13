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

    def flatten_matrix(self):
        """ Flatten out the transform matrix, removing z values.
        """
        self.transform.setColumn(2, QtGui.QVector4D(0, 0, 1, 0))
        self.transform.setRow(2, QtGui.QVector4D(0, 0, 1, 0))

    def run(self):

        # Transform this image based on our matrix
        if self.expression.has_xyz_bounds():
            self.transformed = self.expression.transform(
                    self.transform.inverted()[0], self.transform)
        else:
            self.transformed = self.expression.transformXY(
                    self.transform.inverted()[0], self.transform)

        print self.transformed.xmin, self.transformed.xmax
        print self.transformed.ymin, self.transformed.ymax
        print self.transformed.zmin, self.transformed.zmax
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



