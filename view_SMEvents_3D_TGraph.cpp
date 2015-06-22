
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"

void view_SMEvents_3D_TGraph() {
	/*** Displays an 3D occupancy plot for each SM Event. (stop mode event)

	Can choose which SM event to start at. (find "CHOOSE THIS" in this script)
	Input file must be a raw file (_interpreted.root file).
	***/
	gROOT->Reset();

	// Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/89_module_202_new_stop_mode_ext_trigger_scan_interpreted_raw.root");

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
	// gStyle->SetCanvasPreferGL(true); // for drawing 3d (very slow)
	TCanvas *c1 = new TCanvas("c1","3D Occupancy for Specified SM Event", 1000, 10, 900, 550);
	// TH3F *h = new TH3F("h", "3D Occupancy for Specified SM Event", 80, 0, 20, 336, -16.8, 0, 256, 0, 40.96);
	// h->GetXaxis()->SetTitle("x (mm)");
	// h->GetYaxis()->SetTitle("y (mm)");
	// h->GetZaxis()->SetTitle("z (mm)");
	// h->SetMarkerStyle(7);
	
	TGraph2D *graph = new TGraph2D();
	// graph->SetPoint(0,1,1,1); // dummy point; I just need to first add a point in order for a lot of the methods to function properly (ex: GetXAxis())

	// graph->SetNameTitle("graph", "3D Occupancy for Specified SM Event");
	// graph->GetXaxis()->SetTitle("x (mm)");
	// graph->GetYaxis()->SetTitle("y (mm)");
	// graph->GetZaxis()->SetTitle("z (mm)");

	// graph->GetXaxis()->SetLimits(0, 20); // ROOT is buggy, x and y use setlimits() 
	// graph->GetYaxis()->SetLimits(-16.8, 0); // but z uses setrangeuser()
	// graph->GetZaxis()->SetRangeUser(0, 40.96);
	
	c1->SetRightMargin(0.25);
	

	// Variables used to loop the main loop
	bool endOfReader = false; // if reached end of the reader
	bool quit = false; // if pressed q
	int smEventNum = 1; // the current SM-event CHOOSE THIS to start at desired SM event number

	
	// Main Loop (loops for every smEventNum)
	while (!endOfReader && !quit) {
		// Variables used in this main loop
		int startEntryNum = 0;
		int endEntryNum = 0;
		string histTitle = "3D Occupancy for SM Event ";
		string inString = "";
		bool fitFailed = false; // true if the 3D fit failed
		bool lastEvent = false;

		// Declaring the important values for the current graph and/or line fit
		int numEntries = 0;
		double sumSquares = 0;

		// Get startEntryNum and endEntryNum
		startEntryNum = getEntryNumWithSMEventNum(reader, smEventNum);
		endEntryNum = getEntryNumWithSMEventNum(reader, smEventNum + 1);

		if (startEntryNum == -2) { // can't find the smEventNum
			cout << "Error: There should not be any SM event numbers that are missing." << "\n";
		} else if (startEntryNum == -3) { 
			endOfReader = true;
			break;
		} else if (endEntryNum == -3) { // assuming no SM event nums are skipped
			endEntryNum = reader->GetEntries(false);
			lastEvent = true;
		}

		// Fill TGraph with points, set title and axes
		graph = new TGraph2D(); // create a new TGraph to refresh

		reader->SetEntry(startEntryNum);
		for (int i = 0; i < endEntryNum - startEntryNum; i++) {
			graph->SetPoint(i, (*x - 0.001), (*y + 0.001), (*z - 0.001));
			endOfReader = !(reader->Next());
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

		// 3D Fit, display results, draw graph and line fit, get input, and set smEventNum properly (according to input)
		if (!endOfReader || lastEvent) {
			// Draw graph
			numEntries = graph->GetN();
			cout << "Current SM Event Number: " << smEventNum << "\n";
			cout << "Number of entries:       " << numEntries << "\n";

			// Getting decent starting parameters for the fit - do two 2D fits (one for x vs z, one for y vs z)
			TGraph *graphZX = new TGraph();
			TGraph *graphZY = new TGraph();
			reader->SetEntry(startEntryNum);
			for (int i = 0; i < endEntryNum - startEntryNum; i++) {
				graphZX->SetPoint(i, (*z - 0.001), (*x + 0.001));
				graphZY->SetPoint(i, (*z - 0.001), (*y + 0.001));
				endOfReader = !(reader->Next());
			}
			TFitResultPtr fitZX = graphZX->Fit("pol1", "WQS"); // w for ignore error of each pt, q for quiet (suppress results output), s for return a tfitresultptr
			TFitResultPtr fitZY = graphZY->Fit("pol1", "WQS");
			Double_t param0 = fitZX->GetParams()[0];
			Double_t param1 = fitZX->GetParams()[1];
			Double_t param2 = fitZY->GetParams()[0];
			Double_t param3 = fitZY->GetParams()[1];

			// draw the lines for the two 2D fits
			// int n = 2;
			// TPolyLine3D *lineZX = new TPolyLine3D(n);
			// TPolyLine3D *lineZY = new TPolyLine3D(n);
			// lineZX->SetPoint(0, param0, 0, 0);
			// lineZX->SetPoint(1, param0 + param1 * 40.96, 0, 40.96);
			// lineZX->SetLineColor(kBlue);
			// lineZX->Draw("same");
			// lineZY->SetPoint(0, 0, param2, 0);
			// lineZY->SetPoint(1, 0, param2 + param3 * 40.96, 40.96);
			// lineZY->SetLineColor(kGreen);
			// lineZY->Draw("same");


			// 3D FITTING CODE (based on line3Dfit.C)
			ROOT::Fit::Fitter  fitter;

		   	SumDistance2 sdist(graph);
#ifdef __CINT__
		   	ROOT::Math::Functor fcn(&sdist,4,"SumDistance2");
#else
		   	ROOT::Math::Functor fcn(sdist,4);
#endif
			// set the function and the initial parameter values
			double pStart[4] = {param0,param1,param2,param3};
			fitter.SetFCN(fcn,pStart);
			// set step sizes different than default ones (0.3 times parameter values)
			for (int i = 0; i < 4; ++i) fitter.Config().ParSettings(i).SetStepSize(0.01);

			bool ok = fitter.FitFCN();
			if (!ok) {
			  Error("line3Dfit","Line3D Fit failed");
			  fitFailed = true;
			} else {
				const ROOT::Fit::FitResult & result = fitter.Result();
				sumSquares = result.MinFcnValue();
				std::cout << "Sum of distance squares:  " << sumSquares << std::endl;
				std::cout << "Sum of distance squares divided by numEntries: " << sumSquares/numEntries << std::endl;
				// result.Print(std::cout); // (un)suppress results output

				// get fit parameters
				const double * parFit = result.GetParams();

				// draw the graph
				graph->SetMarkerStyle(8);
				graph->SetMarkerSize(0.5);
				graph->Draw("pcol");

				// draw the fitted line
				int n = 1000;
				double t0 = 0; // t is the z coordinate
				double dt = 40.96;
				TPolyLine3D *l = new TPolyLine3D(n);
				for (int i = 0; i <n;++i) {
				  double t = t0+ dt*i/n;
				  double x,y,z;
				  line(t,parFit,x,y,z);
				  l->SetPoint(i,x,y,z);
				}
				l->SetLineColor(kRed);
				l->Draw("same");

				// Access fit params and minfcnvalue
				// cout << "FIT1: " << parFit[1] << "\n";
				// cout << "FIT2: " << result.MinFcnValue() << "\n";
			}

			// Criteria to be a good event (if not, then don't show)
			bool isGoodEvent = false;
			if (numEntries >= 50 && sumSquares/numEntries < 2.0) {
				isGoodEvent = true;
			}

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