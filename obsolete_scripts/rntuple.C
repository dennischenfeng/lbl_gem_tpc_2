#include "Riostream.h"
void rntuple() {
//  Read data from an ascii file and create a root file with an ntuple.
//   see a variant of this macro in basic2.C
//Author: Rene Brun
      

// read file $ROOTSYS/tutorials/tree/basic.dat
// this file has 10 columns of float data
   TString dir = gSystem->UnixPathName(gInterpreter->GetCurrentMacroName());
   dir.ReplaceAll("rntuple.C","");
   dir.ReplaceAll("/./","/");
   ifstream in;
   in.open(Form("%sntuple061213.txt",dir.Data()));

   Float_t event,file,time,hits,tot,sumtot,x,y,z,meanx,meany,meanz,;
   Int_t nlines = 0;
   TFile *f = new TFile("ntuple061213.root","CREATE");
   TNtuple *ntuple = new TNtuple("ntuple","data from ascii file","event:file:time:hits:tot:sumtot:x:y:z:meanx:meany:meanz");

   while (1) {
      in >> event >> file >> time >> hits >> tot >> sumtot >> x >> y >> z >> meanx >> meany >> meanz;
      if (!in.good()) break;
      if (nlines < 5) printf("event=%8f, file=%8f, time=%8f, hits=%8f, tot=%8f, sumtot=%8f, x=%8f, y=%8f, z=%8f, meanx=%8f, meany=%8f, meany=%8f\n",event,file,time,hits,tot,sumtot,x,y,z,meanx,meany,meanz);
      ntuple->Fill(event,file,time,hits,tot,sumtot,x,y,z,meanx,meany,meanz);
      nlines++;
   }
   printf(" found %d points\n",nlines);

   in.close();

   f->Write();
}