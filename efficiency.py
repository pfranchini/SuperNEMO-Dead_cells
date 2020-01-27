# Usage: python test.py filename.txt

import numpy as np
import matplotlib.pyplot as plt
import sys
print (sys.argv)

data = np.loadtxt(sys.argv[1])

dead_cells = data[:,0]
efficiency = data[:,1]
zero = data[:,2]
more = data[:,3]
fig = plt.figure()

print("Efficiency: %0.2f +/- %0.2f" % (np.mean(efficiency),np.std(efficiency)))
print("Zero tracks: %0.2f +/- %0.2f" % (np.mean(zero),np.std(zero)))
print("More than one track: %0.2f +/- %0.2f" % (np.mean(more),np.std(more)))

plt.hist(efficiency, normed=True, bins=30)

plt.show()
