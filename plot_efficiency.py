
import numpy as np
import matplotlib.pyplot as plt

with open('efficiency.txt') as f:
    lines = f.readlines()
    dead_cells = [line.split()[0] for line in lines]
    efficiency = [line.split()[1] for line in lines]
    zero = [line.split()[2] for line in lines]
    more = [line.split()[3] for line in lines]
    short = [line.split()[4] for line in lines]


fig = plt.figure()

ax1 = fig.add_subplot(111)

ax1.set_ylim([0,100])


ax1.set_title("Killing cells in the tracker")    
ax1.set_xlabel('Number of dead cells')
ax1.set_ylabel('[%]')

ax1.plot(dead_cells,efficiency, color='green', label='Efficiency', marker='o')
ax1.plot(dead_cells,zero, color='red', label='Zero reconstructed tracks', marker='o')
ax1.plot(dead_cells,more, color='blue', label='More than one reconstructed track', marker='o')
ax1.plot(dead_cells,short, color='yellow', label='Short tracks', marker='o')


leg = ax1.legend(loc='upper left', fontsize='small')

plt.savefig('efficiency.png')
plt.show()
plt.draw()

