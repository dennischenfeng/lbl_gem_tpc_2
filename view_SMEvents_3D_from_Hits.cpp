
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"

void view_SMEvents_3D_from_Hits() {
	/*** Displays an 3D occupancy plot for each SM Event. (stop mode event)

	Can choose which SM event to start at. (find "CHOOSE THIS" in this script)
	Input file must be a Hits file (_interpreted_Hits.root file).
	***/
	gROOT->Reset();

	// Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/101_module_202_new_stop_mode_ext_trigger_scan_interpreted_Hits.root");


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

	// Initialize the canvas and graph
	TCanvas *c1 = new TCanvas("c1","3D Occupancy for Specified SM Event", 1000, 10, 900, 550);
	c1->SetRightMargin(0.25);
	TGraph2D *graph = new TGraph2D();

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

		// Declaring some important output values for the current graph and/or line fit
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

		// Fill TGraph with points and set title and axes
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

		// 3D Fit, display results, draw graph and line fit, only accept "good" events, get input
		if (!endOfReader || lastEvent) {
			// Display some results
			numEntries = graph->GetN();
			cout << "Current SM Event Number: " << smEventNum << "\n";
			cout << "Number of entries:       " << numEntries << "\n";

			// Starting the fit. First, get decent starting parameters for the fit - do two 2D fits (one for x vs z, one for y vs z)
			TGraph *graphZX = new TGraph();
			TGraph *graphZY = new TGraph();
			reader->SetEntry(startEntryNum);
			for (int i = 0; i < endEntryNum - startEntryNum; i++) {
				graphZX->SetPoint(i, (*z - 0.001), (*x + 0.001));
				graphZY->SetPoint(i, (*z - 0.001), (*y + 0.001));
				reader->Next();
			}
			TFitResultPtr fitZX = graphZX->Fit("pol1", "WQS"); // w for ignore error of each pt, q for quiet (suppress results output), s for return a tfitresultptr
			TFitResultPtr fitZY = graphZY->Fit("pol1", "WQS");
			Double_t param0 = fitZX->GetParams()[0];
			Double_t param1 = fitZX->GetParams()[1];
			Double_t param2 = fitZY->GetParams()[0];
			Double_t param3 = fitZY->GetParams()[1];

			// // Draw the lines for the two 2D fits
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


			// 3D FITTING CODE (based on line3Dfit.C), draw graph and line fit
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
				const double * fitParams = result.GetParams();

				sumSquares = result.MinFcnValue();
				std::cout << "Sum of distance squares:  " << sumSquares << std::endl;
				std::cout << "Sum of distance squares divided by numEntries: " << sumSquares/numEntries << std::endl;
				std::cout << "Theta : " << TMath::ATan(sqrt(pow(fitParams[1], 2) + pow(fitParams[3], 2))) << std::endl;
				// result.Print(std::cout); // (un)suppress results output

				// Draw the graph
				graph->SetMarkerStyle(8);
				graph->SetMarkerSize(0.5);
				graph->Draw("pcol");

				// Draw the fitted line
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

				// Access fit params and minfcnvalue
				// cout << "FIT1: " << fitParams[1] << "\n";
				// cout << "FIT2: " << result.MinFcnValue() << "\n";
			}

			// Criteria to be a good event (if not good entry, then don't show)
			bool isGoodEvent = false;

				// the following block of code finds the mean X, Y ans Z values
				double meanX = 0;
				double meanY = 0;
				double meanZ = 0;
				reader->SetEntry(startEntryNum);
				for (int i = 0; i < endEntryNum - startEntryNum; i++) {
					meanX += graph->GetX()[i];
					meanY += graph->GetY()[i];
					meanZ += graph->GetZ()[i];
					reader->Next();
				}
				meanX /= endEntryNum - startEntryNum;
				meanY /= endEntryNum - startEntryNum;
				meanZ /= endEntryNum - startEntryNum;

				// the following code block calculates the fraction of the hits in the smEvent that are inside a sphere, centered at the mean XYZ, of radius 'radius' (larger fraction means the track is less like a long streak and more like a dense blob)
				double radius = 1; // length in mm 
				double fractionInsideSphere = 0;
				reader->SetEntry(startEntryNum);
				for (int i = 0; i < endEntryNum - startEntryNum; i++) {
					double distanceFromMeanXYZ = sqrt(pow(graph->GetX()[i] - meanX, 2) + pow(graph->GetY()[i] - meanY, 2) + pow(graph->GetZ()[i] - meanZ, 2));
					if (distanceFromMeanXYZ <= 2) {
						fractionInsideSphere += 1;
					}
					reader->Next();
				}
				fractionInsideSphere /= endEntryNum - startEntryNum;

				cout << "fraction inside sphere: " << fractionInsideSphere << "\n";

			// if (numEntries >= 50 
			// 	&& sumSquares/numEntries < 2.0 
			// 	&& fractionInsideSphere < 0.8) {

			// 	isGoodEvent = true;
			// }

			isGoodEvent = true;

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