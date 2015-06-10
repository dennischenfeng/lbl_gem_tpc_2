{
	gROOT->Reset(); 

	//--Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/745_module_test_stop_mode_ext_trigger_scan_interpreted_raw.root");

	if (!f) { // if we cannot open the file, print an error message and return immediately
		printf("Error: cannot open the root file!\n");
		return;
	}

	TTreeReader tr("Table", f);

	TTreeReaderValue<UInt_t> h5_file_num(tr, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(tr, "event_number");
	TTreeReaderValue<UChar_t> tot(tr, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(tr, "relative_BCID");
	TTreeReaderValue<Double_t> x(tr, "x");
	TTreeReaderValue<Double_t> y(tr, "y");
	TTreeReaderValue<Double_t> z(tr, "z");

	// Initialize the histogram
	TCanvas *c1 = new TCanvas("c1","Histograms");
	TH2F *h = new TH2F("h", "Location of Hits in Specified Event", 80, 0, 20, 336, -16.8, 0);
	h->GetXaxis()->SetTitle("x (mm)");
	h->GetYaxis()->SetTitle("y (mm)");
	//h->SetMarkerStyle(7);

	int event_num = 1; //CHANGE THIS to start at whichever event you want
	char input[10];
	int maxNoEntries = 50; //max number of times it can print out "no entries for __"; after this, program stops
	int noEntriesCnt = 0; // count 
	bool quit = false; // if pressed q
	bool noMoreEvents = false; // for maxNoEntries
}