
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"


// Helper functions
int getSMEventNum(Long64_t event_number, UChar_t relative_BCID) {
	/* Returns the SM-event number. 
	
	Note: h5 event = events are differentiated by event_number in h5 file) that marks the start of the SM event (events are differentiated by groups of 16 event_numbers
	*/

	return (int) (event_number / 16);
}

int getCurrBCID(Long64_t event_number, UChar_t relative_BCID) {
	/* Returns the current BCID number of the SM-event that this hit is in. 
	
	Note: h5 event = events are differentiated by event_number in h5 file) that marks the start of the SM event (events are differentiated by groups of 16 event_numbers
	*/
	
	return (event_number - ((int)(event_number/16)) * 16) * 16 + relative_BCID; // Range: 0 - 255
}

void view_durations_of_all_SMEvents() {
	/*** Used only when input file is an _interpreted_raw.root file (called raw file). Displays a 1D histogram of Count (SM events) vs Duration of SM event (BCIDs). There's 1 histogram for the entire raw file (many SM events). 

	Original purpose: Record the average duration of SM events, and see if it changes when I modify the clock speed.

	(Originally named view_BCIDsPerSMEvent).
	***/
	gROOT->Reset(); 

	//--Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/46_module_202_new_stop_mode_ext_trigger_scan_interpreted_raw.root");

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
	TCanvas *c1 = new TCanvas("c1","Histogram");
	TH1F *h = new TH1F("h", "Durations of all SM events", 60, 0, 60);
	h->GetXaxis()->SetTitle("Duration of SM event (BCIDs)");
	h->GetYaxis()->SetTitle("Count (SM events)");
	//h->SetMarkerStyle(7);

	// Variables used in loop
	int bcidsInCurrEvent = 0; // BCIDs in current event
	int smEventNum = -1; // the current SM-event
	int currBCID = -1; // current BCID of the SM-event (0 - 255)
	int smEventNum_f = 0; // the current SM-event FROM HELPER FUNCTION
	int currBCID_f = 0; // current BCID of the SM-event (0 - 255) FROM HELPER FUNCTION

	while (reader.Next()) {
		
		smEventNum_f = getSMEventNum(*event_number, *relative_BCID);
		currBCID_f = getCurrBCID(*event_number, *relative_BCID);


		if (smEventNum_f == smEventNum && \
			currBCID_f > currBCID) { // its a new bcid

			bcidsInCurrEvent++;
			currBCID = currBCID_f;
		} else if (smEventNum_f > smEventNum) { // new SM-event
			h->Fill(bcidsInCurrEvent);
			smEventNum = smEventNum_f;
			currBCID = -1;
			bcidsInCurrEvent = 1;
		}
	}
	h->Fill(bcidsInCurrEvent); // fill for the last SM-event

	h->Draw("COLZ");
    c1->Update();
}