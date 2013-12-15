import re
import keyword

class Name(object):
    """ A Name is a valid python variable name.
    """
    def __init__(self, v):
        if not re.match("[_A-Za-z][_a-zA-Z0-9]*$", v):
            raise TypeError("Invalid variable name.")
        elif keyword.iskeyword(v):
            raise TypeError("Variable name is a python keyword")
        self.v = v

    def __eq__(self, other):
        """ Checks for equality against strings and other Names
        """
        if isinstance(other, str):      return self.v == other
        elif isinstance(other, Name):   return self.v == other.v
