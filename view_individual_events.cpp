
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"

void view_individual_events() {
	/*** 

	Displays an 3D occupancy plot for each SM Event (stop mode event). The h5_file_num chosen must have working Hits and EventsCR files (_Hits.root and _EventsCR.root files).

	Can choose which SM event to start at. (find "CHOOSE THIS" in this script)
	***/
	gROOT->Reset();

	// Setting up files, treereaders, histograms
	string file_kind = "aggr"; // string that is either "aggr" or "non_aggr" to indicate whether or not its an aggregate file pair or not.
	int file_num_input = 16;
	string view_option = "3d"; // choose what to view:
	// "1" or "3d": view the events with their 3d reconstruction and line fit



	TFile *fileHits;
	TFile *fileEventsCR;
	if (file_kind.compare("non_aggr") == 0) {
		fileHits = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/" + to_string(file_num_input) + "_module_202_new_stop_mode_ext_trigger_scan_interpreted_Hits.root").c_str());
		fileEventsCR = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/" + to_string(file_num_input) + "_module_202_new_stop_mode_ext_trigger_scan_interpreted_EventsCR.root").c_str());
	} else if (file_kind.compare("aggr") == 0) {
		// fileHits = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/homemade_scripts/aggregate_data/1_module_202_new_AggrHits.root");
		fileHits = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/homemade_scripts/aggregate_data/" + to_string(file_num_input) + "_module_202_new_AggrHits.root").c_str());
		// fileEventsCR = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/homemade_scripts/aggregate_data/1_module_202_new_AggrEventsCR.root");
		fileEventsCR = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/homemade_scripts/aggregate_data/" + to_string(file_num_input) + "_module_202_new_AggrEventsCR.root").c_str());
	} else {
		cout << "Error: Input file_kind is not valid.";
	}

	TTreeReader *readerHits = new TTreeReader("Table", fileHits);
	TTreeReaderValue<UInt_t> h5_file_num_Hits(*readerHits, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(*readerHits, "event_number");
	TTreeReaderValue<UChar_t> tot(*readerHits, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(*readerHits, "relative_BCID");
	TTreeReaderValue<Long64_t> SM_event_num_Hits(*readerHits, "SM_event_num");
	TTreeReaderValue<UInt_t> SM_rel_BCID(*readerHits, "SM_rel_BCID");
	TTreeReaderValue<Double_t> x(*readerHits, "x");
	TTreeReaderValue<Double_t> y(*readerHits, "y");
	TTreeReaderValue<Double_t> z(*readerHits, "z");
	TTreeReaderValue<Double_t> s(*readerHits, "s");

	TTreeReader *readerEventsCR = new TTreeReader("Table", fileEventsCR);

	
	TTreeReaderValue<UInt_t> h5_file_num_EventsCR(*readerEventsCR, "h5_file_num");
	TTreeReaderValue<Long64_t> SM_event_num_EventsCR(*readerEventsCR, "SM_event_num");
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
	TTreeReaderValue<Double_t> zenith_angle(*readerEventsCR, "zenith_angle");
	TTreeReaderValue<UInt_t> duration(*readerEventsCR, "duration");

	// Initialize the canvas and graph1
	TCanvas *c1 = new TCanvas("c1","3D Occupancy for Specified SM Event", 1000, 10, 900, 1000);
	// c1->SetRightMargin(0.25);
	TPad *pad1 = new TPad("pad1", "pad1", 0, 0.5, 0.75, 1.0); // upper pad
	TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.5); // lower pad
	pad2->SetRightMargin(0.35);
	TH2F *h1 = new TH2F("h1", "2D Occupancy for ", 80, 0, 20, 336, -16.8, 0);


	bool quit = false; // if pressed q
	
	// Main Loop (loops for every entry in readerEventsCR
	while (readerEventsCR->Next() && !quit) {
		if (readerEventsCR->GetCurrentEntry() == 0 && file_kind.compare("non_aggr") == 0) {
			continue; // skip the entry num 0, because it probably contains no data
		}

		// Get startEntryNum_Hits and endEntryNum_Hits (for readerHits)
		vector<int> entryNumRange_include(2);
		entryNumRange_include = getEntryNumRangeWithH5FileNumAndSMEventNum(readerHits, *h5_file_num_EventsCR, *SM_event_num_EventsCR);
		if (entryNumRange_include[0] == -1) {
			cout << "Error: h5_file_num and SM_event_num should be able to be found in the Hits file.\n";
		}


		// If statement for choosing which graph1/h1 to view
		if (view_option.compare("3d") == 0 || view_option.compare("1") == 0) {
			
			TGraph2D *graph1 = new TGraph2D(); // create a new TGraph to refresh; the graph1 is the 3d plot, the h1 is the 2d plot.
			h1->Reset(); // must do reset for histograms, cannot create a new hist to refresh it

			// Fill graph1 and h1 with points and set title and axes
			readerHits->SetEntry(entryNumRange_include[0]);
			for (int i = 0; i < entryNumRange_include[1] - entryNumRange_include[0] + 1; i++) {
				graph1->SetPoint(i, (*x - 0.001), (*y + 0.001), (*z - 0.001));
				h1->Fill(*x, *y, *SM_rel_BCID); 
				readerHits->Next();
			}
			
			string graphTitle = "3D Reconstruction and Line Fit for h5FileNum " + to_string(*h5_file_num_EventsCR) + ", SMEventNum " + to_string(*SM_event_num_EventsCR);
			// graphTitle.append(to_string(*SM_event_num_EventsCR));
			graph1->SetTitle(graphTitle.c_str());
			graph1->GetXaxis()->SetTitle("x (mm)");
			graph1->GetYaxis()->SetTitle("y (mm)");
			graph1->GetZaxis()->SetTitle("z (mm)");
			graph1->GetXaxis()->SetLimits(0, 20); // ROOT is buggy, x and y use setlimits()
			graph1->GetYaxis()->SetLimits(-16.8, 0); // but z uses setrangeuser()
			graph1->GetZaxis()->SetRangeUser(0, 40.96); 
			c1->SetTitle(graphTitle.c_str());

			h1->GetXaxis()->SetTitle("x (mm)");
			h1->GetYaxis()->SetTitle("y (mm)");
			h1->GetZaxis()->SetTitle("SM Relative BCID (BCIDs)");

			// Draw the graph1 on pad1 (upper pad), and h1 on pad2
			c1->cd();
			pad1->Draw();
			pad1->cd();
			graph1->SetMarkerStyle(8);
			graph1->SetMarkerSize(0.5);
			graph1->Draw("pcol");

			c1->cd();
			pad2->Draw();
			pad2->cd();
			h1->Draw("COLZ");

			pad1->cd();

			// Display results, draw graph1 and line fit, only accept "good" events, get input
			cout << "h5 Event Num: " << *h5_file_num_EventsCR << "     SM Event Num: " << *SM_event_num_EventsCR;
			cout << "          Number of hits: " << *num_hits << "\n";

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

				cout << "Sum of squares div by DoF: " << *sum_squares_div_by_DoF;
			} else {
				cout << "Sum of squares div by DoF: FIT FAILED";
			}

			cout << "          Zenith angle: " << *zenith_angle << "\n";
			cout << "Duration: " << *duration << "\n";
			// cout << "Fraction inside sphere (1 mm radius): " << *fraction_inside_sphere << "\n";
			// cout << "Length of track:                      " << *length_track << "\n";
		} else if (view_option.compare("") == 0) {
			// @@@

		} else {
			cout << "Error: Input view_option is not valid.\n";
		}
		







		
		if (*duration > 150) { // won't show drawings or ask for input unless its a good event // CHOOSE THIS to show all events or only good events
			c1->Update(); // show all the drawings
			// handle input
			string inString = "";
			bool inStringValid = false;
            do {
	            cout << "<Enter>: next; 'b': previous; [number]: the nth SMEvent in the EventsCR file; 'q': quit.\n"; // b is for back
	            getline(cin, inString);

	            // Handles behavior according to input
	            if (inString.empty()) { // <Enter>
	            	// leave things be
					inStringValid = true;
	            } else if (inString.compare("b") == 0) {
					readerEventsCR->SetEntry(readerEventsCR->GetCurrentEntry() - 2);
					// smEventNum -= 2; // because it gets incremented once at the end of this do while loop
					inStringValid = true;
				} else if (inString.compare("q") == 0 || inString.compare(".q") == 0) {
					quit = true;
					inStringValid = true;
				} else if (canConvertStringToPosInt(inString)) {
					readerEventsCR->SetEntry(convertStringToPosInt(inString) - 1);
					// smEventNum = convertStringToPosInt(inString) - 1; // -1 because it gets incremented once at the end of this do while loop
					inStringValid = true;
				} // else, leave inStringValid as false, so that it asks for input again
			} while (!inStringValid);
		} else {
			cout << "\n";
		}

	}

	cout << "Exiting program.\n";
}