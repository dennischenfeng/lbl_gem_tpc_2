
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"

void view_SMEvents_3D_TH1F() {
	/*** Displays an 3D occupancy plot for each SM Event. (stop mode event)

	Can choose which SM event to start at. (find "CHOOSE THIS" in this script)
	Input file must be a raw file (_interpreted.root file).
	***/
	gROOT->Reset(); 

	// Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/46_module_202_new_stop_mode_ext_trigger_scan_interpreted_raw.root");

	//TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/745_module_test_stop_mode_ext_trigger_scan_interpreted_raw.root");

	if (!f) { // if we cannot open the file, print an error message and return immediately
		cout << "Error: cannot open the root file!\n";
		//return;
	}

	TTreeReader *reader = new TTreeReader("Table", f);

	TTreeReaderValue<UInt_t> h5_file_num(*reader, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(*reader, "event_number");
	TTreeReaderValue<UChar_t> tot(*reader, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(*reader, "relative_BCID");
	TTreeReaderValue<Long64_t> SM_event_num(*reader, "SM_event_num");
	TTreeReaderValue<Double_t> x(*reader, "x");
	TTreeReaderValue<Double_t> y(*reader, "y");
	TTreeReaderValue<Double_t> z(*reader, "z");

	// Initialize the histogram
	//gStyle->SetCanvasPreferGL(true); // for drawing 3d
	TCanvas *c1 = new TCanvas("c1","3D Occupancy for Specified SM Event", 1000, 10, 900, 550);
	TH3F *h = new TH3F("h", "3D Occupancy for Specified SM Event", 80, 0, 20, 336, -16.8, 0, 256, 0, 40.96);
	h->GetXaxis()->SetTitle("x (mm)");
	h->GetYaxis()->SetTitle("y (mm)");
	h->GetZaxis()->SetTitle("z (mm)");
	c1->SetRightMargin(0.25);
	h->SetMarkerStyle(7);

	// Variables used in main loop
	bool endOfReader = false; // if reached end of the reader
	bool quit = false; // if pressed q
	int setReaderToEventNum_output; // returned value from calling that function
	int currSMEventNum = 0; // the current SM-event CHOOSE THIS to start at desired SM event number
	vector<int> entryRange (2); // 2 element vector: 1st is startEntryNum, 2nd is endEntryNum; see getEntryRangeWithEventNumRange() method in viewer_helper_functions.cpp for more details 
	string histTitle = "";
	int currBCIDInEvent; // current BCID of the SM-event (0 - 255)
	string inString;

	// Main Loop
	while (!endOfReader && !quit) {
		// Refresh these variables
		setReaderToEventNum_output = 0;
		entryRange[0] = 0;
		entryRange[1] = 0;
		histTitle = "3D Occupancy for SM Event ";
		currBCIDInEvent = 0;
		inString = "";

		// Setting reader to the proper entry
		setReaderToEventNum_output = setReaderToEventNum(reader, currSMEventNum * 16);
		if (setReaderToEventNum_output == 2) {
			currSMEventNum = getSMEventNum(*event_number, *relative_BCID); // to make sure that currSMEventNum is set at the correct value
		} else if (setReaderToEventNum_output == -1) {
			endOfReader = true;
		}

		// Fill histogram with each group of 16 h5 events (aka 1 SM event)
		if (!endOfReader) {
			entryRange = getEntryRangeWithEventNumRange(reader, currSMEventNum * 16, (currSMEventNum + 1) * 16); // array[2] that holds startEntryNum and endEntryNum
			// if entryRange is [-1, -1], then the following for loop simply doesn't loop, which is what is intended to happen
			// if entryRange is [-2, -2], then error message will be printed, and you should fix the problem right away.

			// Fill and draw histogram
			h->Reset();
			histTitle.append(to_string(currSMEventNum));
			h->SetTitle(histTitle.c_str());
			c1->SetTitle(histTitle.c_str());
			for (int i = 0; i < entryRange[1] - entryRange[0]; i++) {
				currBCIDInEvent = getSMRelBCID(*event_number, *relative_BCID);
				//
				h->Fill(*x, *y, *z);
				endOfReader = !(reader->Next());
			}
		}

		// Draw histogram, get input, and set currSMEventNum properly (according to input)
		if (!endOfReader && h->GetEntries() != 0) {
			cout << "Current SM Event Number: " << currSMEventNum << "\n";
			//h->SetMarkerStyle(7);
			h->Draw();
			c1->Update();

			bool inStringValid = false;
            do {
	            cout << "<Enter> for next event, 'b' for previous SM event, [number] for specific SM event number, or 'q' to quit.\n";
	            getline(cin, inString);

	            // Handles behavior according to input
	            if (inString.empty()) { // <Enter>
	            	// leave things be
					inStringValid = true;
	            } else if (inString.compare("b") == 0) {
					currSMEventNum -= 2; // because it gets incremented once at the end of this do while loop
					inStringValid = true;
				} else if (inString.compare("q") == 0) {
					quit = true;
					inStringValid = true;
				} else if (canConvertStringToPosInt(inString)) {
					currSMEventNum = convertStringToPosInt(inString) - 1; // -1 because it gets incremented once at the end of this do while loop
					inStringValid = true;
				} // else, leave inStringValid as false, so that it asks for input again
			} while (!inStringValid);
		}
		currSMEventNum++;
	}

	cout << "Exiting program.\n";
}