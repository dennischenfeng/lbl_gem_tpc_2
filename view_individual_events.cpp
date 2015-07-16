
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

	Note: to add a new view_option:
	- add an info comment about it under "choose what to view"
	- if its a histogram, initialize the hist right before the main loop
	- reset hist
	- under the for loop, fill in the hist
	- add another "else if" statement, to draw the hist

	***/
	gROOT->Reset();

	// Setting up files, treereaders, histograms
	string file_kind = "aggr"; // string that is either "aggr" or "non_aggr" to indicate whether or not its an aggregate file pair or not.
	int file_num_input = 25;
	string view_option = "4"; // choose what to view:
	// "1" or "3d": view the events with their 3d reconstruction and line fit
	// "2" or "SM_rel_BCID": numHits per SMRelBCID with the 3d reconstruction
	// "3" or "sum_tots_per_length": running sum of tots per 2mm
	// "4" or "avg_d2_per_length": running average of d squared per 2 mm


	TFile *fileHits;
	TFile *fileEventsCR;
	if (file_kind.compare("non_aggr") == 0) {
		fileHits = new TFile(("/home/pixel/pybar/pybar_github/pybar/module_1/" + to_string(file_num_input) + "_module_1_stop_mode_ext_trigger_scan_interpreted_Hits.root").c_str());
		fileEventsCR = new TFile(("/home/pixel/pybar/pybar_github/pybar/module_1/" + to_string(file_num_input) + "_module_1_stop_mode_ext_trigger_scan_interpreted_EventsCR.root").c_str());
	} else if (file_kind.compare("aggr") == 0) {
		// fileHits = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/homemade_scripts/aggregate_data/1_module_202_new_AggrHits.root");
		fileHits = new TFile(("/home/pixel/pybar/pybar_github/pybar/homemade_scripts/aggregate_data/" + to_string(file_num_input) + "_module_1_AggrHits.root").c_str());
		// fileEventsCR = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/homemade_scripts/aggregate_data/1_module_202_new_AggrEventsCR.root");
		fileEventsCR = new TFile(("/home/pixel/pybar/pybar_github/pybar/homemade_scripts/aggregate_data/" + to_string(file_num_input) + "_module_1_AggrEventsCR.root").c_str());
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
	TTreeReaderValue<Double_t> d(*readerHits, "d");

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

	// Initialize the canvas and graph_3d
	TCanvas *c1 = new TCanvas("c1","3D Occupancy for Specified SM Event", 1000, 10, 900, 1000);
	// c1->SetRightMargin(0.25);
	TPad *pad1 = new TPad("pad1", "pad1", 0, 0.5, 1, 1.0); // upper pad
	pad1->SetRightMargin(0.25);
	TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.5); // lower pad
	// pad2->SetRightMargin(0.35);
	c1->cd();

	TH2F *h_2d_occupancy = new TH2F("h_2d_occupancy", "2D Occupancy", 80, 0, 20, 336, -16.8, 0);
	h_2d_occupancy->GetXaxis()->SetTitle("x (mm)");
	h_2d_occupancy->GetYaxis()->SetTitle("y (mm)");
	h_2d_occupancy->GetZaxis()->SetTitle("SM Relative BCID (BCIDs)");

	TH1F *h_SM_rel_BCID = new TH1F("h_SM_rel_BCID", "Num Hits per SMRelBCID", 256, 0, 256);
	h_SM_rel_BCID->GetXaxis()->SetTitle("Stop Mode Relative BCID (BCIDs)");
	h_SM_rel_BCID->GetYaxis()->SetTitle("Count (hits)");

	TH1F *h_sum_tots_per_length = new TH1F("h_sum_tots_per_length", "SumTots of hits within 2 mm intervals", 400, -40, 40); // intervals
	h_sum_tots_per_length->GetXaxis()->SetTitle("s (mm)");
	h_sum_tots_per_length->GetYaxis()->SetTitle("Sum of ToTs (units of ToT)");

	

	bool quit = false; // if pressed q
	
	// Main Loop (loops for every entry in readerEventsCR)
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


		// If statement for choosing which graph_3d/h_2d_occupancy to view
		
			
		TGraph2D *graph_3d = new TGraph2D(); // create a new TGraph to refresh; the graph_3d is the 3d plot, the h_2d_occupancy is the 2d plot.
		h_2d_occupancy->Reset(); // must do reset for histograms, cannot create a new hist to refresh it
		h_SM_rel_BCID->Reset();
		h_sum_tots_per_length->Reset();
		TGraph *graph_avg_d2_per_length = new TGraph();

		// Fill in graphs and hists with points
		readerHits->SetEntry(entryNumRange_include[0]);
		double minS = *s; // the lowest s in the event
		double maxS = *s + *length_track; // the highest s in the event
		for (int i = 0; i < entryNumRange_include[1] - entryNumRange_include[0] + 1; i++) {
			graph_3d->SetPoint(i, (*x - 0.001), (*y + 0.001), (*z - 0.001));
			
			h_2d_occupancy->Fill(*x, *y, *SM_rel_BCID);
			h_SM_rel_BCID->Fill(*SM_rel_BCID);

			// fill in for h_sum_tots_per_length
			for (double currS = *s - 1.0; currS <= *s + 1.0; currS += 0.2) { // set increment and interval
				if (currS >= minS + 1.0 && currS <= maxS - 1.0) { // set cut offs
					h_sum_tots_per_length->Fill(currS, *tot);
				}
			}
			readerHits->Next();
		}

		// Fill in graphs (using a slightly different method of looping) (this method was originally for graph_avg_d2_per_length)
		int graph_index = 0; // index for setting points in the graphs
		for (double currS = minS + 1.0; currS <= maxS - 1.0; currS += 0.2, graph_index++) {// set increment and cut offs

			double currSumD2 = 0; // current sum of d squareds
			int numHitsInInterval = 0; // number of hits in the interval
			readerHits->SetEntry(entryNumRange_include[0]);
			for (int i = 0; i < entryNumRange_include[1] - entryNumRange_include[0] + 1; i++) {
				if (*s >= currS - 1.0) { // set interval
					if (*s <= currS + 1.0) {	
						currSumD2 += (*d); // @@@ square this
						numHitsInInterval++;
					} else {
						break;
					}
				}

				readerHits->Next();
			}
			graph_avg_d2_per_length->SetPoint(graph_index, currS, currSumD2/numHitsInInterval);
		}

		// @@@ 
		for ()
		cout<< "GRAPH AVG D2: " << graph_avg_d2_per_length->GetN() << "\n"; // @@@

		// Set title and axes
		string graphTitle = "3D Reconstruction and Line Fit for h5FileNum " + to_string(*h5_file_num_EventsCR) + ", SMEventNum " + to_string(*SM_event_num_EventsCR);
		// graphTitle.append(to_string(*SM_event_num_EventsCR));
		graph_3d->SetTitle(graphTitle.c_str());
		graph_3d->GetXaxis()->SetTitle("x (mm)");
		graph_3d->GetYaxis()->SetTitle("y (mm)");
		graph_3d->GetZaxis()->SetTitle("z (mm)");
		graph_3d->GetXaxis()->SetLimits(0, 20); // ROOT is buggy, x and y use setlimits()
		graph_3d->GetYaxis()->SetLimits(-16.8, 0); // but z uses setrangeuser()
		graph_3d->GetZaxis()->SetRangeUser(0, 136.533); 
		c1->SetTitle(graphTitle.c_str());

		graph_avg_d2_per_length->SetTitle("Avg d squared of hits within 2 mm intervals");
		graph_avg_d2_per_length->GetXaxis()->SetTitle("s (mm)");
		graph_avg_d2_per_length->GetYaxis()->SetTitle("Avg d squared (mm^2)");
		graph_avg_d2_per_length->GetXaxis()->SetLimits(-40, 40); // ROOT is buggy, x and y use setlimits()
		graph_avg_d2_per_length->GetYaxis()->SetRangeUser(0, 10); 



		// Draw the graph_3d on pad1 (upper pad)
		c1->cd();
		pad1->Draw();
		pad1->cd();
		graph_3d->SetMarkerStyle(8);
		graph_3d->SetMarkerSize(0.5);
		graph_3d->Draw("pcol");

		// Draw other histogram on pad2
		c1->cd();
		pad2->Draw();
		pad2->cd();
		if (view_option.compare("3d") == 0 || view_option.compare("1") == 0) {
			pad2->SetRightMargin(0.35);
			h_2d_occupancy->Draw("COLZ");
		} else if (view_option.compare("SM_rel_BCID") == 0 || view_option.compare("2") == 0) {
			pad2->SetRightMargin(0.25);
			h_SM_rel_BCID->Draw("COLZ");
		} else if (view_option.compare("sum_tots_per_length") == 0 || view_option.compare("3") == 0) {
			pad2->SetRightMargin(0.25);
			h_sum_tots_per_length->Draw("BAR");
		} else if (view_option.compare("avg_d2_per_length") == 0 || view_option.compare("4") == 0) {
			pad2->SetRightMargin(0.25);
			graph_avg_d2_per_length->SetMarkerStyle(8);
			graph_avg_d2_per_length->SetMarkerSize(0.5);
			graph_avg_d2_per_length->Draw("AB");
		} else {
			cout << "Error: Input view_option is not valid.\n";
		}
		pad1->cd();

		// Display results, draw graph_3d and line fit
		if (file_kind.compare("aggr") == 0) {
			cout << "Aggr EventsCR Entry Num: " << readerEventsCR->GetCurrentEntry();
		}

		cout << "     h5 Event Num: " << *h5_file_num_EventsCR << "     SM Event Num: " << *SM_event_num_EventsCR << "\n";
		// cout << "          Number of hits: " << *num_hits << "\n";

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
		cout << "Length of track: " << *length_track << "\n";
		cout << "SumTots/Length: " << *sum_tots_div_by_length_track << "\n";
		cout << "Event Status: " << *event_status << "\n";
		


		// if (view_option.compare("3d") == 0 || view_option.compare("1") == 0) {

		// } else if (view_option.compare("SM_rel_BCID") == 0 || view_option.compare("2") == 0) {
		// 	// // Reset histogram
		// 	// h_SM_rel_BCID->Reset();

		// 	// // For every hit, fill in the histogram with the SM_rel_BCID
		// 	// readerHits->SetEntry(entryNumRange_include[0]);
		// 	// for (int i = 0; i < entryNumRange_include[1] - entryNumRange_include[0] + 1; i++) {
				
		// 	// 	h_SM_rel_BCID->Fill(*SM_rel_BCID); 
		// 	// 	readerHits->Next();
		// 	// }

		// 	// // Draw the hist
		// 	// c1->cd();
		// 	// pad1->Draw();
		// 	// pad1->cd();
		// 	// h_SM_rel_BCID->Draw();

		// 	// // Print info lines
		// 	// if (file_kind.compare("aggr") == 0) {
		// 	// 	cout << "Aggr EventsCR Entry Num: " << readerEventsCR->GetCurrentEntry();
		// 	// }
		// 	// cout << "     h5 Event Num: " << *h5_file_num_EventsCR << "     SM Event Num: " << *SM_event_num_EventsCR << "\n";
		// } else {
		// 	cout << "Error: Input view_option is not valid.\n";
		// }
		







		// Ask for input
		if (true) { // won't show drawings or ask for input unless its a good event // CHOOSE THIS to show all events or only good events
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