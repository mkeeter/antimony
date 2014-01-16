import threading

from PySide import QtGui
import numpy as np

class RenderTask(object):

    MAX_ITERATION = 4

    def __init__(self, expression, transform, resolution,
                 posttransform, callback):
        """ Creates a render task.
            'expression' is an Expression object.
            'transform' and 'screen' are transform and pixel matrices.
            'posttransform' is a post-render transform applied to corners of
             the image (used in rendering 2D images)
            'callback' is a function to call when we're done rendering.
        """
        self.expression = expression
        self.transform = transform
        self.posttransform = posttransform
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
                self.resolution == other[2] and
                self.posttransform == other[3])

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
        if min(dx, dy, dz) == 0:    return 1

        # Pick top and bottom resolutions
        bottom = 16 / min(dx, dy, dz)
        top = self.resolution
        if (top < bottom):  top = bottom

        # Linear interpolation between these two resolutions
        i = self.iteration / float(self.MAX_ITERATION-1)
        return top*i + bottom*(1-i)


    def _refine(self):
        # Render the image at the given resolution
        image = self.tree.render(self.get_resolution())

        # Apply the post-transform to image corners
        # (used in flattening out 2D images)
        a = self.posttransform * QtGui.QVector3D(
                image.xmin, image.ymin, image.zmin)
        b = self.posttransform * QtGui.QVector3D(
                image.xmax, image.ymax, image.zmax)

        # Modify image bounds
        image.xmin, image.ymin, image.zmin = a.x(), a.y(), a.z()
        image.xmax, image.ymax, image.zmax = b.x(), b.y(), b.z()

        # Properly flip upside-down 2D images
        if image.ymin > image.ymax:
            image.array = image.array[::-1]
            image.ymin, image.ymax = image.ymax, image.ymin

        self.image = image

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



