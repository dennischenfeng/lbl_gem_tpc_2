
void view_root_tree() {
	/*** Prints the contents of the root ttree in the terminal window.

	Author: Dennis Feng
	***/
	gROOT->Reset(); 

	//--Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/46_module_202_new_stop_mode_ext_trigger_scan_interpreted_CRCalc.root");

	if (f == 0) { // if we cannot open the file, print an error message and return immediately
		printf("Error: cannot open the root file!\n");
		return;
	}


	TTree *t1 = (TTree*)f->Get("Table");
	t1->Print();
	t1->Show(0);
	// t1->Show();
}
