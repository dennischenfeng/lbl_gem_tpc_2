
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"

void view_SMEvents_3D_non_Aggr() {
	/*** Displays an 3D occupancy plot for each SM Event (stop mode event). The h5_file_num chosen must have working Hits and EventsCR files (_Hits.root and _EventsCR.root files).

	Can choose which SM event to start at. (find "CHOOSE THIS" in this script)
	***/
	gROOT->Reset();

	// Setting up files, treereaders, histograms
	int h5_file_num_input = 133;

	TFile *fileHits = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/" + to_string(h5_file_num_input) + "_module_202_new_stop_mode_ext_trigger_scan_interpreted_Hits.root").c_str());
	TFile *fileEventsCR = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/" + to_string(h5_file_num_input) + "_module_202_new_stop_mode_ext_trigger_scan_interpreted_EventsCR.root").c_str());

	TTreeReader *readerHits = new TTreeReader("Table", fileHits);
	TTreeReaderValue<UInt_t> h5_file_num(*readerHits, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(*readerHits, "event_number");
	TTreeReaderValue<UChar_t> tot(*readerHits, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(*readerHits, "relative_BCID");
	TTreeReaderValue<Long64_t> SM_event_num(*readerHits, "SM_event_num");
	TTreeReaderValue<Double_t> x(*readerHits, "x");
	TTreeReaderValue<Double_t> y(*readerHits, "y");
	TTreeReaderValue<Double_t> z(*readerHits, "z");
	TTreeReaderValue<Double_t> s(*readerHits, "s");

	TTreeReader *readerEventsCR = new TTreeReader("Table", fileEventsCR);
	TTreeReaderValue<UInt_t> num_hits(*readerEventsCR, "num_hits");
	TTreeReaderValue<UInt_t> sum_tots(*readerEventsCR, "sum_tots");
	TTreeReaderValue<Double_t> mean_x(*readerEventsCR, "mean_x");
	TTreeReaderValue<Double_t> mean_y(*readerEventsCR, "mean_y");
	TTreeReaderValue<Double_t> mean_z(*readerEventsCR, "mean_z");
	TTreeReaderValue<Double_t> line_fit_param0(*readerEventsCR, "line_fit_param0");
	TTreeReaderValue<Double_t> line_fit_param1(*readerEventsCR, "line_fit_param1");
	TTreeReaderValue<Double_t> line_fit_param2(*readerEventsCR, "line_fit_param2");
	TTreeReaderValue<Double_t> line_fit_param3(*readerEventsCR, "line_fit_param3");
	TTreeReaderValue<Double_t> sum_of_squares(*readerEventsCR, "sum_of_squares");

	TTreeReaderValue<UInt_t> event_status(*readerEventsCR, "event_status");
	TTreeReaderValue<Double_t> fraction_inside_sphere(*readerEventsCR, "fraction_inside_sphere");
	TTreeReaderValue<Double_t> length_track(*readerEventsCR, "length_track");
	TTreeReaderValue<Double_t> sum_tots_div_by_length_track(*readerEventsCR, "sum_tots_div_by_length_track");
	TTreeReaderValue<Double_t> sum_squares_div_by_DoF(*readerEventsCR, "sum_squares_div_by_DoF");

	// Initialize the canvas and graph
	TCanvas *c1 = new TCanvas("c1","3D Occupancy for Specified SM Event", 1000, 10, 900, 550);
	c1->SetRightMargin(0.25);
	TGraph2D *graph = new TGraph2D();

	// Variables used to loop the main loop
	bool endOfReader = false; // if reached end of the readerHits
	bool quit = false; // if pressed q
	int smEventNum = 1; // the current SM-event CHOOSE THIS to start at desired SM event number
	
	// Main Loop (loops for every smEventNum)
	while (!endOfReader && !quit) {
		// Variables used in this main loop
		int startEntryNum_Hits = 0; // for readerHits
		int endEntryNum_Hits = 0;
		int entryNum_EventsCR = 0; // for readerEventsCR
		string histTitle = "3D Occupancy for SM Event ";
		string inString = "";
		bool lastEvent = false;

		// Get startEntryNum_Hits and endEntryNum_Hits (for readerHits)
		startEntryNum_Hits = getEntryNumWithSMEventNum(readerHits, smEventNum);
		endEntryNum_Hits = getEntryNumWithSMEventNum(readerHits, smEventNum + 1);

		if (startEntryNum_Hits == -2) { // can't find the smEventNum
			cout << "Error: There should not be any SM event numbers that are missing." << "\n";
		} else if (startEntryNum_Hits == -3) { 
			endOfReader = true;
			break;
		} else if (endEntryNum_Hits == -3) { // assuming no SM event nums are skipped
			endEntryNum_Hits = readerHits->GetEntries(false);
			lastEvent = true;
		}

		// Fill TGraph with points and set title and axes
		graph = new TGraph2D(); // create a new TGraph to refresh

		readerHits->SetEntry(startEntryNum_Hits);
		for (int i = 0; i < endEntryNum_Hits - startEntryNum_Hits; i++) {
			graph->SetPoint(i, (*x - 0.001), (*y + 0.001), (*z - 0.001)); // @@@ z->s
			endOfReader = !(readerHits->Next());
		}

		histTitle.append(to_string(smEventNum));
		graph->SetTitle(histTitle.c_str());
		graph->GetXaxis()->SetTitle("x (mm)");
		graph->GetYaxis()->SetTitle("y (mm)");
		graph->GetZaxis()->SetTitle("z (mm)");

		graph->GetXaxis()->SetLimits(0, 20); // ROOT is buggy, x and y use setlimits()
		graph->GetYaxis()->SetLimits(-16.8, 0); // but z uses setrangeuser()
		graph->GetZaxis()->SetRangeUser(0, 40.96); 
		c1->SetTitle(histTitle.c_str());

		// Get entryNum_EventsCR and setreader to that entry
		entryNum_EventsCR = getEntryNumWithSMEventNum(readerEventsCR, smEventNum);
		readerEventsCR->SetEntry(entryNum_EventsCR);

		// Display results, draw graph and line fit, only accept "good" events, get input
		if (!endOfReader || lastEvent) {
			cout << "SM Event Num:   " << smEventNum << "\n";
			cout << "Number of hits: " << *num_hits << "\n";

			// Draw the graph
			graph->SetMarkerStyle(8);
			graph->SetMarkerSize(0.5);
			graph->Draw("pcol");

			// Draw the fitted line only if fit did not fail.
			if (*event_status != 1) {
				double fitParams[4];
				fitParams[0] = *line_fit_param0;
				fitParams[1] = *line_fit_param1;
				fitParams[2] = *line_fit_param2;
				fitParams[3] = *line_fit_param3;

				int n = 1000;
				double t0 = 0; // t is the z coordinate
				double dt = 40.96;
				TPolyLine3D *l = new TPolyLine3D(n);
				for (int i = 0; i <n;++i) {
				  double t = t0+ dt*i/n;
				  double x,y,z;
				  line(t,fitParams,x,y,z);
				  l->SetPoint(i,x,y,z);
				}
				l->SetLineColor(kRed);
				l->Draw("same");

				cout << "Sum of squares div by DoF: " << *sum_squares_div_by_DoF << "\n";
			} else {
				cout << "Sum of squares div by DoF: FIT FAILED\n";
			}

			cout << "Fraction inside sphere (1 mm radius): " << *fraction_inside_sphere << "\n";
			cout << "Length of track:                      " << *length_track << "\n";

			if (*event_status == 0) { // won't show drawings or ask for input unless its a good event // CHOOSE THIS to show all events or only good events
				c1->Update(); // show all the drawings
				// handle input
				bool inStringValid = false;
	            do {
		            cout << "<Enter>: next event; 'b': previous SM event; [number]: specific SM event number; 'q': quit.\n";
		            getline(cin, inString);

		            // Handles behavior according to input
		            if (inString.empty()) { // <Enter>
		            	// leave things be
						inStringValid = true;
		            } else if (inString.compare("b") == 0) {
						smEventNum -= 2; // because it gets incremented once at the end of this do while loop
						inStringValid = true;
					} else if (inString.compare("q") == 0 || inString.compare(".q") == 0) {
						quit = true;
						inStringValid = true;
					} else if (canConvertStringToPosInt(inString)) {
						smEventNum = convertStringToPosInt(inString) - 1; // -1 because it gets incremented once at the end of this do while loop
						inStringValid = true;
					} // else, leave inStringValid as false, so that it asks for input again
				} while (!inStringValid);
			} else {
				cout << "\n";
			}

		}
		smEventNum++;
	}

	cout << "Exiting program.\n";
}