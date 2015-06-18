import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

""" This function displays the 3D occupancy of the specified SM event. """ 

# CHOOSE THIS
SM_event_num = 61


# Helper function
def get_entryIndex_with_SMEventNum(data, SM_event_num):
    ''' Returns the entry_index of the first entry that has the specified h5_event_num. Returns -1 if not found '''
    entries_found = np.where(data[:, 5] == SM_event_num) # data[:, 5] is the SM_event_num of all entries
    
    if (len(entries_found) == 1):
        if (len(entries_found[0] != 0)):
            return entries_found[0][0] # first entry num that has SM_event_num
        else:
            return -1 # not found
    else:
        print("Error: len(entries_found) should be 1. Look inside function get_entryIndex_with_h5EventNum().")


if __name__ == '__main__':
	with open('/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/46_module_202_new_stop_mode_ext_trigger_scan_interpreted.txt', 'r') as f:
	    
	    #import pdb; pdb.set_trace() @@@
	    
	    data = np.loadtxt(f, delimiter=',')
	    
	    start = get_entryIndex_with_SMEventNum(data, SM_event_num)
	    end = get_entryIndex_with_SMEventNum(data, SM_event_num + 1) # end index (plotted points does not include the point at this index)
	    
	    # Initialize figure and axes
	    fig = plt.figure() # current fig
	    ax = fig.add_subplot(111, projection='3d') # current axes
	    
	    ax.set_title("3D Occupancy of SM Event")
	    ax.set_xlabel('x (mm)')
	    ax.set_ylabel('y (mm)')
	    ax.set_zlabel('z (mm)')
	    ax.set_xlim3d(0, 20)
	    ax.set_ylim3d(-16.8, 0)
	    ax.set_zlim3d(0, 40.96)
	    
	    # Plot data
	    ax.scatter(data[start:end, 6], data[start:end, 7], data[start:end, 8])
	    plt.show()
