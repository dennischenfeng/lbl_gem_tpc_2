import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D


test = 'svd'

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

if (test == 'testplt'):
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

if (test == 'svd'): # from http://stackoverflow.com/questions/2298390/fitting-a-line-in-3d
	# Generate some data that lies along a line

	x = np.mgrid[-2:5:240j]
	y = np.mgrid[1:9:240j]
	z = np.mgrid[-5:3:240j]

	data = np.concatenate((x[:, np.newaxis], 
	                       y[:, np.newaxis], 
	                       z[:, np.newaxis]), 
	                      axis=1)

	# Perturb with some Gaussian noise
	data += np.random.normal(size=data.shape) * 0.5

	# Calculate the mean of the points, i.e. the 'center' of the cloud
	datamean = data.mean(axis=0)

	# Do an SVD on the mean-centered data.
	uu, dd, vv = np.linalg.svd(data - datamean)

	# Now vv[0] contains the first principal component, i.e. the direction
	# vector of the 'best fit' line in the least squares sense.

	# Now generate some points along this best fit line, for plotting.

	# I use -7, 7 since the spread of the data is roughly 14
	# and we want it to have mean 0 (like the points we did
	# the svd on). Also, it's a straight line, so we only need 2 points.
	linepts = vv[0] * np.mgrid[-7:7:2j][:, np.newaxis]

	# shift by the mean to get the line in the right place
	linepts += datamean

	# Verify that everything looks right.

	import matplotlib.pyplot as plt
	import mpl_toolkits.mplot3d as m3d

	ax = m3d.Axes3D(plt.figure())
	ax.scatter3D(*data.T)
	ax.plot3D(*linepts.T)
	print dd
	plt.show()

	