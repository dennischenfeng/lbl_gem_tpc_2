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