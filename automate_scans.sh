#!/bin/bash
# Runs a specified scan as many times as you want. Also adds to run_extra_info_txt the info that you specify.

# OTHER_INFO="CR GEM-400 Pcp-756 scantime-180 maxtrig-4 (bulk CR events)"
# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
NUM_SCANS=20







































cd /home/pixel/pybar/pybar_github/pybar/scans

strindex() { #from glenn jackman on StackOverflow; finds index of first appearance of substring; used in this script to find LAST_NUM
  x="${1%%$2*}";
  [[ $x = $1 ]] && echo -1 || echo ${#x};
}


for ((i=1; i<=NUM_SCANS; i++)); do
	echo "@@ Starting automate_scan's scan #: $i"
	
	
	#python scan_fei4_self_trigger.py;

	# getting the last number in run_extra_info.txt
	# LAST_LINE=$(sed -n '$p' /home/pixel/pybar/tags/2.0.2/host/pybar/module_test/run_extra_info.txt);
	# LAST_NUM=${LAST_LINE:0:$(strindex "$LAST_LINE" " ")};
	# CURR_NUM=$((LAST_NUM+1))
	# # add new scan's info to run_extra_info.txt
	# echo "$CURR_NUM $OTHER_INFO" >> /home/pixel/pybar/tags/2.0.2/host/pybar/module_test/run_extra_info.txt;
done

