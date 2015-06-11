
void view_events_raw_old() {
	/*** Used only when input file is an _interpreted_raw.root file (called raw file). Opens a window that allows you to view events one by one. "Enter" to go to next event, "q" to quit. 
	You can specify which event number to start at. Ctrl-F "CHANGE THIS" in this script to find the line.

	Note: trigger_number is used for self-trigger scan data; trigger_time_stamp is for ext-trigger-stopmode scan data.
	***/
	gROOT->Reset(); 

	//--Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2/host/pybar/module_test/745_module_test_stop_mode_ext_trigger_scan_interpreted_raw.root");

	if (f == 0) { // if we cannot open the file, print an error message and return immediately
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

	//--Fill in the histogram and draw it
	TCanvas *c1 = new TCanvas("c1","Histograms");
	TH2F *h = new TH2F("h", "Location of Hits in Specified Event", 80, 0, 20, 336, -16.8, 0);
	h->GetXaxis()->SetTitle("x (mm)");
	h->GetYaxis()->SetTitle("y (mm)");
	h->SetMarkerStyle(7);
	

	int event_num = 1; //CHANGE THIS to start at whichever event you want
	char input[1];

	tr.SetEntry(0);
	while (tr.Next()) { //this is to set the entry number of tr to the first entry that has event number event_num
		if (*event_number >= event_num) {
			break;
		}
	}

	/*void set_to_first_entry_w_event_num(TTreeReader tr, int event_num) {
		tr.SetEntry(0);
		TTreeReaderValue<Long64_t> event_number(tr, "event_number");
		while (tr.Next()) { //this is to set the entry number of tr to the first entry that has event number event_num
			if (*event_number >= event_num) {
				break;
			}
		}
	}*/

	int maxNoEntries = 50; //max number of times it can print out "no entries for __"; after this, program stops
	int noEntriesCnt = 0; // count 
	
	// First event 
	while (tr.Next()) { //fills histogram with all its hits (hits with same event_num)
		if (*event_number == event_num) {
			h->Fill(*x, *y);
		} else {
			tr.SetEntry(tr.GetCurrentEntry() - 1);
			break;
		}
	}

	while (input[0] != 'q') {
		h->Reset();
		

		if (h->GetEntries()) {
			noEntriesCnt = 0;
			printf("event_num: %d\n", event_num);
			h->Draw();
			c1->Update();
			printf("'Enter' for next event, 'b' for previous event, <number> for specific event num, or 'q' to quit.\n");
			printf("1 event_num: %d\n", event_num); // @@@
			gets(input);  // "enter" is enough, root waits until you hit a key
			printf("2 event_num: %d\n", event_num); // @@@
			printf("3 *event_number: %d\n", *event_number); // @@@
		} else if (noEntriesCnt >= maxNoEntries) {
			break;
		} else {
			printf("No entries for event_num %d\n", event_num);
			noEntriesCnt += 1;
		}

		if (input[0] == 'b') {
			event_num = event_num - 1;
			printf("4 event_num: %d\n", event_num); // @@@
			printf("5 *event_number: %d\n", *event_number); // @@@
			tr.SetEntry(0);
			while (tr.Next()) { //this is to set the entry number of tr to the first entry that has event number event_num
				if (*event_number >= event_num) {
					break;
				}
			}
			printf("DONE"); // @@@
		} else if (isdigit(input[0])) {
			printf("DIGIT"); // @@@
		} else { // 'enter' or anything else
			event_num++;
		}
	}
}

