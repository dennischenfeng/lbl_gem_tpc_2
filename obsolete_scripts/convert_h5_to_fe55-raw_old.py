"""
Creates a CERN ROOT file that contains a TTree that holds all the necessary data 
values in its branches. Uses the h5 hit and metadata tables to convert.

The TTree contains: (name of value, root type descriptor)
- h5_file_num, i
- time_stamp_start, D
- event_num, L (same labels as indicated by h5 file)
- tot, b
- relative_BCID, b
- x, D
- y, D
- z, D

Depending on whether you want to convert self_trigger scans or ext_trigger_stop_mode scans, you may need to choose which init_hit_struct() to use.

This script is based on convert_table_root_tree.py (from pybar) and convert_multi_table.py
(from Andrew Hard). 

Author: Dennis Feng
"""

import tables as tb
import numpy as np
import ctypes
import progressbar
from ROOT import TFile, TTree
from ROOT import gROOT, AddressOf

'''
#use this method when converting self-trigger scan h5 files
def init_hit_struct(): 
    gROOT.ProcessLine(
        "struct HitInfo{\
        ULong64_t event_number;\
	UInt_t trigger_number;\
        UChar_t relative_BCID;\
        UShort_t LVL1ID;\
        UChar_t column;\
        UShort_t row;\
        UChar_t tot;\
        UShort_t BCID;\
        UShort_t TDC;\
        UShort_t TDC_time_stamp;\
        UChar_t trigger_status;\
        UInt_t service_record;\
        UShort_t event_status;\
    };")
    from ROOT import HitInfo
    return HitInfo()
'''

#use this method when converting stopmode scan h5 files
def init_hit_struct():
    gROOT.ProcessLine(
        "struct HitInfo{\
        ULong64_t event_number;\
        UInt_t trigger_time_stamp;\
        UChar_t relative_BCID;\
        UShort_t LVL1ID;\
        UChar_t column;\
        UShort_t row;\
        UChar_t tot;\
        UShort_t BCID;\
        UShort_t TDC;\
        UShort_t TDC_time_stamp;\
        UChar_t trigger_status;\
        UInt_t service_record;\
        UShort_t event_status;\
    };")
    from ROOT import HitInfo
    return HitInfo()

def init_meta_struct():
    gROOT.ProcessLine(
        "struct MetaInfo{\
        Int_t event_number;\
        Int_t timestamp_start;\
        Double_t timestamp_stop;\
        UChar_t error_code;\
    };")
    from ROOT import MetaInfo
    return MetaInfo();

'''
This is to contain the info for calculated data, such as z, mean x, RMSxyz, etc.
'''
def init_extracalc_struct():
    gROOT.ProcessLine(
        "struct ExtraCalcInfo{\
        UInt_t raw_file_num;\
        Double_t z;\
        UInt_t num_hits;\
        UInt_t sum_tots;\
        Double_t mean_x;\
        Double_t mean_y;\
        Double_t mean_z;\
        Double_t R_xy;\
        Double_t R_xyz;\
        Double_t RMS_xy;\
        Double_t RMS_xyz;\
    };")
    from ROOT import ExtraCalcInfo
    return ExtraCalcInfo();

def get_root_type_descriptor(numpy_type_descriptor):
    ''' Converts the numpy type descriptor to the ROOT type descriptor.
    Parameters
    ----------
    numpy_type_descriptor: np.dtype
    '''
    return{
        'int64': 'L',
        'uint64': 'l',
        'int32': 'I',
        'uint32': 'i',
        'int16': 'S',
        'uint16': 's',
        'int8': 'B',
        'uint8': 'b',
        'float64': 'D',
        'ufloat64': 'd',
        'double': 'D',
        'udouble': 'd',

    }[str(numpy_type_descriptor)]


def get_c_type_descriptor(numpy_type_descriptor):
    ''' Converts the numpy type descriptor to the ctype descriptor.
    Parameters
    ----------
    numpy_type_descriptor: np.dtype
    '''
    return{
        'int64': ctypes.c_longlong,
        'uint64': ctypes.c_ulonglong,
        'int32': ctypes.c_int,
        'uint32': ctypes.c_uint,
        'int16': ctypes.c_short,
        'uint16': ctypes.c_ushort,
        'int8': ctypes.c_byte,
        'uint8': ctypes.c_ubyte,
        'float64': ctypes.c_double,
        'ufloat64': ctypes.c_udouble,
        
    }[str(numpy_type_descriptor)]


def init_tree_from_table(hit_table, meta_table, chunk_size=1, hit_tree_entry=None, meta_tree_entry=None, extracalc_tree_entry=None):
    ''' Initializes a ROOT tree from a HDF5 table.
    Takes the HDF5 table column names and types and creates corresponding 
    branches. If a chunk size is specified the branches will have the length of 
    the chunk size and an additional parameter is returned to change the chunk 
    size at a later stage. If a tree_entry is defined (a ROOT c-struct) the new 
    tree has the branches set to the corresponding tree entry address.
    
    Parameters
    ----------
    numpy_type_descriptor: np.dtype
    '''
    if(chunk_size > 1 and hit_tree_entry is not None and meta_tree_entry is not None):
        raise NotImplementedError()

    tree = TTree('Table', 'Converted HDF5 table')
    n_entries = None
    if chunk_size > 1:
        n_entries = ctypes.c_int(chunk_size) if chunk_size > 1 else 1
        # needs to be added, otherwise one cannot access chunk_size_tree:
        tree.Branch('n_entries', ctypes.addressof(n_entries), 'n_entries/I')
        
    # adding branches for hit table:
    #for hit_column_name in hit_table.dtype.names:
    #    print hit_column_name, hit_table.dtype[hit_column_name], get_root_type_descriptor(hit_table.dtype[hit_column_name])
    #    tree.Branch(hit_column_name, 'NULL' if hit_tree_entry is None else AddressOf(hit_tree_entry, hit_column_name), hit_column_name + '[n_entries]/' + get_root_type_descriptor(hit_table.dtype[hit_column_name]) if chunk_size > 1 else hit_column_name + '/' + get_root_type_descriptor(hit_table.dtype[hit_column_name]))

    # adding branches for meta table (timestamps):
    #for meta_column_name in meta_table.dtype.names:
    #    print meta_column_name, meta_table.dtype[meta_column_name], get_root_type_descriptor(meta_table.dtype[meta_column_name])
    #    if meta_column_name == 'timestamp_start' or meta_column_name == 'timestamp_stop':
    #        tree.Branch(meta_column_name, 'NULL' if meta_tree_entry is None else AddressOf(meta_tree_entry, meta_column_name), meta_column_name + '[n_entries]/' + get_root_type_descriptor(meta_table.dtype[meta_column_name]) if chunk_size > 1 else meta_column_name + '/' + get_root_type_descriptor(meta_table.dtype[meta_column_name]))
    
    tree.Branch('raw_file_num', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'raw_file_num'), 'raw_file_num' + '/' + 'i')
    tree.Branch('timestamp_start', 'NULL' if meta_tree_entry is None else AddressOf(meta_tree_entry, 'timestamp_start'), 'timestamp_start' + '[n_entries]/' + 'D' if chunk_size > 1 else 'timestamp_start' + '/' + 'D')
    #tree.Branch('time_stamp', 'NULL' if hit_tree_entry is None else AddressOf(time_stamp), 'time_stamp' + '[n_entries]/' + 'I' if chunk_size > 1 else 'time_stamp' + '/' + 'I')
    tree.Branch('event_number', 'NULL' if hit_tree_entry is None else AddressOf(hit_tree_entry, 'event_number'), 'event_number' + '[n_entries]/' + 'L' if chunk_size > 1 else 'event_number' + '/' + 'L')
    tree.Branch('x', 'NULL' if hit_tree_entry is None else AddressOf(hit_tree_entry, 'column'), 'x' + '[n_entries]/' + 'b' if chunk_size > 1 else 'x' + '/' + 'b')
    tree.Branch('y', 'NULL' if hit_tree_entry is None else AddressOf(hit_tree_entry, 'row'), 'y' + '[n_entries]/' + 's' if chunk_size > 1 else 'y' + '/' + 's')
    tree.Branch('tot', 'NULL' if hit_tree_entry is None else AddressOf(hit_tree_entry, 'tot'), 'tot' + '[n_entries]/' + 'b' if chunk_size > 1 else 'tot' + '/' + 'b')
    tree.Branch('relative_BCID', 'NULL' if hit_tree_entry is None else AddressOf(hit_tree_entry, 'relative_BCID'), 'relative_BCID' + '[n_entries]/' + 'b' if chunk_size > 1 else 'relative_BCID' + '/' + 'b')
    tree.Branch('z', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'z'), 'z' + '[n_entries]/' + 'D' if chunk_size > 1 else 'z' + '/' + 'D')
    tree.Branch('num_hits', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'num_hits'), 'num_hits' + '[n_entries]/' + 'i' if chunk_size > 1 else 'num_hits' + '/' + 'i')
    tree.Branch('sum_tots', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'sum_tots'), 'sum_tots' + '[n_entries]/' + 'i' if chunk_size > 1 else 'sum_tots' + '/' + 'i')
    tree.Branch('mean_x', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'mean_x'), 'mean_x' + '[n_entries]/' + 'D' if chunk_size > 1 else 'mean_x' + '/' + 'D')
    tree.Branch('mean_y', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'mean_y'), 'mean_y' + '[n_entries]/' + 'D' if chunk_size > 1 else 'mean_y' + '/' + 'D')
    tree.Branch('mean_z', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'mean_z'), 'mean_z' + '[n_entries]/' + 'D' if chunk_size > 1 else 'mean_z' + '/' + 'D')
    tree.Branch('R_xy', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'R_xy'), 'R_xy' + '[n_entries]/' + 'D' if chunk_size > 1 else 'R_xy' + '/' + 'D')
    tree.Branch('R_xyz', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'R_xyz'), 'R_xyz' + '[n_entries]/' + 'D' if chunk_size > 1 else 'R_xyz' + '/' + 'D')
    tree.Branch('RMS_xy', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'RMS_xy'), 'RMS_xy' + '[n_entries]/' + 'D' if chunk_size > 1 else 'RMS_xy' + '/' + 'D')
    tree.Branch('RMS_xyz', 'NULL' if hit_tree_entry is None else AddressOf(extracalc_tree_entry, 'RMS_xyz'), 'RMS_xyz' + '[n_entries]/' + 'D' if chunk_size > 1 else 'RMS_xyz' + '/' + 'D')

    return tree, n_entries


def convert_two_hit_tables(input_filename, output_filename):
    ''' Creates a ROOT Tree by looping over all entries of the table.
    In each iteration all entries are type casting to int and appended to the ROOT Tree. This is straight forward but rather slow (45 kHz Hits).
    The ROOT Tree has its addresses pointing to a hit struct members. The struct is defined in ROOT.

    Parameters
    ----------
    input_filename: string
        The file name of the hdf5 hit table.

    output_filename: string
        The filename of the created ROOT file

    '''
    with tb.open_file(input_filename, 'r') as in_file_h5:

        # load the two desired tables:
        hits = in_file_h5.root.Hits
        myHit = init_hit_struct()

        meta = in_file_h5.root.meta_data
        myMeta = init_meta_struct()

        # assign the myExtraCalc struct
        myExtraCalc = init_extracalc_struct()
        
        # create ttree using table structure:
        out_file_root = TFile(output_filename, 'RECREATE')
        tree, _ = init_tree_from_table(hits, meta, 1, myHit, myMeta, myExtraCalc)

        progress_bar = progressbar.ProgressBar(widgets=['', progressbar.Percentage(), ' ', progressbar.Bar(marker='*', left='|', right='|'), ' ', progressbar.ETA()], maxval=hits.shape[0], term_width=80)
        progress_bar.start()

        update_progressbar_index = hits.shape[0] / 1000
        
        # create iterator for 'meta' to advanced inside 'hits' for-loop:
        meta_iter = iter(meta)
        try:
            event = meta_iter.next()
        except StopIteration:
            pass
        
        for index, hit in enumerate(hits):
            myHit.event_number = int(hit['event_number'])
            #myHit.trigger_number = int(hit['trigger_number']) # use this for self trigger scans
            myHit.trigger_time_stamp = int(hit['trigger_time_stamp']) # use this for stop mode scans
            myHit.relative_BCID = int(hit['relative_BCID'])
            myHit.LVL1ID = int(hit['LVL1ID'])
            myHit.column = int(hit['column'])
            myHit.row = int(hit['row'])
            myHit.tot = int(hit['tot'])
            myHit.BCID = int(hit['BCID'])
            myHit.TDC = int(hit['TDC'])
            myHit.trigger_status = int(hit['trigger_status'])
            myHit.service_record = int(hit['service_record'])
            myHit.event_status = int(hit['event_status'])
        
            # here's where we advance the meta table iterator:
            while event['event_number'] < hit['event_number']:
                try:
                    event = meta_iter.next()
                except StopIteration:
                    break;

            if event['event_number'] == hit['event_number']:
                myMeta.timestamp_start = float(event['timestamp_start'])
                myMeta.timestamp_stop = float(event['timestamp_stop'])
            
            # Here, we calc the values for the calculated data and assign them
            myExtraCalc.raw_file_num = 100
            myExtraCalc.z = 0
            myExtraCalc.num_hits = 100
            myExtraCalc.sum_tots = 0
            myExtraCalc.mean_x = 0
            myExtraCalc.mean_y = 0
            myExtraCalc.mean_z = 0
            myExtraCalc.R_xy = 0
            myExtraCalc.R_xyz = 0
            myExtraCalc.RMS_xy = 0
            myExtraCalc.RMS_xyz = 0

            # Fill the tree that includes data from both trees. 
            tree.Fill()
            if (index % update_progressbar_index == 0):
                progress_bar.update(index)
        progress_bar.finish()

        out_file_root.Write()
        out_file_root.Close()


def convert_hit_table_fast(input_filename, output_filename):
    ''' Creates a ROOT Tree by looping over chunks of the hdf5 table. Some pointer magic is used to increase the conversion speed. Is 40x faster than convert_hit_table.

    Parameters
    ----------
    input_filename: string
        The file name of the hdf5 hit table.

    output_filename: string
        The filename of the created ROOT file

    '''

    with tb.open_file(input_filename, 'r') as in_file_h5:
        hits_table = in_file_h5.root.Hits

        out_file_root = TFile(output_filename, 'RECREATE')

        tree, chunk_size_tree = init_tree_from_table(hits_table, chunk_size)

        progress_bar = progressbar.ProgressBar(widgets=['', progressbar.Percentage(), ' ', progressbar.Bar(marker='*', left='|', right='|'), ' ', progressbar.ETA()], maxval=hits_table.shape[0], term_width=80)
        progress_bar.start()

        for index in range(0, hits_table.shape[0], chunk_size):
            hits = hits_table.read(start=index, stop=index + chunk_size)

            column_data = {}  # columns have to be in an additional python data container to prevent the carbage collector from deleting

            for branch in tree.GetListOfBranches():  # loop over the branches
                if branch.GetName() != 'n_entries':
                    column_data[branch.GetName()] = np.ascontiguousarray(hits[branch.GetName()])  # a copy has to be made to get the correct memory alignement
                    branch.SetAddress(column_data[branch.GetName()].data)  # get the column data pointer by name and tell its address to the tree

            if index + chunk_size > hits_table.shape[0]:  # decrease tree leave size for the last chunk
                chunk_size_tree.value = hits_table.shape[0] - index

            tree.Fill()
            progress_bar.update(index)

        out_file_root.Write()
        out_file_root.Close()


if __name__ == "__main__":
    # chose this parameter as big as possible to increase speed, but not too 
    # big otherwise program crashed:
    chunk_size = 50000  
    convert_two_hit_tables('/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/111_module_test_stop_mode_ext_trigger_scan_interpreted.h5', '/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/111_module_test_stop_mode_ext_trigger_scan_interpreted_fe55-calcdata.root')

    #convert_two_hit_tables('/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/87_module_test_fei4_self_trigger_scan_interpreted.h5', '/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/87_module_test_fei4_self_trigger_scan_interpreted.root')
    
