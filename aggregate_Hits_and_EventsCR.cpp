#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"


void aggregate_Hits_and_EventsCR() {
	/*** Creates two ROOT files: 1) a AggrHits file, and 2) a AggrEventsCR file. An AggrHits File contains all the hits from GOOD EVENTS (event_status == 0) from the Hits files whose numbers are indicated by the fileNums array. Similarly, AggrEventsCR contains all the good events from the EventsCR files. Each Aggr file contains a TTree, whose branches are the same as their non-aggregate counterparts (you can check these branches from the info comments of convert_h5_to_Hits.py and convert_Hits_to_EventsCR.cpp). 

	Purpose of Aggr files: aggregate GOOD events only (does not aggregate bad events)

	Used when source is: cosmic rays (CR).
	
	Note: Aggr means Aggregate.
	
	Note: To add a branch:
	- add branch variable for AggrHits/AggrEventsCR
	- add branch for AggrHits/AggrEventsCR
	- add TTreeReaderValue for Hits/EventsCR
	- assign variable in the main loop for AggrHits/AggrEventsCR



	Author: Dennis Feng	
	***/
	gROOT->Reset(); 

	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// CHOOSE THESE 
	int aggrFileNum = 32; //name number of the aggregate
	const int numFiles = 51; 
	const int fileNums[numFiles] = {1220,1221,1222,1223,1224,1225,1226,1227,1228,1229,1230,1231,1232,1233,1234,1235,1236,1237,1238,1239,1240,1241,1242,1243,1244,1245,1246,1247,1248,1249,1250,1251,1252,1253,1254,1255,1256,1257,1258,1259,1260,1261,1262,1263,1264,1265,1266,1267,1268,1269,1270};

	// Legacy 
	//{332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,  361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380, 381,382,383,384,385,386,387,  389,390,391,392,393,394,395,396,397,398,399,400,401,402,403,404,405,406,407,408,  410,411,412,  414,415,416,417,418,419,420,421,422,423,424,425,426,427,428,429,430,431,432,433,434,435,436,437,438,439,440,441,442,443,444,445,446, 448,449,450,451,  453,454,  456,457,458,459,460,461,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,501,502,  504,505,506,507,508,509,510,511,512,513,514,515,516,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,540,541,542,543,544,545,546,547,548,549,550,551,552,553,554,555,556,557,558,559,560,561,562,563,564,565,566,567,568,569,570,571,  573,574,575,576,  578,579,580,581,582,583,584,585,586,587,588,589,590,591,592,593,594,595,596,597,598,599,600,601,602,603,604,          615,     618,619,620,621,622,623,624,625,626,627,628,629,630,631,632,633,634,635,636,637,638,639,640,641,642,643,644,645,646,647,648,649,650,  652,653,654,655,  657,658,659,660,  662,663,664,665,666,667,668,669,670,671,672,673,674,675,676,677,678,679,680,681,682,683,684,685,686,687,688}; // h5 file nums of the Hits files (and corresponding EventsCR files) that will be aggregated into the Aggregate files


	// Make files, trees, and branches for AggrHits and AggrEventsCR
	TFile *f_AggrHits = new TFile(("/home/pixel/pybar/pybar_github/pybar/homemade_scripts/aggregate_data/" + to_string(aggrFileNum) + "_module_1_AggrHits.root").c_str(), "RECREATE");
	TTree *T_AggrHits = new TTree("Table", "Aggregate Hits Data");
	UInt_t h5_file_num_AggrHits = 0;
	Long64_t event_number_AggrHits = 0;
	UChar_t tot_AggrHits = 0;
	UChar_t relative_BCID_AggrHits = 0;
	Long64_t SM_event_num_AggrHits = 0;
	UInt_t SM_rel_BCID_AggrHits = 0;
	Double_t x_AggrHits = 0;
	Double_t y_AggrHits = 0;
	Double_t z_AggrHits = 0;
	Double_t s_AggrHits = 0;
	Double_t d_AggrHits = 0;
	T_AggrHits->Branch("h5_file_num", &h5_file_num_AggrHits, "h5_file_num/i");
	T_AggrHits->Branch("event_number", &event_number_AggrHits, "event_number/L");
	T_AggrHits->Branch("tot", &tot_AggrHits, "tot/b");
	T_AggrHits->Branch("relative_BCID", &relative_BCID_AggrHits, "relative_BCID/b");
	T_AggrHits->Branch("SM_event_num", &SM_event_num_AggrHits, "SM_event_num/L");
	T_AggrHits->Branch("SM_rel_BCID", &SM_rel_BCID_AggrHits, "SM_rel_BCID/i");
	T_AggrHits->Branch("x", &x_AggrHits, "x/D");
	T_AggrHits->Branch("y", &y_AggrHits, "y/D");
	T_AggrHits->Branch("z", &z_AggrHits, "z/D");
	T_AggrHits->Branch("s", &s_AggrHits, "s/D");
	T_AggrHits->Branch("d", &d_AggrHits, "d/D");

	TFile *f_AggrEventsCR = new TFile(("/home/pixel/pybar/pybar_github/pybar/homemade_scripts/aggregate_data/" + to_string(aggrFileNum) + "_module_1_AggrEventsCR.root").c_str(), "RECREATE");
	TTree *T_AggrEventsCR = new TTree("Table", "Aggregate EventsCR Data");
	UInt_t h5_file_num_AggrEventsCR = 0;
	Long64_t SM_event_num_AggrEventsCR = 0;
	UInt_t num_hits_AggrEventsCR = 0;
	UInt_t sum_tots_AggrEventsCR = 0;
	Double_t mean_x_AggrEventsCR = 0;
	Double_t mean_y_AggrEventsCR = 0;
	Double_t mean_z_AggrEventsCR = 0;
	Double_t line_fit_param0_AggrEventsCR = 0;
	Double_t line_fit_param1_AggrEventsCR = 0;
	Double_t line_fit_param2_AggrEventsCR = 0;
	Double_t line_fit_param3_AggrEventsCR = 0;
	Double_t sum_of_squares_AggrEventsCR = 0;
	UInt_t event_status_AggrEventsCR = 0;
	Double_t fraction_inside_sphere_AggrEventsCR = 0;
	Double_t length_track_AggrEventsCR = 0;
	Double_t sum_tots_div_by_length_track_AggrEventsCR = 0;
	Double_t sum_squares_div_byDoF_AggrEventsCR = 0;
	Double_t zenith_angle_AggrEventsCR = 0;
	UInt_t duration_AggrEventsCR = 0;
	T_AggrEventsCR->Branch("h5_file_num", &h5_file_num_AggrEventsCR, "h5_file_num/i");
	T_AggrEventsCR->Branch("SM_event_num", &SM_event_num_AggrEventsCR, "SM_event_num/L");
	T_AggrEventsCR->Branch("num_hits", &num_hits_AggrEventsCR, "num_hits/i");
	T_AggrEventsCR->Branch("sum_tots", &sum_tots_AggrEventsCR, "sum_tots/i");
	T_AggrEventsCR->Branch("mean_x", &mean_x_AggrEventsCR, "mean_x/D");
	T_AggrEventsCR->Branch("mean_y", &mean_y_AggrEventsCR, "mean_y/D");
	T_AggrEventsCR->Branch("mean_z", &mean_z_AggrEventsCR, "mean_z/D");
	T_AggrEventsCR->Branch("line_fit_param0", &line_fit_param0_AggrEventsCR, "line_fit_param0/D");
	T_AggrEventsCR->Branch("line_fit_param1", &line_fit_param1_AggrEventsCR, "line_fit_param1/D");
	T_AggrEventsCR->Branch("line_fit_param2", &line_fit_param2_AggrEventsCR, "line_fit_param2/D");
	T_AggrEventsCR->Branch("line_fit_param3", &line_fit_param3_AggrEventsCR, "line_fit_param3/D");
	T_AggrEventsCR->Branch("sum_of_squares", &sum_of_squares_AggrEventsCR, "sum_of_squares/D");
	T_AggrEventsCR->Branch("event_status", &event_status_AggrEventsCR, "event_status/i");
	T_AggrEventsCR->Branch("fraction_inside_sphere", &fraction_inside_sphere_AggrEventsCR, "fraction_inside_sphere/D");
	T_AggrEventsCR->Branch("length_track", &length_track_AggrEventsCR, "length_track/D");
	T_AggrEventsCR->Branch("sum_tots_div_by_length_track", &sum_tots_div_by_length_track_AggrEventsCR, "sum_tots_div_by_length_track/D");
	T_AggrEventsCR->Branch("sum_squares_div_by_DoF", &sum_squares_div_byDoF_AggrEventsCR, "sum_squares_div_by_DoF/D");
	T_AggrEventsCR->Branch("zenith_angle", &zenith_angle_AggrEventsCR, "zenith_angle/D");
	T_AggrEventsCR->Branch("duration", &duration_AggrEventsCR, "duration/i");

	// Assign the values to the vector fileNames
	vector< vector<string> > fileNames;
	fileNames.resize(numFiles, vector<string>(2, ""));
	for (int i = 0; i < numFiles; i++) {
		fileNames[i][0] = "/home/pixel/pybar/pybar_github/pybar/module_1/" + to_string(fileNums[i]) + "_module_1_stop_mode_ext_trigger_scan_interpreted_Hits.root";
		fileNames[i][1] = "/home/pixel/pybar/pybar_github/pybar/module_1/" + to_string(fileNums[i]) + "_module_1_stop_mode_ext_trigger_scan_interpreted_EventsCR.root";
	}

	// Main loop
	for (int fileIndex = 0; fileIndex < fileNames.size(); fileIndex++) { // for every non-aggregate file pair
		// Get the readers for the Hits and EventsCR files
		TFile *f_Hits = new TFile(fileNames[fileIndex][0].c_str());
		TFile *f_EventsCR = new TFile(fileNames[fileIndex][1].c_str());

		TTreeReader *readerHits = new TTreeReader("Table", f_Hits);
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

		TTreeReader *readerEventsCR = new TTreeReader("Table", f_EventsCR);
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

		// Main loop
		while (readerEventsCR->Next()) { // for every SM Event in the EventsCR file
			if (readerEventsCR->GetCurrentEntry() == 0) {
				continue; // skip the entry num 0, because it probably contains no data
			}

			if (*event_status == 0) { // if event is a good event
				// Fill in T_AggrEventsCR first
				h5_file_num_AggrEventsCR = *h5_file_num_EventsCR;
				SM_event_num_AggrEventsCR = *SM_event_num_EventsCR;
				num_hits_AggrEventsCR = *num_hits;
				sum_tots_AggrEventsCR = *sum_tots;
				mean_x_AggrEventsCR = *mean_x;
				mean_y_AggrEventsCR = *mean_y;
				mean_z_AggrEventsCR = *mean_z;
				line_fit_param0_AggrEventsCR = *line_fit_param0;
				line_fit_param1_AggrEventsCR = *line_fit_param1;
				line_fit_param2_AggrEventsCR = *line_fit_param2;
				line_fit_param3_AggrEventsCR = *line_fit_param3;
				sum_of_squares_AggrEventsCR = *sum_of_squares;
				event_status_AggrEventsCR = *event_status;
				fraction_inside_sphere_AggrEventsCR = *fraction_inside_sphere;
				length_track_AggrEventsCR = *length_track;
				sum_tots_div_by_length_track_AggrEventsCR = *sum_tots_div_by_length_track;
				sum_squares_div_byDoF_AggrEventsCR = *sum_squares_div_by_DoF;
				zenith_angle_AggrEventsCR = *zenith_angle;
				duration_AggrEventsCR = *duration;
				T_AggrEventsCR->Fill();

				// Now, fill in T_AggrHits
				vector<int> entryNumRange_include(2);
				entryNumRange_include = getEntryNumRangeWithH5FileNumAndSMEventNum(readerHits, *h5_file_num_EventsCR, *SM_event_num_EventsCR);
				if (entryNumRange_include[0] == -1) {
					cout << "Error: h5_file_num and SM_event_num should be able to be found in the Hits file.\n";
				}
				readerHits->SetEntry(entryNumRange_include[0]);
				for (int i = 0; i < entryNumRange_include[1] - entryNumRange_include[0] + 1; i++) {
					h5_file_num_AggrHits = *h5_file_num_Hits;
					event_number_AggrHits = *event_number;
					tot_AggrHits = *tot;
					relative_BCID_AggrHits = *relative_BCID;
					SM_event_num_AggrHits = *SM_event_num_Hits;
					SM_rel_BCID_AggrHits = *SM_rel_BCID;
					x_AggrHits = *x;
					y_AggrHits = *y;
					z_AggrHits = *z;
					s_AggrHits = *s;
					d_AggrHits = *d;

					T_AggrHits->Fill();
					readerHits->Next();
				}
			}
		}

		f_Hits->Close();
		f_EventsCR->Close();
	}

	f_AggrHits->Write();
	f_AggrHits->Close();
	f_AggrEventsCR->Write();
	f_AggrEventsCR->Close();

}
