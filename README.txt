Author: Dennis Feng
Date: 15/06/10

Procedure to get the final calculated data files (called Events files, ex: EventsCR files):
1. Run the desired scan (ex: stop mode scan), which will output an ..._interpreted.h5 file.
2. Run "convert_h5_to_Hits.py" (pointing to the desired ..._interpreted.h5), which will output the ..._Hits.root file.
3. Run "convert_Hits_to_Events(SOURCE).cpp" (pointing to the desired ..._Hits.root), which will output the ..._Events(SOURCE).root file.

The possible sources are:
- XR: X-rays, Fe55 source 
- CR: cosmic rays
- Nu: neutrons

To run python scripts, just type "python __________" into terminal.
To run ROOT scripts, type "root -l -x __________" into terminal (-l to skip splash image, -x to run file). Then ".q" to quit ROOT shell.



Date: 7/23/15

MAJOR ASPECTS OF THE GEM-TPC PROJECT
------------------------------------
1. Hardware - TPC Setup, FE-I4B and USBPix module
2. Software - PyBAR, ROOT


OVERVIEW OF HOW WE USE PYBAR
----------------------------
PyBAR is a DAQ readout system; it allows you to readout from the FE-I4 and analyze its data. 

The main pybar folder is /home/pixel/pybar/pybar_github/pybar. For the most part, we are only concerned with the folders homemade_scripts, module_1, and scans. 

	homemade_scripts: contains the scripts that I made for further data analysis (not originally from pybar).
	
	module_1: contains all the files that are outputted from the scans 
		pdf file: for viewing the data at a glance; some useful histograms
		_.h5: raw data file
		_interpreted.h5: data file that has been analyzed by pybar; this is the file that we are concerned with; it contains info about row, col, tot, relative_BCID, etc
	
	scans: contains all the scripts that run the chip. The ones that we use are:
		scan_analog.py: for viewing the ToT distr (checking if tuning worked)
		scan_threshold.py: for viewing the threshold distr (checking if tuning worked)
		scan_threshold_fast.py: faster version
		scan_ext_trigger_stop_mode.py: we use stop mode so that we can gather events that span the entire 255 time bins (rather than only 16). we need this to see long tracks that go through our entire 12 cm field cage
		tune_fei4.py: tune the chip


GENERAL PROCEDURE TO GATHER AND VIEW DATA
-----------------------------------------
Note: chip should be tuned, everything should be turned on

1. Gather data from pybar (_interpreted.h5 files)
	- Open up automate_scans.sh and change NUM_SCANS to the number of repetitions of SM (stop mode) scans you want. 
	- Type "bash automate_scans.sh" in terminal to start the scans.

2. Convert the _interpreted.h5 files into Aggr files (aggregated)
	- Choose which h5 files (_interpreted.h5) and record them down in aggregate_data_info.txt 
	- Open up convert_h5_to_Hits.py. 
		Scroll down to the "main" method (if __name__ == "__main__":) and change h5_file_nums to be the list of h5 files you chose.
	- Open up convert_Hits_to_EventsCR.cpp. 
		Change fileNums[numFiles] to be the list of h5 files you chose and numFiles to be the number of those files. 
		Here is where you specify the good event criteria. Scroll down to near the end of the script or ctrl-f "// Criteria for good event" and change the critera there.
	- Open up aggregate_Hits_and_EventsCR.cpp. 
		Change fileNums[numFiles] to be the list of h5 files you chose and numFiles to be the number of those files. Also change the aggrFileNum to the next number.
	- Run convert_h5_to_Hits.py, then convert_Hits_to_EventsCR.cpp, then aggregate_Hits_and_EventsCR.cpp, in that sequence. 
		Run .py scripts by "python _.py" and run root scripts by "root -l -x _.cpp". Now you should have the Aggr pair (AggrHits and AggrEventsCR) in your homemade_scripts/aggregate_data folder.

3. View histograms on the data
	- Open up view_individual_events.cpp or view_all_events.cpp.
	- Change the file_kind, file_num_input, and view_option to what you want.
	- Run the script in terminal.










