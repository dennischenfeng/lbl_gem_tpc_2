
void convert_raw_to_CR-calc() {
	/*** Creates a CERN ROOT file that contains a TTree that holds all the calculated data needed for analysis. Used when source is: cosmic rays (CR)

	Organized in the following branches: (name of branch, root type descriptor)
	- CR-raw_num, i
	- event_num, L (NOT same numbers as indicated by h5/raw files; goes by 1, 2, 3, etc)
	- num_hits, i
	- sum_tots, i
	- mean_x, D
	- mean_y, D
	- mean_z, D
	- R_xy, D
	- R_xyz
	
	The formatting was based off of Kelsey Oliver-Mallory's organization and scripts that were used with the older software, STControl.

	Author: Dennis Feng	
	***/
	gROOT->Reset(); 

	// Setting up TTreeReader for input file
	TFile *in_file = new TFile("/home/pixel/pybar/tags/2.0.1/host/pybar/module_test/111_module_test_stop_mode_ext_trigger_scan_interpreted_raw.root");

	TTreeReader tr("Table", in_file);

	TTreeReaderValue<UInt_t> h5_file_num(tr, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(tr, "event_number");
	TTreeReaderValue<UChar_t> tot(tr, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(tr, "relative_BCID");
	TTreeReaderValue<Double_t> x(tr, "x");
	TTreeReaderValue<Double_t> y(tr, "y");
	TTreeReaderValue<Double_t> z(tr, "z");


	// Create CR-calc file and TTree
	TFile *out_file = new TFile("test_CR-calc.root","RECREATE");
	TTree *t = new TTree("Table","CR-calc Data");
	
	UInt_t CR-raw_num;
	Long64_t event_num;
	UInt_t num_hits;
	UInt_t sum_tots;
	Double_t mean_x;
	Double_t mean_y;
	Double_t mean_z;
	Double_t R_xy;
	Double_t R_xyz;
	Double_t RMS_xy;
	Double_t RMS_xyz;

	t->Branch("CR-raw_num", &CR-raw_num, "CR-raw_num/i");
	t->Branch("event_num", &event_num, "event_num/L");
	t->Branch("num_hits", &num_hits, "num_hits/i");
	t->Branch("sum_tots", &sum_tots, "sum_tots/i");
	t->Branch("mean_x", &mean_x, "mean_x/D");
	t->Branch("mean_y", &mean_y, "mean_y/D");
	t->Branch("mean_z", &mean_z, "mean_z/D");
	t->Branch("R_xy", &R_xy, "R_xy/D");
	t->Branch("R_xyz", &R_xyz, "R_xyz/D");
	t->Branch("RMS_xy", &RMS_xy, "RMS_xy/D");
	t->Branch("RMS_xyz", &RMS_xyz, "RMS_xyz/D");

	// Calculations:
	
}
