{
	TFile *f = new TFile("/home/pixel/pybar/tags/2.0.2_new/pyBAR-master/pybar/module_202_new/54_module_202_new_stop_mode_ext_trigger_scan_interpreted_raw.root");

	TTreeReader *reader = new TTreeReader("Table", f);

	TTreeReaderValue<UInt_t> h5_file_num(*reader, "h5_file_num");
	TTreeReaderValue<Long64_t> event_number(*reader, "event_number");
}
