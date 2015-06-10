
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "viewer_helper_functions.cpp"

void view_events_raw() {
	/*** Used only when input file is an _interpreted_raw.root file (called raw file). Opens a window that allows you to view h5-events one by one. "Enter" to go to next event, 'b' for previous event, <number> for specific event num, or 'q' to quit.
	You can specify which event number to start at. Ctrl-F "CHANGE THIS" in this script to find the line.

	Note: trigger_number is used for self-trigger scan data; trigger_time_stamp is for ext-trigger-stopmode scan data.
	***/
	gROOT->Reset(); 

	// Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/46_module_202_new_stop_mode_ext_trigger_scan_interpreted_raw.root");

	//TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/745_module_test_stop_mode_ext_trigger_scan_interpreted_raw.root");

	if (!f) { // if we cannot open the file, print an error message and return immediately
		cout << "Error: cannot open the root file!\n";
		//return;
	}

	TTreeReader reader("Table", f);

	TTreeReaderValue<UInt_t> h5_file_num(reader, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(reader, "event_number");
	TTreeReaderValue<UChar_t> tot(reader, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(reader, "relative_BCID");
	TTreeReaderValue<Double_t> x(reader, "x");
	TTreeReaderValue<Double_t> y(reader, "y");
	TTreeReaderValue<Double_t> z(reader, "z");

	// Initialize the histogram
	TCanvas *c1 = new TCanvas("c1","Occupancy for Specified h5 Event");
	TH2F *h = new TH2F("h", "Occupancy for Specified h5 Event", 80, 0, 20, 336, -16.8, 0);
	h->GetXaxis()->SetTitle("x (mm)");
	h->GetYaxis()->SetTitle("y (mm)");
	//h->SetMarkerStyle(7);

	// Variables used in main loop
	int currEventNum = 1; //CHANGE THIS to start at whichever event you want
	string inString;
	bool endOfEvent = false;
	bool endOfTree = false;
	Long64_t nextEventNum = 0;
	int deltaEventNum = 0; // change in event num, nextEventNum - *event_number
	bool quit = false; // if pressed q

	// Set the entry number correctly at the start (before the main loop)
	while (reader.Next()) { //this is to set the entry number of reader to the first entry that has event number currEvent_num
        if (*event_number >= currEventNum) {
            break;
        }
    }

	// Main Loop
	while (reader.Next() && !quit) {
		if (*event_number < currEventNum) { // check for error
			cout << "Error: *event_number should not be smaller than currEventNum.\n";
		}

		// Reset the histogram if previous entry had reached the end of an event
		if (endOfEvent) {
			h->Reset();
			endOfEvent = false;
		}

		h->Fill(*x, *y);

		// Check if next entry exists, and assign to nextEventNum
		endOfTree = !reader.Next();
		if (!endOfTree) {
			nextEventNum = *event_number;
			reader.SetEntry(reader.GetCurrentEntry() - 1);
		} else {
			nextEventNum = *event_number + 1; // ad hoc solution to make the program run through main loop one last time
			// leave reader be; it should be set at the last valid entry number
		}

		// If the next entry is the start of a new event, do the following: Draw histogram, print out curr event num and prompt user for input (pausing the program), increment reader and currEventNum depending on the input 
		if (nextEventNum != *event_number) {
			deltaEventNum = nextEventNum - *event_number;
			currEventNum = nextEventNum;
			endOfEvent = true;
 
			h->SetTitle("Occupancy of Specified Event");
			h->Draw("COLZ");
            c1->Update();

            cout << "Current h5 Event Number: " << *event_number << "\n";
            
            bool inStringValid = false;
            do {
	            cout << "<Enter> for next event, 'b' for previous event, [number] for specific event_number, or 'q' to quit.\n";
	            getline(cin, inString);

	            // Handles behavior according to input
	            if (inString.empty()) { // <Enter>
	            	// Displays "no hits for _" messages, if any
	            	if (deltaEventNum == 2) {
						cout << "No hits for event_numbers " << *event_number + 1 << "\n";
					} else if (deltaEventNum > 2) {
						cout << "No hits for event_numbers " << *event_number + 1 << " through " << nextEventNum - 1 << "\n";
					} else if (deltaEventNum < 0) {
						cout << "Error: nextEventNum should be equal to or greater than (current) event_number.\n";
					}

					inStringValid = true;
	            } else if (inString.compare("b") == 0) {
					currEventNum--;
					reader.SetEntry(0);
					while (reader.Next()) { //this is to set the entry number of tr to the first entry that has event number currEventNum
						if (*event_number >= currEventNum) {
							break;
						}
					}

					inStringValid = true;
				} else if (inString.compare("q") == 0) {
					quit = true;
					inStringValid = true;
				} else if (canConvertStringToPosInt(inString)) {
					// @@@ try catch block here
					currEventNum = convertStringToPosInt(inString);
					reader.SetEntry(0);
					while (reader.Next()) { //this is to set the entry number of tr to the first entry that has event number currEventNum
						if (*event_number >= currEventNum) {
							break;
						}
					}

					inStringValid = true;
				} 
			} while (!inStringValid);
		}
	}

	cout << "Exited program.\n";
}