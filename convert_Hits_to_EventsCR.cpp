#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"


void convert_Hits_to_EventsCR() {
	/*** Creates a CERN ROOT file that contains a TTree that holds all the calculated data needed for analysis. ALSO, updates the Hits file, adding in a branch for the s values. Used when source is: cosmic rays (CR)

	Organized in the following branches: (name of branch, root type descriptor)
	1. h5_file_num, i, UInt_t
	2. SM_event_num, L, Long64_t (NOT same as h5 event num)
	3. num_hits, i, UInt_t
	4. sum_tots, i, UInt_t
	5. mean_x, D, Double_t
	6. mean_y, D, Double_t
	7. mean_z, D, Double_t
	8. line_fit_param0, D, Double_t (x-intercept when z = 0)
	9. line_fit_param1, D, Double_t (dx/dz)
	10. line_fit_param2, D, Double_t (y-intercept when z = 0)
	11. line_fit_param3, D, Double_t (dy/dz)
	12. sum_of_squares, D, Double_t (same as chi square with error = 1 for all pts)
	
	13. event_status, i, UInt_t (0 if good event, 1 if fit failed, 2 if didn't meet "good" event criteria)
	14. fraction_inside_sphere, D, Double_t (fraction of the hits that are within a sphere of radius 1 centered at the mean; if this is close to 1, it means the hits are like a dense blob instead of a long streak)
	15. length_track, D, Double_t (length of track, in mm)
	16. sum_tots_div_by_length_track, D, Double_t
	17. sum_squares_div_by_DoF, D, Double_t
	

	Note: to add a new branch:
	- add to this info comment
	- make a variable for the branch
	- make the branch
	- *if needed, add to list of important output values at the start of while loop
	- put in calculations for value
	- assign the branch variable at the end of the while loop

	Author: Dennis Feng	
	***/
	gROOT->Reset(); 

	// Setting up TTreeReader for input file, also add the "s" branch to the ttree
	UInt_t h5_file_num_input = 133;    // CHOOSE THIS

	TFile *in_file = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/" + to_string(h5_file_num_input) + "_module_202_new_stop_mode_ext_trigger_scan_interpreted_Hits.root").c_str(), "UPDATE");

	TTreeReader *reader = new TTreeReader("Table", in_file);

	TTreeReaderValue<UInt_t> h5_file_num(*reader, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(*reader, "event_number");
	TTreeReaderValue<UChar_t> tot(*reader, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(*reader, "relative_BCID");
	TTreeReaderValue<Long64_t> SM_event_num(*reader, "SM_event_num");
	TTreeReaderValue<Double_t> x(*reader, "x");
	TTreeReaderValue<Double_t> y(*reader, "y");
	TTreeReaderValue<Double_t> z(*reader, "z");

	// Add branch "s" if not already added before:
	bool noSBranchBefore = false;
	Double_t s = 0;
	TTree *T_Hits = (TTree*) in_file->Get("Table"); // tree from Hits file
	TBranch *branch_S;
	if (T_Hits->GetBranch("s") == 0) {
		noSBranchBefore = true;
		branch_S = T_Hits->Branch("s", &s, "s/D");
	}

	// Create EventsCR file and TTree
	TFile *out_file = new TFile(("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/" + to_string(h5_file_num_input) + "_module_202_new_stop_mode_ext_trigger_scan_interpreted_EventsCR.root").c_str(), "RECREATE");
	TTree *t = new TTree("Table","EventsCR Data");
	
	UInt_t h5_file_num_EventsCR = h5_file_num_input; // must be different name than the one from raw file
	Long64_t SM_event_num_EventsCR = 0; // must be different name than the one from raw file
	UInt_t num_hits = 0;
	UInt_t sum_tots = 0;
	Double_t mean_x = 0;
	Double_t mean_y = 0;
	Double_t mean_z = 0;
	Double_t line_fit_param0 = 0;
	Double_t line_fit_param1 = 0;
	Double_t line_fit_param2 = 0;
	Double_t line_fit_param3 = 0;
	Double_t sum_of_squares = 0;

	UInt_t event_status = 0;
	Double_t fraction_inside_sphere = 0;
	Double_t length_track = 0;
	Double_t sum_tots_div_by_length_track = 0;
	Double_t sum_squares_div_by_DoF = 0;

	t->Branch("h5_file_num", &h5_file_num_EventsCR, "h5_file_num/i");
	t->Branch("SM_event_num", &SM_event_num_EventsCR, "SM_event_num/L");
	t->Branch("num_hits", &num_hits, "num_hits/i");
	t->Branch("sum_tots", &sum_tots, "sum_tots/i");
	t->Branch("mean_x", &mean_x, "mean_x/D");
	t->Branch("mean_y", &mean_y, "mean_y/D");
	t->Branch("mean_z", &mean_z, "mean_z/D");
	t->Branch("line_fit_param0", &line_fit_param0, "line_fit_param0/D");
	t->Branch("line_fit_param1", &line_fit_param1, "line_fit_param1/D");
	t->Branch("line_fit_param2", &line_fit_param2, "line_fit_param2/D");
	t->Branch("line_fit_param3", &line_fit_param3, "line_fit_param3/D");
	t->Branch("sum_of_squares", &sum_of_squares, "sum_of_squares/D");

	t->Branch("event_status", &event_status, "event_status/i");
	t->Branch("fraction_inside_sphere", &fraction_inside_sphere, "fraction_inside_sphere/D");
	t->Branch("length_track", &length_track, "length_track/D");
	t->Branch("sum_tots_div_by_length_track", &sum_tots_div_by_length_track, "sum_tots_div_by_length_track/D");
	t->Branch("sum_squares_div_by_DoF", &sum_squares_div_by_DoF, "sum_squares_div_by_DoF/D");




	// Calculations:
	// Initialize the graph
	TGraph2D *graph = new TGraph2D();

	// Variables used to loop the main loop
	bool endOfReader = false; // if reached end of the reader
	Long64_t smEventNum = 1; // the current SM-event, start at 1
	t->Fill(); // fills SM Event 0 with all zeroes. Assumes there's nothing in this event, which is probably true. Also assumes smEventNum starts at 1.
	
	// Main Loop (loops for every smEventNum)
	while (!endOfReader) {
		// Variables used in this main loop
		int startEntryNum = 0;
		int endEntryNum = 0;
		bool lastEvent = false;

		// Declaring some important output values for the current graph and/or line fit
		UInt_t numEntries = 0;
		UInt_t sumTots = 0;
		double meanX = 0;
		double meanY = 0;
		double meanZ = 0;
		double param0 = 0;
		double param1 = 0;
		double param2 = 0;
		double param3 = 0;
		double sumSquares = 0;

		UInt_t eventStatus = 0;
		double fractionInsideSphere = 0;
		double lengthTrack = 0;

		// Get startEntryNum and endEntryNum
		startEntryNum = getEntryNumWithSMEventNum(reader, smEventNum);
		endEntryNum = getEntryNumWithSMEventNum(reader, smEventNum + 1);

		if (startEntryNum == -2) { // can't find the exact smEventNum
			cout << "Error: There should not be any SM event numbers that are missing." << "\n";
		} else if (startEntryNum == -3) { 
			endOfReader = true;
			break;
		} else if (endEntryNum == -3) { // assuming no SM event nums are skipped
			endEntryNum = reader->GetEntries(false);
			lastEvent = true;
		}

		// Fill TGraph with points, get mean XYZ and sumTots
		graph = new TGraph2D(); // create a new TGraph to refresh

		reader->SetEntry(startEntryNum);
		for (int i = 0; i < endEntryNum - startEntryNum; i++) {
			graph->SetPoint(i, (*x - 0.001), (*y + 0.001), (*z - 0.001));
			
			// get mean XYZ and sumTots
			meanX += *x;
			meanY += *y;
			meanZ += *z;
			sumTots += *tot;

			endOfReader = !(reader->Next());
		}
		numEntries = endEntryNum - startEntryNum;
		meanX /= numEntries;
		meanY /= numEntries;
		meanZ /= numEntries;

		// 3D Fit, only accept "good" events, get input
		if (!endOfReader || lastEvent) {
			// Starting the fit. First, get decent starting parameters for the fit - do two 2D fits (one for x vs z, one for y vs z). 
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
			Double_t initParam0 = fitZX->GetParams()[0];
			Double_t initParam1 = fitZX->GetParams()[1];
			Double_t initParam2 = fitZY->GetParams()[0];
			Double_t initParam3 = fitZY->GetParams()[1];

			// 3D FITTING CODE (based on line3Dfit.C), draw graph and line fit
			ROOT::Fit::Fitter  fitter;
		   	SumDistance2 sdist(graph);
#ifdef __CINT__
		   	ROOT::Math::Functor fcn(&sdist,4,"SumDistance2");
#else
		   	ROOT::Math::Functor fcn(sdist,4);
#endif
			// set the function and the initial parameter values
			double pStart[4] = {initParam0, initParam1, initParam2, initParam3};
			fitter.SetFCN(fcn,pStart);
			// set step sizes different than default ones (0.3 times parameter values)
			for (int i = 0; i < 4; ++i) fitter.Config().ParSettings(i).SetStepSize(0.01);

			bool ok = fitter.FitFCN();
			if (!ok) {
				// Error("line3Dfit","Line3D Fit failed");
				cout << "Error: SM Event " << smEventNum << ": 3D line fit failed.\n";
				eventStatus = 1; // signifies that fit failed
			} else if (numEntries == 1) {
				cout << "Error solved: SM Event " << smEventNum << ": Has only 1 hit, so the line fit failed, and it was taken care of accordingly." << "\n";
				eventStatus = 1; // signifies that fit failed
			} else if (numEntries <= 0) {
				cout << "Error: SM Event " << smEventNum << ": Causes numEntries <= 0, which should never be the case." << "\n";
			} else {
				const ROOT::Fit::FitResult & result = fitter.Result();
				const double * fitParams = result.GetParams();

				// Access fit params and minfcnvalue
				param0 = fitParams[0];
				param1 = fitParams[1];
				param2 = fitParams[2];
				param3 = fitParams[3];
				sumSquares = result.MinFcnValue();

				// std::cout << "Theta : " << TMath::ATan(sqrt(pow(fitParams[1], 2) + pow(fitParams[3], 2))) << std::endl;
			}

			// Add s value to Hits file for each hit
			// s = AP . AB_hat; the "." is a dot product, and _hat means it's a unit vector. Point A is the mean XYZ position. Point P is the current hit position. AB_hat is a unit vector pointing along the track's best fit line, in the direction of increasing z.
			TVector3 AB(param1, param3, 1);
			TVector3 AB_hat = AB * (1 / sqrt(AB.Dot(AB)));
			double max_s = 0; // maximum s in this event
			double min_s = 0;
			reader->SetEntry(startEntryNum);
			for (int i = 0; i < endEntryNum - startEntryNum; i++) {
				TVector3 AP(*x - meanX, *y - meanY, *z - meanZ);
				s = AP.Dot(AB_hat);
				
				if (noSBranchBefore) {
					branch_S->Fill();
				}

				if (s > max_s) {
					max_s = s;
				} else if (s < min_s) {
					min_s = s;
				}

				reader->Next();
			}
			lengthTrack = max_s - min_s;

			// calculating fraction of hits inside the sphere of radius 1 (mm)
			double radius = 1; // length in mm 
			reader->SetEntry(startEntryNum);
			for (int i = 0; i < endEntryNum - startEntryNum; i++) {
				double distanceFromMeanXYZ = sqrt(pow(graph->GetX()[i] - meanX, 2) + pow(graph->GetY()[i] - meanY, 2) + pow(graph->GetZ()[i] - meanZ, 2));
				if (distanceFromMeanXYZ <= 2) {
					fractionInsideSphere += 1;
				}
				reader->Next();
			}
			fractionInsideSphere /= endEntryNum - startEntryNum;

			// Fill the TTree, fill in event_status after all the others
			if (eventStatus == 1) { // if fit failed, these variables are 0
				param0 = 0;
				param1 = 0;
				param2 = 0;
				param3 = 0;
				sumSquares = 0;
			}

			h5_file_num_EventsCR = h5_file_num_input;
			SM_event_num_EventsCR = smEventNum;
			num_hits = numEntries;
			sum_tots = sumTots;
			mean_x = meanX;
			mean_y = meanY;
			mean_z = meanZ;
			line_fit_param0 = param0;
			line_fit_param1 = param1;
			line_fit_param2 = param2;
			line_fit_param3 = param3;
			sum_of_squares = sumSquares;

			// event_status = eventStatus;
			fraction_inside_sphere = fractionInsideSphere;
			length_track = lengthTrack;
			sum_tots_div_by_length_track = sum_tots / length_track;
			sum_squares_div_by_DoF = sum_of_squares / (num_hits - 2);

			if (eventStatus != 1) { // if fit didn't fail
				if (num_hits < 50 
					|| sum_of_squares / num_hits >= 2.0 
					|| fraction_inside_sphere >= 0.8
					|| length_track < 3.0) { // if bad event

					eventStatus = 2;
				}
			}
			event_status = eventStatus;

			t->Fill();
			cout << "SM Event " << smEventNum << ": event_status: " << event_status << "\n";
		}
		smEventNum++;
	}


	in_file->Write();
    in_file->Close();
	out_file->Write();
    out_file->Close();
}
