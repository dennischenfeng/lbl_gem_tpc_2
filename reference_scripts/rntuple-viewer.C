void viewer()
{
char s[1];
char ev[20];
int i=0;
std::stringstream out;

TFile f("ntuple1009-1413-390-757torr-hitsG9.root", "READ");
TCanvas * c1 = new TCanvas("c1","histograms"); 
TH3F *h3 = new TH3F("h3","h3",80,0,20000,336,0,16800,24,0,6979.72245072);
h3->SetMarkerStyle(8);

while (s[0]!='q') {
i++;
sprintf(ev,"event== %d",i);
ntuple->Project("h3","z:y:x",ev,""); //make a histogram using cut defined in "ev"
if (h3->GetEntries()) //only go on to display histograms that are not empty
{
	h3->Draw();
	c1->Update();
	cout << ev << "\n Hit return for next event, q to quit";
	gets(s);  // "return" is enough, root waits until you hit a key
}
}
}