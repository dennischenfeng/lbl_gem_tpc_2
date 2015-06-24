"""
Creates a txt file that contains all the necessary data 
values. The .txt file is for my python scripts. Uses the hit table from the _interpreted.h5 
file of the desired scan to convert. 

The data contains: (column num, root type descriptor, root type)
0. hit_entry_index
1. h5_file_num, i, UInt_t
2. event_number, L, Long64_t (same numbers as indicated by h5 file)
3. tot, b, UChar_t
4. relative_BCID, b, UChar_t
5. SM_event_num
6. x, D, Double_t (in mm)
7. y, D, Double_t 
8. z, D, Double_t

Author: Dennis Feng
"""

import tables as tb
import numpy as np
import ctypes
import progressbar
from ROOT import TFile, TTree
from ROOT import gROOT, AddressOf

def convert_two_hit_tables(input_filename, output_filename, h5_file_num):
    # @@@
    ''' Creates a ROOT Tree by looping over all entries of the table.
    In each iteration all entries are type casting to int and appended to the ROOT Tree. This is straight forward but rather slow (45 kHz Hits).
    The ROOT Tree has its addresses pointing to a hit struct members. The struct is defined in ROOT.

    Parameters
    ----------
    input_filename: string
        The file name of the hdf5 hit table.

    output_filename: string
        The filename of the created ROOT file

    h5_file_num: int
        The number of the h5 file; assigned by pybar
        
    '''
    with tb.open_file(input_filename, 'r') as in_file_h5:

        # load the two desired tables:
        hits = in_file_h5.root.Hits
        meta = in_file_h5.root.meta_data

        progress_bar = progressbar.ProgressBar(widgets=['', progressbar.Percentage(), ' ', progressbar.Bar(marker='*', left='|', right='|'), ' ', progressbar.ETA()], maxval=hits.shape[0], term_width=80)
        progress_bar.start()

        update_progressbar_index = hits.shape[0] / 1000
        
        with open(output_filename, 'w') as out_file_txt:
            for index, hit in enumerate(hits):
                event_number = int(hit['event_number'])
                #trigger_number = int(hit['trigger_number']) # use this for self trigger scans
                trigger_time_stamp = int(hit['trigger_time_stamp']) # use this for stop mode scans
                relative_BCID = int(hit['relative_BCID'])
                LVL1ID = int(hit['LVL1ID'])
                column = int(hit['column'])
                row = int(hit['row'])
                tot = int(hit['tot'])
                BCID = int(hit['BCID'])
                TDC = int(hit['TDC'])
                trigger_status = int(hit['trigger_status'])
                service_record = int(hit['service_record'])
                event_status = int(hit['event_status'])

                SM_event_num = event_number / 16
                x = (int(hit['column']) - 1) * 0.25 + 0.001 # + 0.001 is to get rid of any tiny roundoff errors due to using a doubles
                y = - (int(hit['row']) - 1) * 0.05 + 0.001 # 
                
                SM_rel_BCID = (int(hit['event_number']) - (int(hit['event_number'])/16) * 16) * 16 + int(hit['relative_BCID']) # Range: 0 - 255
                z = (SM_rel_BCID * 0.16) + 0.001 # Drift speed: 0.16 mm per BCID
                
                '''
                0. hit entry index
                1. h5_file_num, i, UInt_t
                2. event_number, L, Long64_t (same numbers as indicated by h5 file)
                3. tot, b, UChar_t
                4. relative_BCID, b, UChar_t
                5. SM_event_num
                6. x, D, Double_t (in mm)
                7. y, D, Double_t 
                8. z, D, Double_t
                '''
                out_file_txt.write(\
                    str(index) + ', ' + \
                    str(h5_file_num) + ', ' + \
                    str(event_number) + ', ' + \
                    str(tot) + ', ' + \
                    str(relative_BCID) + ', ' + \
                    str(SM_event_num) + ', ' + \
                    str(x) + ', ' + \
                    str(y) + ', ' + \
                    str(z) + '\n')

                if (index % update_progressbar_index == 0):
                    progress_bar.update(index)

        progress_bar.finish()

if __name__ == "__main__":
    path_to_folder = '/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new'
    name_tail = '_module_202_new_stop_mode_ext_trigger_scan_interpreted'

    h5_file_num = 46

    # chose this parameter as big as possible to increase speed, but not too 
    # big otherwise program crashed:
    chunk_size = 50000  
    
    convert_two_hit_tables(path_to_folder + '/' + str(h5_file_num) + name_tail +'.h5', path_to_folder + '/' + str(h5_file_num) + name_tail + '.txt', h5_file_num)
    
