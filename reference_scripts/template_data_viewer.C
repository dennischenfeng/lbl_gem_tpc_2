//#include "TFile.h" // we don't really need these as of right now
//#include "TTree.h"

void template_data_viewer() {
/*** A template for accessing the raw data of ___interpreted.root files. 

***/

//--Setting up file, treereader, histogram
TFile *f = new TFile("/home/pixel/pybar/tags/2.0.1/host/pybar/module_test/91_module_test_fei4_self_trigger_scan_interpreted.root");
if (f == 0) { // if we cannot open the file, print an error message and return immediately
    printf("Error: cannot open the root file!\n");
    return;
}

TTreeReader tReader("Table", f);

TTreeReaderValue<Long64_t> event_number(tReader, "event_number");
TTreeReaderValue<UInt_t> trigger_number(tReader, "trigger_number");
TTreeReaderValue<UChar_t> relative_BCID(tReader, "relative_BCID");
TTreeReaderValue<UShort_t> LVL1ID(tReader, "LVL1ID");
TTreeReaderValue<UChar_t> column(tReader, "column");
TTreeReaderValue<UShort_t> row(tReader, "row");
TTreeReaderValue<UChar_t> tot(tReader, "tot");
TTreeReaderValue<UShort_t> BCID(tReader, "BCID");
TTreeReaderValue<UShort_t> TDC(tReader, "TDC");
TTreeReaderValue<UChar_t> TDC_time_stamp(tReader, "TDC_time_stamp");
TTreeReaderValue<UChar_t> trigger_status(tReader, "trigger_status");
TTreeReaderValue<UInt_t> service_record(tReader, "service_record");
TTreeReaderValue<UShort_t> event_status(tReader, "event_status");
TTreeReaderValue<Double_t> timestamp_start(tReader, "timestamp_start");
TTreeReaderValue<Double_t> timestamp_stop(tReader, "timestamp_stop");

TH2F *hEvent0Hits = new TH2F("hEvent0Hits", "Location of Hits in Event 0", 80, 0, 80, 336, 0, 336);
hEvent0Hits->GetXaxis()->SetTitle("Column");
hEvent0Hits->GetYaxis()->SetTitle("Row");

//--Fill in the histogram and draw it

//int num_events = 0; //this was to find out the total number of events in the TTree; found to be: 21987
int time_start = 0;
int time_end = 0;

while (tReader.Next()) {
    /*if (num_events < *event_number) {
        num_events = *event_number;
    }*/

    if (*event_number == 1) {
        hEvent0Hits->Fill(*column, *row);
    }

    /*if (*event_number == 0) {
        time_start = *timestamp_start;
    }
    if (*event_number == 1) {
        time_end = *timestamp_start;
    }*/
}

//printf("num_events: %d\n", num_events);
//printf("time_start: %d\n", time_start);
//printf("time_end: %d\n", time_end);
hEvent0Hits->SetMarkerStyle(7);
hEvent0Hits->Draw();

/*TTree *t1 = (TTree*)f->Get("Table");
t1->Print();
t1->Show(1000);*/

}
