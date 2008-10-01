import sys
sys.path.append('..')

from ebsd import *
from scipy import array, arange, c_, zeros, histogram

from matplotlib.mlab import hist

from pickle import *

ds = ebsd()
ds.read_raw("../../w")

grains = ds.grains()

vols = grains['volume']
adj = grains['num_adj_grains']

f = open("A1",'w')
dump(vols,f)
f.close()
f2 = open("A2",'w')
dump(adj,f2)

#n, bins, patches = hist(vols)
	
	
#from matplotlib.mlab import normpdf
#import pylab as p

#p.plot(bins, n, color='red', lw=2)
#p.show()
