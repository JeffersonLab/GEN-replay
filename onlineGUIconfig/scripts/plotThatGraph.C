//Macro created by Zeke Wertz 06/14/2022 to draw TGraphs because Panguin does not like to do that
#include "TGraph.h"
#include "TStyle.h"
#include "TFile.h"

void plotThatGraph(TString myname){
	TGraph  *myGraph;
	myGraph = (TGraph*)(gFile->Get(myname));
	myGraph->SetMarkerStyle(kFullDotLarge);
	myGraph->Draw("AP");
}
