import os
import glob
files = [os.path.basename(f)[:-3] for f in
           glob.glob(os.path.dirname(__file__)+"/*.py") if
           not os.path.basename(f).startswith('_')]
for f in files:
    __import__('sb.nodes.%s' % f, locals(), globals())
