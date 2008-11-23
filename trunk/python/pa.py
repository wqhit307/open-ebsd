import sys
sys.path.append('..')

from ebsd import *
from scipy import array, arange, c_, zeros, histogram

from matplotlib.mlab import hist

from pickle import *

f = open("A1")
vols = load(f)
f.close
f = open("A2")
adj = load(f)

n, bins = histogram(adj,100,(0,200))
	
	
from matplotlib.mlab import normpdf
import pylab as p

p.plot(bins, n, color='red', lw=2)
p.show()
