
void view_events_h5() {
/*** Only for ..._interpreted.h5 files. Opens a window that allows you to view events one by one. "Enter" to go to next event, "q" to quit. 
You can specify which event number to start at. Ctrl-F "CHANGE THIS" in this script to find the line.

Note: trigger_number is used for self-trigger scan data; trigger_time_stamp is for ext-trigger-stopmode scan data.
***/
gROOT->Reset(); 

//--Setting up file, treereader, histogram
TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/408_module_test_stop_mode_ext_trigger_scan_interpreted_raw.root");

if (f == 0) { // if we cannot open the file, print an error message and return immediately
	printf("Error: cannot open the root file!\n");
	return;
}

TTreeReader tReader("Table", f);

TTreeReaderValue<Long64_t> event_number(tReader, "event_number");
//TTreeReaderValue<UInt_t> trigger_number(tReader, "trigger_number"); //for self-trigger scan data
TTreeReaderValue<UInt_t> trigger_time_stamp(tReader, "trigger_time_stamp"); // for stopmode scan data
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
TCanvas *c1 = new TCanvas("c1","Histograms");
TH2F *h = new TH2F("h", "Location of Hits in Specified Event", 80, 0, 80, 336, 0, 336);
h->GetXaxis()->SetTitle("Column");
h->GetYaxis()->SetTitle("Row");
h->SetMarkerStyle(7);

int event_num = 1; //CHANGE THIS to start at whichever event you want
char input[1];

while (tReader.Next()) { //this is to set the entry number of tReader to the first entry that has event number event_num
	if (*event_number >= event_num) {
		tReader.SetEntry(tReader.GetCurrentEntry());
		break;
	}
}

maxNoEntries = 500; //max number of times it can print out "no entries for __"; after this, program stops
noEntriesCnt = 0; // count 
while (input[0] != 'q') {
	while (tReader.Next()) { //fills histogram with all its hits (hits with same event_num)
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
		noEntriesCnt = 0;
	} else if (noEntriesCnt >= maxNoEntries) {
		break;
	} else {
		printf("No entries for event_num %d\n", event_num);
		noEntriesCnt += 1;
	}

	event_num++;
	h->Reset();
}

}

