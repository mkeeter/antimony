import re
import keyword

class Name(object):
    """ A Name is a valid python variable name.
    """
    def __init__(self, v='_'):
        if not re.match("[_A-Za-z][_a-zA-Z0-9]*$", v):
            raise TypeError("Invalid variable name.")
        elif keyword.iskeyword(v):
            raise TypeError("Variable name is a python keyword")
        self.v = v

    def __str__(self):  return self.v
