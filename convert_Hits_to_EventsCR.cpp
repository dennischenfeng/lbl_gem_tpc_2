#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"


void convert_Hits_to_EventsCR() {
	/*** Creates a CERN ROOT file that contains a TTree that holds all the calculated data needed for analysis. ALSO, updates the Hits file, adding in a branch for the s values. Used when source is: cosmic rays (CR)

	Organized in the following branches: (name of branch, root type descriptor, root type)
	Each entry is data for 1 event. 
	1. h5_file_num, i, UInt_t
	2. SM_event_num, L, Long64_t (NOT same as h5 event num)
	3. num_hits, i, UInt_t
	4. sum_tots, i, UInt_t
	5. mean_x, D, Double_t
	6. mean_y, D, Double_t
	7. mean_z, D, Double_t
	8. line_fit_param0, D, Double_t (x-intercept when z = 0, not necessarily in box)
	9. line_fit_param1, D, Double_t (dx/dz)
	10. line_fit_param2, D, Double_t (y-intercept when z = 0, not necessarily in box)
	11. line_fit_param3, D, Double_t (dy/dz)
	12. sum_of_squares, D, Double_t (to best fit line, same as chi square with error = 1 for all pts)
	
	13. event_status, i, UInt_t (0 if good event, 1 if fit failed, 2 if didn't meet "good" event criteria, 3 if noise burst or its an event that occurred after a noise burst (after noise burst means potentially unreliable time bins))
	14. fraction_inside_sphere, D, Double_t (fraction of the hits that are within a sphere of radius 1 centered at the mean; if this is close to 1, it means the hits are like a dense blob instead of a long streak)
	15. length_track, D, Double_t (length of track, in mm, using s coordinate)
	16. sum_tots_div_by_length_track, D, Double_t
	17. sum_squares_div_by_DoF, D, Double_t (DoF= degrees of freedom ~num hits)
	18. zenith_angle, D, Double_t (zenith angle of track, in radians)
	19. duration, i, UInt_t (time duration of event, in units of BCIDs (1BCID= 1/f))
	

	Note: to add a new branch:
	- add to this info comment
	- make a branch pointer variable for the branch
	- make the branch
	- *if needed, add to list of important output values (dummy variables) at the start of while loop
	- put in calculations for value
	- assign the dummy varialbe to the branch variable at the end of the while loop
	
	Note: you can also change the criteria for a bad event (event_status = 2) 
	- search this script for "criteria for good event"

	Author: Dennis Feng	
	***/
	gROOT->Reset(); 

	
	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	const int numFiles = 51; // CHOOSE THESE
	const int fileNums[numFiles] = {1220,1221,1222,1223,1224,1225,1226,1227,1228,1229,1230,1231,1232,1233,1234,1235,1236,1237,1238,1239,1240,1241,1242,1243,1244,1245,1246,1247,1248,1249,1250,1251,1252,1253,1254,1255,1256,1257,1258,1259,1260,1261,1262,1263,1264,1265,1266,1267,1268,1269,1270};
	// const int fileNums[numFiles] = {133};


	for (int fileIndex = 0; fileIndex < numFiles; fileIndex++) {
		cout << "Converting Hits file with h5 file num: " << fileNums[fileIndex] << "\n";

		// Setting up TTreeReader for input file, also add the "s" branch to the ttree 
		TFile *in_file = new TFile(("/home/pixel/pybar/pybar_github/pybar/module_1/" + to_string(fileNums[fileIndex]) + "_module_1_stop_mode_ext_trigger_scan_interpreted_Hits.root").c_str(), "UPDATE");

		TTreeReader *reader = new TTreeReader("Table", in_file);

		TTreeReaderValue<UInt_t> h5_file_num(*reader, "h5_file_num");
		TTreeReaderValue<Long64_t> event_number(*reader, "event_number");
		TTreeReaderValue<UChar_t> tot(*reader, "tot");
		TTreeReaderValue<UChar_t> relative_BCID(*reader, "relative_BCID");
		TTreeReaderValue<Long64_t> SM_event_num(*reader, "SM_event_num");
		TTreeReaderValue<UInt_t> SM_rel_BCID(*reader, "SM_rel_BCID");
		TTreeReaderValue<Double_t> x(*reader, "x");
		TTreeReaderValue<Double_t> y(*reader, "y");
		TTreeReaderValue<Double_t> z(*reader, "z");

		// Add branch "s" if not already added before:
		bool noSorDBranchBefore = false; // does not have s or d branch yet
		Double_t s = 0;
		Double_t d = 0;
		TTree *T_Hits = (TTree*) in_file->Get("Table"); // tree from Hits file
		TBranch *branch_S;
		TBranch *branch_D;
		if (T_Hits->GetBranch("s") == 0 || T_Hits->GetBranch("d") == 0) { 
			noSorDBranchBefore = true;
			branch_S = T_Hits->Branch("s", &s, "s/D");
			branch_D = T_Hits->Branch("d", &d, "d/D");
		}

		// Create EventsCR file and TTree
		TFile *out_file = new TFile(("/home/pixel/pybar/pybar_github/pybar/module_1/" + to_string(fileNums[fileIndex]) + "_module_1_stop_mode_ext_trigger_scan_interpreted_EventsCR.root").c_str(), "RECREATE");
		TTree *t = new TTree("Table","EventsCR Data");
		
		// Branch pointer variables
		UInt_t h5_file_num_EventsCR = 0; // must be different name than the one from raw file
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
		Double_t zenith_angle = 0;
		UInt_t duration = 0;

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
		t->Branch("zenith_angle", &zenith_angle, "zenith_angle/D");
		t->Branch("duration", &duration, "duration/i");




		// Calculations:
		// Initialize the graph
		TGraph2D *graph = new TGraph2D();

		// Variables used to loop the main loop
		bool endOfReader = false; // if reached end of the reader
		Long64_t smEventNum = 1; // the current SM-event, start at 1
		bool noiseBurstOccurred  = false; // if an event is a noise burst (>20000 events), then mark that event as well as every event occurring after it as a bad event
		t->Fill(); // fills SM Event 0 with all zeroes. Assumes there's nothing in this event, which is probably true. Also assumes smEventNum starts at 1.
		
		// Main Loop (loops for every smEventNum= each event)
		while (!endOfReader) {
			// cout << "STARTING SM EVENT: " << smEventNum << "\n"; // @@@

			// Variables used in this main loop
			int startEntryNum = 0;
			int endEntryNum = 0;
			bool lastEvent = false;
			int startRelBCID = 0;
			int endRelBCID_include = 0; // endRelBCID_include is included in calculating the duration

			// Declaring some important output values for the current graph and/or line fit
			// Dummy variables (which Dennis added in for some strange reason)
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

			// cout << "CHECKPT 1... start: " << startEntryNum << "     end: " << endEntryNum << "\n"; // @@@

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


				//------------------------------------------------

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







				//-----------------------------------------------------

				// Add s and d value to Hits file for each hit
				// s = AP DOT AB_hat; the "." is a dot product, and _hat means it's a unit vector. Point A is the mean XYZ position. Point P is the current hit position. AB_hat is a unit vector pointing along the track's best fit line, in the direction of increasing z.
				// d = |AP x AB_hat|
				TVector3 AB(param1, param3, 1);
				TVector3 AB_hat = AB * (1 / sqrt(AB.Dot(AB)));
				double max_s = 0; // maximum s in this event
				double min_s = 0;
				reader->SetEntry(startEntryNum);
				for (int i = 0; i < endEntryNum - startEntryNum; i++) {
					TVector3 AP(*x - meanX, *y - meanY, *z - meanZ);
					s = AP.Dot(AB_hat);
					d = (AP.Cross(AB_hat)).Mag(); // @@@ does this work properly? Yes it does
					
					if (noSorDBranchBefore) {
						branch_S->Fill();
						branch_D->Fill();
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

				// getting the startRelBCID and endRelBCID_include
				reader->SetEntry(startEntryNum);
				for (int i = 0; i < endEntryNum - startEntryNum; i++) {
					if (i == 0) {
						startRelBCID = *SM_rel_BCID;
						endRelBCID_include = *SM_rel_BCID;
					} else {
						if (*SM_rel_BCID > endRelBCID_include) {
							endRelBCID_include = *SM_rel_BCID;
						}
					}
					reader->Next();
				}


				// Fill the TTree, fill in event_status after all the others
				if (eventStatus == 1) { // if fit failed, these variables are 0
					param0 = 0;
					param1 = 0;
					param2 = 0;
					param3 = 0;
					sumSquares = 0;
				}

				// fill in using num_hits. Data comes from numEntries (calculations above) 

				h5_file_num_EventsCR = fileNums[fileIndex];
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

				// Additional information.
				// branch pointer <-- dummy variable

				// event_status = eventStatus;
				fraction_inside_sphere = fractionInsideSphere;
				length_track = lengthTrack;
				sum_tots_div_by_length_track = sumTots / lengthTrack;
				sum_squares_div_by_DoF = sumSquares / (numEntries - 2);
				zenith_angle = TMath::ATan(sqrt(pow(param1, 2) + pow(param3, 2)));
				duration = endRelBCID_include - startRelBCID + 1;


				//--------------------------------------------------------
				//THIS IS WHERE YOU PUT THE CRITERIA FOR GOOD AND BAD EVENTS !!!!!!!!!!!!!!!!!!!!

				if (num_hits >= 20000) { // 20000 hits on one event is a noise burst
					noiseBurstOccurred = true;
					eventStatus = 3;
				} else if (noiseBurstOccurred) {
					eventStatus = 3;
				} else if (eventStatus != 1) { // if line fit didn't fail, now apply criteria for good events
					// Criteria for bad events
					if (num_hits < 20)
					{ // mark this as a bad event
						eventStatus = 2;
					}
				}

				event_status = eventStatus;

				//--------------------------------------------------------


				t->Fill();
				// cout << "SM Event " << smEventNum << ": event_status: " << event_status << "\n";
			}
			smEventNum++;
		}


		in_file->Write();
	    in_file->Close();
		out_file->Write();
	    out_file->Close();
	}
}
