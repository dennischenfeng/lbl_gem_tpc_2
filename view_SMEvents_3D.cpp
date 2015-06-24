
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"

void view_SMEvents_3D() {
	/*** Displays an 3D occupancy plot for each SM Event (stop mode event). The h5_file_num chosen must have working raw and CRCalc files (_raw.root and _CRCalc.root files).

	Can choose which SM event to start at. (find "CHOOSE THIS" in this script)
	Input file must be a raw file (_interpreted.root file).
	***/
	gROOT->Reset();

	// Setting up files, treereaders, histograms
	int h5_file_num_input = 101;

	TFile *fileRaw = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/" + to_string(h5_file_num_input) + "_module_202_new_stop_mode_ext_trigger_scan_interpreted_raw.root").c_str());
	TFile *fileCRCalc = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/" + to_string(h5_file_num_input) + "_module_202_new_stop_mode_ext_trigger_scan_interpreted_CRCalc.root").c_str());

	TTreeReader *readerRaw = new TTreeReader("Table", fileRaw);
	TTreeReaderValue<UInt_t> h5_file_num(*readerRaw, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(*readerRaw, "event_number");
	TTreeReaderValue<UChar_t> tot(*readerRaw, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(*readerRaw, "relative_BCID");
	TTreeReaderValue<Long64_t> SM_event_num(*readerRaw, "SM_event_num");
	TTreeReaderValue<Double_t> x(*readerRaw, "x");
	TTreeReaderValue<Double_t> y(*readerRaw, "y");
	TTreeReaderValue<Double_t> z(*readerRaw, "z");

	TTreeReader *readerCRCalc = new TTreeReader("Table", fileCRCalc);
	TTreeReaderValue<UInt_t> num_hits(*readerCRCalc, "num_hits");
	TTreeReaderValue<UInt_t> sum_tots(*readerCRCalc, "sum_tots");
	TTreeReaderValue<Double_t> mean_x(*readerCRCalc, "mean_x");
	TTreeReaderValue<Double_t> mean_y(*readerCRCalc, "mean_y");
	TTreeReaderValue<Double_t> mean_z(*readerCRCalc, "mean_z");
	TTreeReaderValue<Double_t> line_fit_param0(*readerCRCalc, "line_fit_param0");
	TTreeReaderValue<Double_t> line_fit_param1(*readerCRCalc, "line_fit_param1");
	TTreeReaderValue<Double_t> line_fit_param2(*readerCRCalc, "line_fit_param2");
	TTreeReaderValue<Double_t> line_fit_param3(*readerCRCalc, "line_fit_param3");
	TTreeReaderValue<Double_t> sum_of_squares(*readerCRCalc, "sum_of_squares");
	TTreeReaderValue<Double_t> fraction_inside_sphere(*readerCRCalc, "fraction_inside_sphere");

	// Initialize the canvas and graph
	TCanvas *c1 = new TCanvas("c1","3D Occupancy for Specified SM Event", 1000, 10, 900, 550);
	c1->SetRightMargin(0.25);
	TGraph2D *graph = new TGraph2D();

	// Variables used to loop the main loop
	bool endOfReader = false; // if reached end of the readerRaw
	bool quit = false; // if pressed q
	int smEventNum = 1; // the current SM-event CHOOSE THIS to start at desired SM event number
	
	// Main Loop (loops for every smEventNum)
	while (!endOfReader && !quit) {
		// Variables used in this main loop
		int startEntryNumRaw = 0; // for readerRaw
		int endEntryNumRaw = 0;
		int entryNumCRCalc = 0; // for readerCRCalc
		string histTitle = "3D Occupancy for SM Event ";
		string inString = "";
		bool lastEvent = false;

		// Get startEntryNumRaw and endEntryNumRaw (for readerRaw)
		startEntryNumRaw = getEntryNumWithSMEventNum(readerRaw, smEventNum);
		endEntryNumRaw = getEntryNumWithSMEventNum(readerRaw, smEventNum + 1);

		if (startEntryNumRaw == -2) { // can't find the smEventNum
			cout << "Error: There should not be any SM event numbers that are missing." << "\n";
		} else if (startEntryNumRaw == -3) { 
			endOfReader = true;
			break;
		} else if (endEntryNumRaw == -3) { // assuming no SM event nums are skipped
			endEntryNumRaw = readerRaw->GetEntries(false);
			lastEvent = true;
		}

		// Fill TGraph with points and set title and axes
		graph = new TGraph2D(); // create a new TGraph to refresh

		readerRaw->SetEntry(startEntryNumRaw);
		for (int i = 0; i < endEntryNumRaw - startEntryNumRaw; i++) {
			graph->SetPoint(i, (*x - 0.001), (*y + 0.001), (*z - 0.001));
			endOfReader = !(readerRaw->Next());
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

		// Get entryNumCRCalc and setreader to that entry
		entryNumCRCalc = getEntryNumWithSMEventNum(readerCRCalc, smEventNum);
		readerCRCalc->SetEntry(entryNumCRCalc);

		// Display results, draw graph and line fit, only accept "good" events, get input
		if (!endOfReader || lastEvent) {
			cout << "SM Event Num:   " << smEventNum << "\n";
			cout << "Number of hits: " << *num_hits << "\n";

			// Draw the graph
			graph->SetMarkerStyle(8);
			graph->SetMarkerSize(0.5);
			graph->Draw("pcol");

			// Draw the fitted line only if fit did not fail.
			if (!(*line_fit_param0 == 0 && *line_fit_param1 == 0 && *line_fit_param2 == 0 && *line_fit_param3 == 0)) {
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

				cout << "Sum of squares:            " << *sum_of_squares << "\n";
				cout << "Sum of squares div by DoF: " << *sum_of_squares / (*num_hits - 2) << "\n";
			} else {
				cout << "Sum of squares:            FIT FAILED\n";
				cout << "Sum of squares div by DoF: FIT FAILED\n";
			}

			cout << "Fraction inside sphere (1 mm radius): " << *fraction_inside_sphere << "\n";

			// Criteria to be a good event (if not good entry, then don't show)
			bool isGoodEvent = false;

			if (*num_hits >= 50 
				&& *sum_of_squares / *num_hits < 2.0 
				&& *fraction_inside_sphere < 0.8) {

				isGoodEvent = true;
			}

			// isGoodEvent = true;

			if (isGoodEvent) { // won't show drawings or ask for input unless its a good event
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