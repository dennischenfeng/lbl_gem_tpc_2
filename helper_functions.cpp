#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include <TMath.h> // from line3Dfit.C
#include <TGraph2D.h>
#include <TRandom2.h>
#include <TStyle.h>
#include <TF2.h>
#include <TH1.h>
#include <Math/Functor.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <Fit/Fitter.h>


#include <vector>

bool canConvertStringToPosInt(string str) { // can convert string to pos int
	// every character must be a digit; does not allow '-' or '.' or anything else
	for (int i = 0; i < str.size(); i++) {
	    if ((int) str[i] < 48 || (int) str[i] > 57) { // b/c (int)'0' == 48
	        return false;
	    }
	}

	return true;
}

int convertStringToPosInt(string str) {
	int result = 0;
	int digitPlace;

	for (int i = 0; i < str.size(); i++) {
		digitPlace = str.size() - i;
		result += (int)pow(10, digitPlace - 1) * ((int)str[i] - 48);
	}

	return result;
}

// old
int setReaderToEventNum(TTreeReader *reader, int eventNum) {
	/* Tries to set the entry number of reader to the first entry that has *event_number eventNum, as well as returning an int:

	Returns 1 if fully successful: found *event_number with eventNum, and set reader to that entry
	Returns 2 if mildly successful: found  an *event_number > eventNum, and set reader to that entry
	Returns -1 if not successful: eventNum is larger than all the *event_number's in the reader, and set reader to the last entry.

	*/  
	TTreeReaderValue<Long64_t> event_number(*reader, "event_number");

	reader->SetEntry(-1);
	while (reader->Next()) { 
        if (*event_number == eventNum) {
            return 1;
        } else if (*event_number > eventNum) {
        	return 2;
        }
    }

    return -1;
}

int getSMEventNum(Long64_t event_number, UChar_t relative_BCID) {
	/* Returns the SM-event number. (first one is 0)
	
	Note: h5 event = events are differentiated by event_number in h5 file) that marks the start of the SM event (events are differentiated by groups of 16 event_numbers
	*/

	return (int) (event_number / 16);
}

int getSMRelBCID(Long64_t event_number, UChar_t relative_BCID) {
	/* Returns the BCID number of the SM-event that this hit is in. (0 - 255)
	We call it the SM-RelBCID of the hit (stop mode - relative BCID), but it used to be called BCIDInEvent (so you may see it as BCIDInEvent in older code).
	
	Note: h5 event = events are differentiated by event_number in h5 file) that marks the start of the SM event (events are differentiated by groups of 16 event_numbers
	*/
	
	return (event_number - ((int)(event_number/16)) * 16) * 16 + relative_BCID; // Range: 0 - 255

}

// old
vector<int> getEntryRangeWithEventNumRange(TTreeReader *reader, int startEventNum, int endEventNum) {
	/* Given a desired range of h5 event nums, this script outputs a range of entry nums that correspond to those. If no entries had any of the event nums in the range, both elements of the returned array will be -1. If any error occurs, both elems will be -2.

	Returns an int array; index 1: start entry num; index 2 end entry num.
	Note that "entry" is the entry number in reader, and "event" is the h5 event num
	For both ranges we are dealing with, "start" means the first included number in the range, and "end" means the first NON-included number in the range.
	Ex: When startEventNum = 16, and endEventNum = 32, the range indicated is 16 through 31.

	Does not modify reader's entry (at the end of the function, reverts back to initial entry).
	*/
	int initialEntryNum = reader->GetCurrentEntry();
	vector<int> entryRange (2);

	TTreeReaderValue<Long64_t> event_number(*reader, "event_number");
	
	int setReaderToEventNum_start = setReaderToEventNum(reader, startEventNum);

	if (endEventNum <= startEventNum) {
		cout << "Error: endEventNum must be larger than startEventNum.\n";
		entryRange[0] = -2; // -2 indicates that there's an error
		entryRange[1] = -2;
	} else if (setReaderToEventNum_start == -1 || *event_number >= endEventNum) {
		entryRange[0] = -1; // -1 indicates no entries found with range specified
		entryRange[1] = -1;
	} else { // has at least entry within the range specified
		entryRange[0] = reader->GetCurrentEntry();
		reader->SetEntry(entryRange[0] - 1);

		bool reachedEnd = false; // reached end of ttree
		while (true) { // loop until one of the lines below break out of loop
			if (not reader->Next()) {
				reachedEnd = true;
				break;
			} else if (*event_number >= endEventNum) {
				break;
			}
		}

		if (!reachedEnd) {
			entryRange[1] = reader->GetCurrentEntry();
		} else {
			entryRange[1] = reader->GetCurrentEntry() + 1;
		}
	}

	reader->SetEntry(initialEntryNum);
	return entryRange;
}

int getEntryNumWithSMEventNum(TTreeReader *reader, int smEventNum) {
	/* Tries to get the entry number of reader to the first entry that has *SM_event_num smEventNum. Does not change the reader.

	Returns entry num if fully successful.
	Returns -2 if mildly successful: found  an *SM_event_num > smEventNum
	Returns -3 if not successful: smEventNum is larger than all the *SM_event_num's in the reader
	*/  

	int initialEntryNum = reader->GetCurrentEntry();
	int entryNum;
	TTreeReaderValue<Long64_t> SM_event_num(*reader, "SM_event_num");

	reader->SetEntry(-1);
	while (reader->Next()) { 
        if (*SM_event_num == smEventNum) {
            entryNum = reader->GetCurrentEntry();
            reader->SetEntry(initialEntryNum);
            return entryNum;
        } else if (*SM_event_num > smEventNum) {
            reader->SetEntry(initialEntryNum);
            return -2;
        }
    }

    reader->SetEntry(initialEntryNum);
    return -3;
}

// The following three code blocks are from line3Dfit.C from the ROOT tutorials folder
// define the parameteric line equation
void line(double t, const double *p, double &x, double &y, double &z) {
   // a parameteric line is define from 6 parameters but 4 are independent
   // x0,y0,z0,z1,y1,z1 which are the coordinates of two points on the line
   // can choose z0 = 0 if line not parallel to x-y plane and z1 = 1;
   x = p[0] + p[1]*t;
   y = p[2] + p[3]*t;
   z = t;
}

// calculate distance line-point
double distance2(double x,double y,double z, const double *p) {
   // distance line point is D= | (xp-x0) cross  ux |
   // where ux is direction of line and x0 is a point in the line (like t = 0)
   ROOT::Math::XYZVector xp(x,y,z);
   ROOT::Math::XYZVector x0(p[0], p[2], 0. );
   ROOT::Math::XYZVector x1(p[0] + p[1], p[2] + p[3], 1. );
   ROOT::Math::XYZVector u = (x1-x0).Unit();
   double d2 = ((xp-x0).Cross(u)) .Mag2();
   return d2;
}
bool first = true;


// function Object to be minimized
struct SumDistance2 {
   // the TGraph is a data member of the object
   TGraph2D * fGraph;

   SumDistance2(TGraph2D * g) : fGraph(g) {}

   // implementation of the function to be minimized
   double operator() (const double * par) {

      assert(fGraph    != 0);
      double * x = fGraph->GetX();
      double * y = fGraph->GetY();
      double * z = fGraph->GetZ();
      int npoints = fGraph->GetN();
      double sum = 0;
      for (int i  = 0; i < npoints; ++i) {
         double d = distance2(x[i],y[i],z[i],par);
         sum += d;
#ifdef DEBUG
         if (first) std::cout << "point " << i << "\t"
            << x[i] << "\t"
            << y[i] << "\t"
            << z[i] << "\t"
            << std::sqrt(d) << std::endl;
#endif
      }
      
      // if (first)
      // 	std::cout << "Total distance square w/ initial params: " << sum << std::endl;
      // 	first = false;
      return sum;
   }

};








void helper_functions() { // this function is for testing purposes
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/46_module_202_new_stop_mode_ext_trigger_scan_interpreted_raw.root");

	TTreeReader *reader = new TTreeReader("Table", f);

	TTreeReaderValue<UInt_t> h5_file_num(*reader, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(*reader, "event_number");
	
	int smEventNum = 12;
	int startEntryNum = getEntryNumWithSMEventNum(reader, smEventNum);
	int endEntryNum = getEntryNumWithSMEventNum(reader, smEventNum + 1);

	cout << startEntryNum << "   " << endEntryNum << "\n";
	cout << reader->GetCurrentEntry();
}