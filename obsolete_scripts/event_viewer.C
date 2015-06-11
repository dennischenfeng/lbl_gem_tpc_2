//#include "TFile.h" // we don't really need these as of right now
//#include "TTree.h"

void event_viewer() {
/*** Allows user to view events one-by-one, in order. Press 'return' key to display next event, and 'q' to quit.

***/
gROOT->Reset(); 

//--Setting up file, treereader, histogram
TFile *f = new TFile("/home/pixel/pybar/tags/2.0.1/host/pybar/module_test/91_module_test_fei4_self_trigger_scan_interpreted.root");

if (f == 0) { // if we cannot open the file, print an error message and return immediately
	printf("Error: cannot open the root file!\n");
	return;
}

TTreeReader tReader("Table", f);

TTreeReaderValue<Long64_t> event_number(tReader, "event_number");
TTreeReaderValue<UInt_t> trigger_number(tReader, "trigger_number");
TTreeReaderValue<UChar_t> relative_BCID(tReader, "relative_BCID");
TTreeReaderValue<UShort_t> LVL1ID(tReader, "LVL1ID");
TTreeReaderValue<UChar_t> column(tReader, "column");
TTreeReaderValue<UShort_t> row(tReader, "row");
TTreeReaderValue<UChar_t> tot(tReader, "tot");
TTreeReaderValue<UShort_t> BCID(tReader, "BCID");
TTreeReaderValue<UShort_t> TDC(tReader, "TDC");
TTreeReaderValue<UChar_t> TDC_time_stamp(tReader, "TDC_time_stamp");
TTreeReaderValue<UChar_t> trigger_status(tReader, "trigger_status");
TTreeReaderValue<UInt_t> service_record(tReader, "service_record");
TTreeReaderValue<UShort_t> event_status(tReader, "event_status");
TTreeReaderValue<Double_t> timestamp_start(tReader, "timestamp_start");
TTreeReaderValue<Double_t> timestamp_stop(tReader, "timestamp_stop");

//--Fill in the histogram and draw it

//total number of events in the TTree for scan91-  found to be: 21987

char input[1];
int event_num = 20000; //CHANGE THIS to start at whichever event you want
TCanvas *c1 = new TCanvas("c1","Histograms");
TH2F *h = new TH2F("h", "Location of Hits in Specified Event", 80, 0, 80, 336, 0, 336);
h->GetXaxis()->SetTitle("Column");
h->GetYaxis()->SetTitle("Row");
h->SetMarkerStyle(7);

while (tReader.Next()) { //this is to set the entry number of tReader to the first entry that has event number event_num
	if (*event_number == event_num) {
		tReader.SetEntry(tReader.GetCurrentEntry());
		break;
	}
}

while (input[0] != 'q') {
	while (tReader.Next()) { 
		if (*event_number == event_num) {
			h->Fill(*column, *row);
		} else {
			tReader.SetEntry(tReader.GetCurrentEntry() - 1);
			break;
		}
	}
	
	if (h->GetEntries()) {
		printf("event_num: %d\n", event_num);
		h->Draw();
		c1->Update();
		printf("Press 'return' key for next event, or type 'q' to quit.\n");
		gets(input);  // "return" is enough, root waits until you hit a key
	} else {
		printf("No entries for event_num %d\n", event_num);
	}

	event_num++;
	h->Reset();
}



}




















