Author: Dennis Feng
Date: 15/06/10

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
	- Open up automate_scans.sh and change NUM_SCANS to the number of repetitions of SM (stop mode) scans you want. Save.
	- Type "bash automate_scans.sh" in terminal to start the scans. The h5 data files and pdf files will automatically be sent to the module_1 folder.  
	- Open up run_extra_info.txt, and record the info about the scans you are running.

2. Convert the _interpreted.h5 files into Aggr files ("aggregated" files). We want to aggregate a lot of the h5 files into one big file.
	- Choose which h5 files (_interpreted.h5) you want and record them down in aggregate_data_info.txt. Do not include h5 files that do not exist (probably due to the scan crashing). You should also record down the important parameters for this batch of scans (GEM voltage? any filters to cut out bad events?).
	- Open up convert_h5_to_Hits.py. This script creates a Hits file(contains info for each hit) corresponding to each h5 file chosen.
		Scroll down to the "main" method (if __name__ == "__main__":) and change h5_file_nums to be the list of h5 files you chose.
	- Open up convert_Hits_to_EventsCR.cpp. This script creates an EventsCR file(contains info for each event) corresponding to each h5 file chosen.
		Change fileNums[numFiles] to be the list of h5 files you chose and numFiles to be the number of those files. 
		Here is where you specify the good event criteria. Scroll down to near the end of the script or ctrl-f "// Criteria for good/bad event" and change the critera there.
	- Open up aggregate_Hits_and_EventsCR.cpp. This script creates an AggrHits file and an AggrEventsCR file, which contain the hits and events info respectively. These Aggr files are the big files that contain the data from all the h5 files specified.
		Change fileNums[numFiles] to be the list of h5 files you chose and numFiles to be the number of those files. Important: also change the aggrFileNum to the next number, or else the file with the specified file num will be overwritten. 
	- Run convert_h5_to_Hits.py, then convert_Hits_to_EventsCR.cpp, then aggregate_Hits_and_EventsCR.cpp, in that sequence. 
		Run .py scripts by "python _.py" and run root scripts by "root -l -x _.cpp". Now you should have the Aggr pair (AggrHits and AggrEventsCR) in your homemade_scripts/aggregate_data folder, which you can run data analysis on.

3. View histograms on the data
	- Open up view_individual_events.cpp or view_all_events.cpp.
	- Change the file_kind, file_num_input, and view_option to what you need.
	- Run the script in terminal.

WORKFLOW LAYOUT
---------------
I like to keep the files open in the following order in Sublime Text:

run_extra_info.txt                       - recording info
aggregate_data_info.txt                  - recording info
automate_scans.sh
scan_ext_trigger_stop_mode.py <- not necessary to be open if you never change this
convert_h5_to_Hits.py                    - converting file
convert_Hits_to_EventsCR.cpp             - converting file
aggregate_Hits_and_EventsCR.cpp          - converting file
view_individual_events.cpp               - viewing data
view_individual_events_to_scale.cpp      - viewing data
view_all_events.cpp                      - viewing data





2ND DRAFT:

SETUP THE PROGRAMS
------------------
1. Terminal
	- Open up the terminal. It's the terminal icon on the upper menu bar.
	- Type in "cd /home/pixel/pybar/pybar_github/pybar/homemade_scripts". This changes your directory to the homemade_scripts folder, which you need to be at in order to run the scripts.

2. File Browser
	- Open up the File Browser.
	- In the "Location:" box, type in "/home/pixel/pybar/pybar_github/pybar/module_1".
	- Create a new tab by clicking File, then New Tab. In the "Location:" box of the new tab, type in "/home/pixel/pybar/pybar_github/pybar/homemade_scripts".

3. Sublime Text
	- Open up the following files in Sublime Text (which can all be found in the homemade_scripts folder). You open up files in Sublime Text by double-clicking on them in the File Browser.

		run_extra_info.txt                       - recording info
		aggregate_data_info.txt                  - recording info
		automate_scans.sh
		scan_ext_trigger_stop_mode.py            <- not necessary
		convert_h5_to_Hits.py                    - converting file
		convert_Hits_to_EventsCR.cpp             - converting file
		aggregate_Hits_and_EventsCR.cpp          - converting file
		view_individual_events.cpp               - viewing data
		view_individual_events_to_scale.cpp      - viewing data
		view_all_events.cpp                      - viewing data


GENERAL PROCEDURE TO GATHER AND VIEW DATA
-----------------------------------------
Note: chip should be tuned, everything should be turned on

1. Gather data from pybar (_interpreted.h5 files)
	- Open up automate_scans.sh and change NUM_SCANS to the number of repetitions of SM (stop mode) scans you want. 
	- Type "bash automate_scans.sh" in terminal to start the scans. The h5 data files and pdf files will automatically be sent to the module_1 folder.  
	- Open up run_extra_info.txt, and record the info about the scans you are running.

2. Convert the _interpreted.h5 files into Hits and EventsCR files. They need to be converted in order to use the data analysis scripts. 
	- Open up convert_h5_to_Hits.py. This script creates a Hits file(contains info for each hit) corresponding to each h5 file chosen.
		Scroll down to the "main" method (if __name__ == "__main__":) and change h5_file_nums to be the list of h5 files you want to convert. Alternatively, you can also easily locate the correct line by clicking the "Find" tab, and then clicking "Find...". This opens up a little panel at the bottom of the window, in which you could type "@@@@@@@@@@@@", and the Find utility will take you to the correct line.
	- Open up convert_Hits_to_EventsCR.cpp. This script creates an EventsCR file (contains info for each event) corresponding to each h5 file chosen.
		Change fileNums[numFiles] to be the list of h5 files you chose and numFiles to be the number of those files. 
	- Type "python convert_h5_to_Hits.py" into the terminal to run the script for converting to Hits files.
	- Type "root -l -x convert_Hits_to_EventsCR.cpp" into the terminal to run the script for converting to EventsCR files. Ignore the errors and warnings. Then type ".q" to quit ROOT and return to normal terminal use.

	Now you should have the Hits and EventsCR files in your module_1 folder, which you can run data analysis on.

3. View histograms on the data
	- Open up view_individual_events.cpp.
	- Change the file_num_input to the h5 file num that you want.
	- Check to make sure the view_option is "1".
	- Type in "root -l -x view_individual_events.cpp" in the terminal to run the script.

USING THE 






