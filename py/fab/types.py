# Types are loaded from Boost binding into the _fabtypes module.
# We run "import *" in this file to preserve the illusion of a
# consistent hierarchy (fab.shapes and fab.types); this is the
# cleanest way I've found to embed the Boost binding into a Python
# namespace that contains other .py files.
from _fabtypes import *
