#include <TSystem.h>
#include <TString.h>
#include "TFile.h"
#include "TTree.h"
#include <TNtuple.h>
#include "TCanvas.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TH1F.h"
#include <TH2.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TMath.h>
#include <TLegend.h>
#include <TPaveLabel.h>
#include <TProfile.h>
#include <TPolyLine.h>
#include <TObjArray.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include<math.h>
#include <TRandom3.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <TDecompSVD.h>

#include <stdio.h>
#include <stdlib.h>

using namespace std;

const Int_t nTDC = 512;
const Int_t nADC = 64;

void GRINCH_offlineMon(TString basename="grinch_") {
//void GRINCH_offlineMon(TString basename="e1209019_trigtest_") {


//!!!!!!!!!!!!!!!!!
//ONLY CHANGE THIS!
Int_t runnum = 62;
int runnum_int = 62;
//!!!!!!!!!!!!!!!!!

//void GRINCH_offlineMon(TString basename="e1209019_trigtest_10080.evio.0") {

   TString inputroot;
   inputroot="../rootfiles/"+basename+runnum+".evio.0.root";


TFile *file = new TFile(inputroot); 
TTree *eventtree = (TTree*) file->Get("T");
 Int_t ntdc;
 Double_t grinch_tdc_pmt[nTDC];
 Double_t grinch_tdc_le[nTDC];
 Double_t grinch_tdc_te[nTDC];
 Double_t grinch_tdc_mult[nTDC];
 Double_t grinch_tdc_tot[nTDC];

 Double_t grinch_adc_pmt[nADC];
 Double_t grinch_adc[nADC];

//********************************************************************************************************************
// Fill the arrays from ROOT branches (variables to use)
 eventtree->SetBranchAddress("Ndata.bb.grinch_tdc.tdc",&ntdc) ; // Number of TDC (LE) in event
 eventtree->SetBranchAddress("bb.grinch_tdc.tdcelemID",&grinch_tdc_pmt) ; // PMT number
 eventtree->SetBranchAddress("bb.grinch_tdc.tdc",&grinch_tdc_le) ; // TDC LE
 eventtree->SetBranchAddress("bb.grinch_tdc.tdc_te",&grinch_tdc_te) ; // TDC TE
 eventtree->SetBranchAddress("bb.grinch_tdc.tdc_mult",&grinch_tdc_mult) ; // TDC multiplicity
// TDC time over threshold
// The SBSGenericDetector variable needs work though, so I just do it manually here.
// eventtree->SetBranchAddress("bb.grinch_tdc.tot",&grinch_tdc_tot) ;

 eventtree->SetBranchAddress("bb.grinch_adc.adcelemID",&grinch_adc_pmt) ; // ADC number
// eventtree->SetBranchAddress("bb.grinch_adc.a",&grinch_adc) ; // ADC counts
 eventtree->SetBranchAddress("bb.grinch_adc.a_amp_p",&grinch_adc) ; // ADC counts


// TDC histograms
 TH1F *GRINCH_ntdc = new TH1F("GRINCH_ntdc"," ; Nhit ; Counts",512,0,511); // 
 TH2F *GRINCH_tdc_LE_vs_pmt = new TH2F("GRINCH_tdc_LE_vs_pmt"," ; PMT # ; TDC leading edge (ns)", 512,0,511,100,1,100);
 TH2F *GRINCH_tdc_TE_vs_pmt = new TH2F("GRINCH_tdc_TE_vs_pmt"," ; PMT # ; TDC trailing edge (ns)", 512,0,511,100,1,100);
 TH2F *GRINCH_mult_vs_pmt = new TH2F("GRINCH_mult_vs_pmt"," ; PMT # ; PMT hit multiplicity", 512,0,511,20,1,20);
 TH2F *GRINCH_tdc_ToT_vs_pmt = new TH2F("GRINCH_tdc_ToT_vs_pmt"," ; PMT # ; Time over threshold (ns)", 512,0,511,50,1,50);

// ADC histograms
 TH2F *GRINCH_adc_chan_vs_num = new TH2F("GRINCH_adc_chan_vs_num"," ; ADC # ; ADC channel",64,0,63,501, -0.5, 500.5);

//*********************************************************************************************************************

// Loop over events
 Long64_t nentries = eventtree->GetEntries();
	for (int i = 0; i < nentries; i++) {
      		eventtree->GetEntry(i);		
                if (i%1000==0) cout << " Entry = " << i << endl;
		GRINCH_ntdc->Fill(float(ntdc));

                // Loop over TDC (PMTs)
		for (Int_t n=0;n<nTDC;n++) {
		  GRINCH_tdc_LE_vs_pmt->Fill( grinch_tdc_pmt[n],grinch_tdc_le[n] );
                  GRINCH_tdc_TE_vs_pmt->Fill( grinch_tdc_pmt[n],grinch_tdc_te[n] );
                  GRINCH_mult_vs_pmt->Fill( grinch_tdc_pmt[n],grinch_tdc_mult[n] );
                  GRINCH_tdc_ToT_vs_pmt->Fill( grinch_tdc_pmt[n],(grinch_tdc_te[n]-grinch_tdc_le[n]) );
                  //h_ToT_vs_pmt->Fill( grinch_tdc_pmt[n],grinch_tdc_tot[n] ); // The SBS-Offline variable isn't filled correctly...

		}
                // Loop over ADC
                for (Int_t n=0;n<nADC;n++) {
                   GRINCH_adc_chan_vs_num->Fill( grinch_adc_pmt[n],grinch_adc[n] );
                }

	}  // event loop

///// OUTPUT HISTOGRAMS .png //////////////////////////////////////////////////////
TCanvas *c1 = new TCanvas("c1", "c1",10,10,1000,750);
c1->SetLogz();
gStyle->SetOptStat(11);
gStyle->SetStatX(0.60);

char outputname[200];


//char *runStr = itoa(runnum);
//string run = string(runStr);

GRINCH_mult_vs_pmt->Draw("COLZ");
strcpy(outputname,"../plots/grinch/");
//strcat(outputname,Form(%d,runnum_int));
c1->SaveAs(strcat(outputname,"grinch_tdc_mult.png"));

//strcpy(outputname,"../plots/grinch/monitoring/run");
//strcat(outputname,run);
//strcat(outputname,"grinch_tdc_mult.png");
//c1->SaveAs(strcat(outputname,"_grinch_tdc_mult.png"));

GRINCH_tdc_LE_vs_pmt->Draw("COLZ");
strcpy(outputname,"../plots/grinch/");
c1->SaveAs(strcat(outputname,"grinch_tdc_LE.png"));

GRINCH_tdc_TE_vs_pmt->Draw("COLZ");
strcpy(outputname,"../plots/grinch/");
c1->SaveAs(strcat(outputname,"grinch_tdc_TE.png"));

GRINCH_tdc_ToT_vs_pmt->Draw("COLZ");
strcpy(outputname,"../plots/grinch/");
c1->SaveAs(strcat(outputname,"grinch_tdc_ToT.png"));

GRINCH_adc_chan_vs_num->Draw("COLZ");
strcpy(outputname,"../plots/grinch/");
c1->SaveAs(strcat(outputname,"grinch_adc.png"));

}

