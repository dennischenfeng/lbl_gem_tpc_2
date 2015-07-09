
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

void view_occupancy() {
	/*** Used only when input file is an _interpreted_Hits.root file (called Hits file). Displays a 1D histogram of BCIDs per SM event. 
	***/
	gROOT->Reset(); 

	//--Setting up file, treereader, histogram
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/44_module_202_new_stop_mode_ext_trigger_scan_interpreted_Hits.root");

	if (!f) { // if we cannot open the file, print an error message and return immediately
		cout << "Error: cannot open the root file!\n";
		//return;
	}

	TTreeReader reader("Table", f);

	TTreeReaderValue<UInt_t> h5_file_num(reader, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(reader, "event_number");
	TTreeReaderValue<UChar_t> tot(reader, "tot");
	TTreeReaderValue<UChar_t> relative_BCID(reader, "relative_BCID");
	TTreeReaderValue<Double_t> x(reader, "x");
	TTreeReaderValue<Double_t> y(reader, "y");
	TTreeReaderValue<Double_t> z(reader, "z");

	// Initialize the histogram
	TCanvas *c1 = new TCanvas("c1","Occupancy Plot of All Events");
	TH2F *h = new TH2F("h", "Occupancy Plot of All Events", 80, 0, 20, 336, -16.8, 0);
	h->GetXaxis()->SetTitle("x (mm)");
	h->GetYaxis()->SetTitle("y (mm)");
	//h->SetMarkerStyle(7);

	while (reader.Next()) {
		
		h->Fill(*x, *y);
	}

	h->Draw("COLZ");
    c1->Update();
}