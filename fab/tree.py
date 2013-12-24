import os
import threading

class MathTree(object):
    def __init__(self, expr, ptr):
        self.ptr  = ptr
        self.expr = expr

    def __del__(self):
        if libfab:
            libfab.free_tree(self.ptr)

    @classmethod
    def from_expression(cls, expr):
        if not isinstance(expr, expression.Expression):
            raise TypeError("Input expression is of wrong type.")
        ptr = libfab.parse(expr.math)
        if ptr.value is None:
            raise TypeError("Math expression parsing failed.")
        return cls(expr, ptr)

    def __str__(self):
        if not hasattr(self, '_str'):
            self._str = self.make_str()
        return self._str

    def make_str(self):
        """ @brief Converts the object into an infix-notation string

            @details
            Creates a OS pipe, instructs the object to print itself into the
            pipe, and reads the output in chunks of maximum size 1024.
        """

        # Create a pipe to get the printout
        read, write = os.pipe()

        # Start the print function running in a separate thread
        # (so that we can eat the output and avoid filling the pipe)
        t = threading.Thread(target=libfab.fdprint_tree,
                             args=(self.ptr, write))
        t.daemon = True
        t.start()

        s = r = os.read(read, 1024)
        while r:
            r = os.read(read, 1024)
            s += r
        t.join()

        os.close(read)

        return s


import expression
from libfab import libfab
