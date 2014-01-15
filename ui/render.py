import threading

from PySide import QtGui
import numpy as np

class RenderTask(object):

    MAX_ITERATION = 4

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

        self.iteration = 0

        self.image = None

        self.thread = threading.Thread(target=self.run)
        self.thread.daemon = True
        self.thread.start()

    def __eq__(self, other):
        return (self.expression == other[0] and
                self.transform == other[1] and
                self.resolution == other[2])

    def run(self):

        # Transform this image based on our matrix
        if self.expression.has_xyz_bounds():
            self.transformed = self.expression.transform(
                    self.transform.inverted()[0], self.transform)
        else:
            self.transformed = self.expression.transformXY(
                    self.transform.inverted()[0], self.transform)

        self.tree = self.transformed.to_tree()

        self._refine()

    def refine(self):
        self.thread = threading.Thread(target=self._refine)
        self.thread.daemon = True
        self.thread.start()

    def get_resolution(self):
        dx = self.transformed.xmax - self.transformed.xmin
        dy = self.transformed.ymax - self.transformed.ymin
        dz = self.transformed.zmax - self.transformed.zmin
        # Linear interpolation between these two resolutions
        bottom = 16 / min(dx, dy, dz)
        top = self.resolution
        if (top < bottom):  top = bottom
        i = self.iteration / float(self.MAX_ITERATION-1)
        print i, top, bottom
        return top*i + bottom*(1-i)


    def _refine(self):
        self.image = self.tree.render(self.get_resolution())

        # Then call the callback (which updates rendering)
        self.callback()

        if self.iteration < self.MAX_ITERATION:     self.iteration += 1

    def can_refine(self):
        return self.thread is None and self.iteration < self.MAX_ITERATION

    def join(self):
        """ Attempts to join the thread.
            Has no effect if the thread is already done running.
        """
        if self.thread:
            self.thread.join(0)
            if not self.thread.isAlive():
                self.thread = None
                return True
            else:
                return False
        return True



