
#include "TROOT.h"
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TCanvas.h"
#include "TH2F.h"

#include "helper_functions.cpp"

void view_all_events() {
	/*** 

	The h5_file_num chosen must have working Hits and EventsCR files (_Hits.root and _EventsCR.root files).

	***/
	gROOT->Reset();

	// Setting up files, treereaders, histograms
	string file_kind = "aggr"; // string that is either "aggr" or "non_aggr" to indicate whether or not its an aggregate file pair or not.
	int file_num_input = 23; // either an h5filenum or aggrfilenum
	string view_option = "4"; // choose what to view:
	// "1" or "zenith": zenith angle distribution
	// "2" or "tot_per_length": sum_tots_div_by_length_track distribution
	// "3" or "zenith_vs_totperlength": graph
	// "4" or "duration": duration distr
	// "5" or "cosine_zenith": cos theta distribution

	string file_string = ""; // ex: "AggrFileNum 3"
	TFile *fileHits;
	TFile *fileEventsCR;
	if (file_kind.compare("non_aggr") == 0) {
		file_string = "h5FileNum " + to_string(file_num_input);
		fileHits = new TFile(("/home/pixel/pybar/pybar_github/pybar/module_1/" + to_string(file_num_input) + "_module_1_stop_mode_ext_trigger_scan_interpreted_Hits.root").c_str());
		fileEventsCR = new TFile(("/home/pixel/pybar/pybar_github/pybar/module_1/" + to_string(file_num_input) + "_module_1_stop_mode_ext_trigger_scan_interpreted_EventsCR.root").c_str());
	} else if (file_kind.compare("aggr") == 0) {
		file_string = "AggrFileNum " + to_string(file_num_input);
		fileHits = new TFile(("/home/pixel/pybar/pybar_github/pybar/homemade_scripts/aggregate_data/" + to_string(file_num_input) + "_module_1_AggrHits.root").c_str());
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

	// Initialize the canvas
	TCanvas *c1 = new TCanvas("c1","3D Occupancy for Specified SM Event", 1000, 10, 900, 550);
	c1->SetRightMargin(0.25);
	
	// Initialize the histograms/graphs
	TH1F *h_zenith = new TH1F("h_zenith", ("Zenith Angle Distr for " + file_string).c_str(), 20, 0, 1.5708);
	h_zenith->GetXaxis()->SetTitle("Zenith angle (radians)");
	h_zenith->GetYaxis()->SetTitle("Count (events)");

	TH1F *h_tot_per_length = new TH1F("h_tot_per_length", ("Average ToT per Length Distr for " + file_string).c_str(), 50, 0, 160);
	h_tot_per_length->GetXaxis()->SetTitle("Average ToT per Length (tot/mm)");
	h_tot_per_length->GetYaxis()->SetTitle("Count (events)");
	
	TGraph *g_zenith_vs_totperlength = new TGraph();
	g_zenith_vs_totperlength->SetTitle(("Zenith vs ToTPerLength for " + file_string).c_str());
	g_zenith_vs_totperlength->GetXaxis()->SetTitle("ToT per length (tot/mm)");
	g_zenith_vs_totperlength->GetYaxis()->SetTitle("Zenith angle (radians)");
	g_zenith_vs_totperlength->GetXaxis()->SetRangeUser(0, 40); // ROOT is buggy, x and y use setlimits()
	g_zenith_vs_totperlength->GetYaxis()->SetRangeUser(0, 1.5708); // but z uses setrangeuser()

	TH1F *h_duration = new TH1F("h_duration", ("Duration Distr for " + file_string).c_str(), 255, 0, 255);
	h_duration->GetXaxis()->SetTitle("Duration (in BCIDs)");
	h_duration->GetYaxis()->SetTitle("Count (events)");

	TH1F *h_cos_zenith = new TH1F("h_cos_zenith", ("Cosine of Zenith Angle Distr for " + file_string).c_str(), 20, 0, 1);
	h_cos_zenith->GetXaxis()->SetTitle("Cos of Zenith angle");
	h_cos_zenith->GetYaxis()->SetTitle("Count (events)");
	h_cos_zenith->GetYaxis()->SetRangeUser(0, 210);


	int i = 0; // index for each entry in readerEventCR
	// Main Loop (loops for every entry in readerEventsCR)
	while (readerEventsCR->Next()) {
		if (readerEventsCR->GetCurrentEntry() == 0 && file_kind.compare("non_aggr") == 0) {
			continue; // skip the entry num 0, because it probably contains no data
		}

		// Get startEntryNum_Hits and endEntryNum_Hits (for readerHits)
		vector<int> entryNumRange_include(2);
		entryNumRange_include = getEntryNumRangeWithH5FileNumAndSMEventNum(readerHits, *h5_file_num_EventsCR, *SM_event_num_EventsCR);
		if (entryNumRange_include[0] == -1) {
			cout << "Error: h5_file_num and SM_event_num should be able to be found in the Hits file.\n";
		}



		// Fill
		if (view_option.compare("zenith") == 0 || view_option.compare("1") == 0) {
			if (true) { // @@@ 
				h_zenith->Fill(*zenith_angle);
			}
		} else if (view_option.compare("tot_per_length") == 0 || view_option.compare("2") == 0) {
			h_tot_per_length->Fill(*sum_tots_div_by_length_track);
		} else if (view_option.compare("zenith_vs_totperlength") == 0 || view_option.compare("3") == 0) {
			g_zenith_vs_totperlength->SetPoint(i, *sum_tots_div_by_length_track, *zenith_angle);
		} else if (view_option.compare("duration") == 0 || view_option.compare("4") == 0) {
			h_duration->Fill(*duration);
		} else if (view_option.compare("cos_zenith") == 0 || view_option.compare("5") == 0) {
			h_cos_zenith->Fill(TMath::Cos(*zenith_angle));
		}
		
		i++;
	}

	// Draw
	if (view_option.compare("zenith") == 0 || view_option.compare("1") == 0) {
		h_zenith->Draw("COLZ");
	} else if (view_option.compare("tot_per_length") == 0 || view_option.compare("2") == 0) {
		h_tot_per_length->Draw("COLZ");
	} else if (view_option.compare("zenith_vs_totperlength") == 0 || view_option.compare("3") == 0) {
		g_zenith_vs_totperlength->SetMarkerStyle(8);
		g_zenith_vs_totperlength->SetMarkerSize(1);
		g_zenith_vs_totperlength->SetLineColor(0);
		g_zenith_vs_totperlength->Draw();
	} else if (view_option.compare("duration") == 0 || view_option.compare("4") == 0) {
		h_duration->Draw("COLZ");
	} else if (view_option.compare("cos_zenith") == 0 || view_option.compare("5") == 0) {
		h_cos_zenith->Draw("COLZ");
	} else {
		cout << "Error: Input view_option is not valid.\n";
	}


	c1->Update();
}