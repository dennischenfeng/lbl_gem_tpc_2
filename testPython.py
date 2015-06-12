import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D


test = 'plt'

if (test == 'out'):
	with open('testPython.txt', 'w') as out_file:
	    out_file.write('1,650.4,567.445\n')
	    out_file.write('2, 700.3,900.34\n')
	    out_file.write('3, 867.3453, 389.452' + '\n')
	    out_file.write('3, 400.3453, 700.452    ' + '\n')

if (test == 'in'):
	with open('testPython.txt', 'r') as f:
	    lines = '';
	    for line in f:
	        lines += line
	    print(lines)

if (test == 'plt'):
	with open('/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/46_module_202_new_stop_mode_ext_trigger_scan_interpreted.txt', 'r') as f:
		data = np.loadtxt(f, delimiter=',')
		'''
		plt.hist2d(data[:, 5], data[:, 6], 20)
		plt.title("Title yee")
		plt.xlabel("X Axis Label")
		plt.ylabel("Y Axis Label")
		#plt.legend()
		plt.show()
		'''
		fig = plt.figure()
		ax = fig.add_subplot(111, projection='3d')
		ax.scatter(data[976:3000, 5], data[976:3000, 6], data[976:3000, 7])
		plt.show()