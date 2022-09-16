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

#include "TGLHistPainter.h"
#include "TGLBoxPainter.h"
#include <stdlib.h>

#include <string>
#include "TH1.h"
#include "TRandom.h"
#include <vector>
#include <TGClient.h>
#include <TF1.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>
//using namespace std;

//TTree *eventtree = new TTree;


TString inputroot="../rootfiles/grinch_62.evio.0.root";
//TString inputroot="../rootfiles/e1209019_trigtest_10080.evio.0.root";
TFile *file = new TFile(inputroot);
TTree *eventtree = (TTree*) file->Get("T");
int N_evts = eventtree->GetEntries();


// Fill the arrays from ROOT branches (variables to be used anywhere)
///////////////////////////////////////////////////////////////////////////////
//eventtree->SetBranchAddress("Ndata.bb.grinch_tdc.tdc",&ntdc) ; // Number of TDC (LE) in event
//eventtree->SetBranchAddress("bb.grinch_tdc.elemID",&grinch_tdc_pmt) ; // PMT number
//eventtree->SetBranchAddress("bb.grinch_tdc.tdc",&grinch_tdc_le) ; // TDC LE
//eventtree->SetBranchAddress("bb.grinch_tdc.tdc_te",&grinch_tdc_te) ; // TDC TE

//eventtree->SetBranchAddress("bb.grinch_tdc.row",&grinch_pmt_row) ; // TDC row
//eventtree->SetBranchAddress("bb.grinch_tdc.col",&grinch_pmt_col) ; // TDC column
////////////////////////////////////////////////////////////////////////////////// 



#define N_ROW 60

//TTree *eventtree;

//int Event=-1;
int Event;
Int_t event_cnt;


//UTEventMin *event;
const Int_t maxHitsPerEvent = 100;
double* ped=NULL;
int* Hit_Events=NULL;
TCanvas* c;
TCanvas* c_heat;
TCanvas* c2;
TCanvas* allC;
TCanvas* allC_heat;

//int N_evts;

int eventID;

const int N_PMT=512;

//Array to count hits on each tube for entire run.
double TubeHits[N_PMT];
double multiTubeHits[N_PMT];
//Array to count hits on each tube for individual events.
double EvtTubeHits[N_PMT] = {};


// SBS-Offline variables
Int_t ntdc;
Double_t grinch_tdc_pmt[512];
Double_t grinch_tdc_le[200];
Double_t grinch_tdc_te[200];
Double_t grinch_tdc_tot[50];
Double_t grinch_tdc_mult[512];

Double_t grinch_pmt_row[512];
Double_t grinch_pmt_col[512];



Int_t i,j;
double cut = 2000;
double MaxHits = 100;

Int_t next_evt=0;
Int_t usr_evt;

Int_t tdc_event[N_PMT][10];
Int_t tdcTimeArray[N_PMT];
//Double_t tdcTime[N_PMT];
double tdcTime=0;

TPaveLabel** Signal_Label=new TPaveLabel*[N_PMT];
TEllipse** PMT=new TEllipse*[N_PMT];
TPaveLabel** PMT_Label=new TPaveLabel*[N_PMT];
Int_t eventEntries;

Color_t hit_color[10]={kRed, kPink+1, kMagenta, kViolet+7, kBlue, kAzure-4, kCyan, kTeal+3, kGreen, kYellow};
//Color_t hit_color_excess = kBlack;

TControlBar* bar = new TControlBar("","");

void load_data() {

   //TString basename="replayed_bbgrinch_33.evio.0";
   //TString inputroot="../../ReplayedFiles/replayed_bbgrinch_24.evio.0.root";
   //TString inputroot="../rootfiles/e1209019_trigtest_10042.evio.0.root";

TFile *file = new TFile(inputroot); 
TTree *eventtree = (TTree*) file->Get("T");

 Int_t ntdc;
 Double_t grinch_tdc_pmt[512];
 Double_t grinch_tdc_le[200];
 Double_t grinch_tdc_te[200];
 Double_t grinch_tdc_tot[50];

 Double_t grinch_pmt_row[512];
 Double_t grinch_pmt_col[512];


// Fill the arrays from ROOT branches (variables to be used anywhere)
///////////////////////////////////////////////////////////////////////////////
eventtree->SetBranchAddress("Ndata.bb.grinch_tdc.tdc",&ntdc) ; // Number of TDC (LE) in event
eventtree->SetBranchAddress("bb.grinch_tdc.tdcelemID",&grinch_tdc_pmt) ; // PMT number
eventtree->SetBranchAddress("bb.grinch_tdc.tdc",&grinch_tdc_le) ; // TDC LE
eventtree->SetBranchAddress("bb.grinch_tdc.tdc_te",&grinch_tdc_te) ; // TDC TE
eventtree->SetBranchAddress("bb.grinch_tdc.tdc_mult",&grinch_tdc_mult) ; // TDC multiplicity

eventtree->SetBranchAddress("bb.grinch_tdc.tdcrow",&grinch_pmt_row) ; // TDC row
eventtree->SetBranchAddress("bb.grinch_tdc.tdccol",&grinch_pmt_col) ; // TDC column
////////////////////////////////////////////////////////////////////////////////// 
 

// Histograms
 TH1F *h_ntdc = new TH1F("h_ntdc"," ; Nhit ; Counts",512,0,511); // 
 TH2F *h_tdc_vs_pmt = new TH2F("h_tdc_vs_pmt"," ; PMT # ; TDC LE", 512,0,511,150,1,150);
 TH2F *h_adc_vs_chan = new TH2F("h_adc_vs_chan"," ; chan # ; ADC counts", 64,0,63,200,1,200);
 TH2F *GRINCH_mult_vs_pmt = new TH2F("GRINCH_mult_vs_pmt"," ; PMT # ; PMT hit multiplicity", 512,0,511,20,1,20);

///////////////////
// Loop over events
//////////////////

// Long64_t nentries = eventtree->GetEntries();
//	for (int i = 0; i < nentries; i++) {
//      		eventtree->GetEntry(i);		
//                if (i%1000==0) cout << " Entry = " << i << endl;
//		h_ntdc->Fill(float(ntdc));
//
//                // Loop over TDC LE
//		for (Int_t n=0;n<ntdc;n++) {
//		  h_tdc_vs_pmt->Fill(grinch_tdc_pmt[n],grinch_tdc_le[n]);
//		}
//	}  // event loop

/////////////////////////////////
///// OUTPUT HISTOGRAMS .png ////
////////////////////////////////

	//TCanvas *c1 = new TCanvas("c1", "c1",10,10,1000,750);
	//c1->SetLogz();

	//char outputname[200];

	//h_tdc_vs_pmt->Draw("COLZ");
	//strcpy(outputname,"../monitoringPlots/h_tdc_vs_pmt");
	//c1->SaveAs(strcat(outputname,".png"));

	} // END load_data


	void set_color_env(){
	  //Create a nice color gradient.
	  const Int_t NRGBs = 5;
	  const Int_t NCont = 255;

	  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
	  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
	  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
	  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
	  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
	  gStyle->SetNumberContours(NCont);   //Chooses custom gradient NCont.
	}


	void draw_event_display()
	{
	  //*************************************
	  //DOUBLE LOOP FOR EVENT DISPLAY CREATION
	  ///////////////////////////////////////

	  if (c == NULL) c=new TCanvas("c");

	  load_data();

	  double PMT_dia=2./(59.0); //the whole canvas has a max size of 1. We divide by the number of rows. 
	  double x,y;//center of PMT. Draws PMT array.

	  double labelsize=0.2;
	  Int_t k=0;

	  double y_off = -0.25;

	  cout<<"Creating Event Display..."<<endl;
	  for ( i=0; i<N_ROW; i++ )
	    {
	      int N_COL = 8 + i%2;
	      for ( j=0; j<N_COL; j++ )
		{
		  if ( i%2==0 )
		    {
		      x=(j+1.5)*PMT_dia +0.1;
		      y=N_ROW*PMT_dia-(i*sin(60*TMath::DegToRad())+0.5)*PMT_dia + y_off;
		    }
		  else
		    {
		      x=(j+1)*PMT_dia  +0.1;
		      y=N_ROW*PMT_dia-(i*sin(60*TMath::DegToRad())+0.5)*PMT_dia + y_off;
		    }
		  // cout<<"ROW: "<<i<<" PMT: "<<k<<" X: "<<x<<" Y: "<<y<<endl;
		  PMT_Label[k]=new TPaveLabel(x-PMT_dia*labelsize, y-PMT_dia*labelsize, x+PMT_dia*labelsize, y+PMT_dia*labelsize, Form("%d",k));
		  PMT[k]=new TEllipse(x, y, PMT_dia*0.5, PMT_dia*0.5);

		  if ( i==0 && j==0 )
		    PMT[k]->Draw();
		  else
		    PMT[k]->Draw("same");

		  PMT_Label[k]->SetFillColor(0);
		  //PMT_Label[k]->SetBorderSize(1);
		  //PMT_Label[k]->SetTextSize(0.5);
		  PMT_Label[k]->SetTextSize(1);
		  PMT_Label[k]->Draw("same");

		  Signal_Label[k]=new TPaveLabel(x-1.5*PMT_dia*labelsize,y+PMT_dia*labelsize,x+1.5*PMT_dia*labelsize,y+2*PMT_dia*labelsize,"");
		  Signal_Label[k]->SetTextColor(2);
		  Signal_Label[k]->SetFillColor(0);
		  Signal_Label[k]->SetLabel("");
		  k++;
		}
	    }//End building event display geometry.
	  cout<<"...Event Display Created"<<endl;
	  //*************************************
	} // Event Display function


	void fill_event(Int_t Event)
	{
	  //load_data(); // probably not needed here, just testing

	  allC =  new TCanvas("allC","Multiplicity vs. PMT",800,800);
	  allC -> SetCanvasSize(700, 400);
	  allC -> SetWindowSize(700, 450);

	  allC -> Divide(8,8);


	//*** Variables ***********************************************
	  eventtree->SetBranchAddress("Ndata.bb.grinch_tdc.tdc",&ntdc) ; // Number of TDC (LE) in event
	  eventtree->SetBranchAddress("bb.grinch_tdc.tdcelemID",&grinch_tdc_pmt) ; // PMT number
	  eventtree->SetBranchAddress("bb.grinch_tdc.tdc",&grinch_tdc_le) ; // TDC LE
	  eventtree->SetBranchAddress("bb.grinch_tdc.tdc_te",&grinch_tdc_te) ; // TDC TE
	  eventtree->SetBranchAddress("bb.grinch_tdc.tdc_mult",&grinch_tdc_mult) ; // TDC multiplicity
	  eventtree->SetBranchAddress("bb.grinch_tdc.tdcrow",&grinch_pmt_row) ; // TDC row
	  eventtree->SetBranchAddress("bb.grinch_tdc.tdccol",&grinch_pmt_col) ; // TDC column
	//*************************************************************

	// GRINCH_mult_vs_pmt = new TH2F("GRINCH_mult_vs_pmt"," ; PMT # ; PMT hit multiplicity", 512,0,511,20,1,20);


	  cout<<"Event # = "<<Event<<endl;


	  if ( c != NULL)
	    {
	      c->Clear();
	    }
	  else
	    {
	      c=new TCanvas("c");
	    }

	  c -> Range(0,0,1.5, 1.8);// This changes the range of the canvas
	  c -> SetCanvasSize(1000, 1000);
	  c -> SetWindowSize(400, 2500);

	  if( c2 )
	    {
	      c->cd();
	    }


	  draw_event_display();

	  eventtree->GetEntry(Event);

	  const Int_t nChanADC = 64;
	  const Int_t nChanVETROC = 512;

	  //const Int_t TDC_array = 1000;
  const Int_t TDC_array = 10;

  for(int r=0; r<N_PMT; r++)  //Reset individual events tube hits counter for each event.
    {
      EvtTubeHits[r] = 0;
      tdcTimeArray[r] = 0;
    }

  TH1F *h_GRINCH_mult = new TH1F("h_GRINCH_mult"," ; PMT hit multiplicity", 512,0,511);
  TH2F *GRINCH_mult_vs_pmt = new TH2F("GRINCH_mult_vs_pmt"," ; PMT # ; PMT hit multiplicity", 512,0,511,10,1,10);

  for(int j=0; j<nChanVETROC; j++)
    {
        //TH2F *GRINCH_mult_vs_pmt = new TH2F("GRINCH_mult_vs_pmt"," ; PMT # ; PMT hit multiplicity", 512,0,511,20,1,20);
        //h_GRINCH_mult->
        h_GRINCH_mult->SetBinContent(j,grinch_tdc_mult[j]);
        GRINCH_mult_vs_pmt->Fill( grinch_tdc_pmt[j] , grinch_tdc_mult[j] );


        //h_GRINCH_mult->Fill(j,grinch_tdc_mult[j]);
        

        int counts = 0;
//      for(int co = 0; co < TDC_array; co++)
//        {
            cout<<"grinch_tdc_mult[j]  = "<<grinch_tdc_mult[j]<<endl;
            cout<<"h_GRINCH_mult[j]  = "<<h_GRINCH_mult->GetBinContent(j)<<endl;

          //if ( (tdc_event[j][co] == -1000)||(tdc_event[j][co]<380 ||tdc_event[j][co]>450) ) break;//the extra conditions is to take only the cosmic peaks
          //if ( (tdc_event[j][co] == -10) || (tdc_event[j][co] == -20)) break; // Selects events where the PMT has a signal
          //if ( (tdc_event[j][co] < 220) || (tdc_event[j][co] > 270)) break; // Selects events within TDC peak (run 3016)
          //if ( (tdc_event[j][co] < 250) || (tdc_event[j][co] > 290)) break; // Run 2054 (Thanksgiving run)
          //if ( (tdc_event[j][co] < 700) || (tdc_event[j][co] > 760)) break; // run 3022
          //if ( (tdc_event[j][co] < 180) || (tdc_event[j][co] > 230)) break; // Run 2054 (64 ADC run)
          //if ( (tdc_event[j][co] < 200) || (tdc_event[j][co] > 270)) break; // Run 3110 (64 ADC run)
          //if ( grinch_tdc_le[j] < 0) break; // Run 33 (CODA3)
          //if ( (grinch_tdc_le[j] < 40) || (grinch_tdc_le[j] > 50)) break; // Run 33 (CODA3)
          //tdcTimeArray[j]++;
          //counts = sum(grinch_tdc_mult[j]);
          //tdcTimeArray[j] = grinch_tdc_mult[j];
          tdcTimeArray[j] = h_GRINCH_mult->GetBinContent(j);
          //tdcTimeArray[j] = GRINCH_mult_vs_pmt->GetBinContent(j);
//        }

      //tdcTimeArray[j] = eventtree->GetEntries(Form("fall_%i>220&&fall_%i<270", j)); // select events in the TDC peak      
      // if (j==0) flag=1;//something avoid the correct read of this value

        if(tdcTimeArray[j] > 0)
        //if(true)
        {
          Int_t d = j;
          Int_t color_shift = tdcTimeArray[d]-1;


          //      cout<<"PMT: "<<d<<" value: "<<tdcTime<<endl;
          EvtTubeHits[d] = EvtTubeHits[d] + 1;

          Signal_Label[d]->SetFillColor(hit_color[color_shift]);

        if(tdcTimeArray[j]<=10)
        //if(true)
        {
          Signal_Label[d]->SetTextColor(kBlack);
          PMT[d]->SetFillColor(hit_color[color_shift]);
          PMT_Label[d]->SetFillColor(hit_color[color_shift]);

        }else{
          Signal_Label[d]->SetTextColor(kRed);
          PMT[d]->SetFillColor(kBlack);
          PMT_Label[d]->SetFillColor(kRed);
          }

          Signal_Label[d]->SetTextSize(2);
          cout<<"tdcTimeArray[j]: "<<tdcTimeArray[d]<<" d: "<<d<<endl;
          Signal_Label[d]->SetLabel(Form("%i",tdcTimeArray[d]));

          Signal_Label[d]->Draw("same");

        }

    }

  allC->cd()->SetLogy(0);
  //style(0);
  gStyle->SetOptStat(0);
//  spe->GetYaxis()->SetRangeUser(0,50);
  GRINCH_mult_vs_pmt->SetXTitle("PMT #");
  GRINCH_mult_vs_pmt->SetYTitle("multiplicity");
  GRINCH_mult_vs_pmt->Draw("COLZ");
  GRINCH_mult_vs_pmt->Draw("text same");
  //spe2->Draw("same");

  c->SetTitle(Form("Event %d",Event));
  allC->SetTitle(Form("Event %d",Event));

  c->Modified(); c->Update(); gSystem->ProcessEvents();
  allC->Modified(); allC->Update(); gSystem->ProcessEvents();
  //allC2->Modified(); allC2->Update(); gSystem->ProcessEvents();
  //allC3->Modified(); allC3->Update(); gSystem->ProcessEvents();

  //c->SetTitle(Form("Event %d",Event));
  //allC->SetTitle(Form("Event %d",Event));

} // END FILL EVENT



void heat_map(Int_t NoOfEve)
{
  //load_data(); // probably not needed here, just testing

  allC_heat =  new TCanvas("allC_heat","Multiplicity vs. PMT (all)",800,800);
  allC_heat -> SetCanvasSize(1000, 400);
  allC_heat -> SetWindowSize(1000, 450);

  allC_heat -> Divide(8,8);


//*** Variables ***********************************************
  eventtree->SetBranchAddress("Ndata.bb.grinch_tdc.tdc",&ntdc) ; // Number of TDC (LE) in event
  eventtree->SetBranchAddress("bb.grinch_tdc.elemID",&grinch_tdc_pmt) ; // PMT number
  eventtree->SetBranchAddress("bb.grinch_tdc.tdc",&grinch_tdc_le) ; // TDC LE
  eventtree->SetBranchAddress("bb.grinch_tdc.tdc_te",&grinch_tdc_te) ; // TDC TE
  eventtree->SetBranchAddress("bb.grinch_tdc.tdc_mult",&grinch_tdc_mult) ; // TDC multiplicity
  eventtree->SetBranchAddress("bb.grinch_tdc.row",&grinch_pmt_row) ; // TDC row
  eventtree->SetBranchAddress("bb.grinch_tdc.col",&grinch_pmt_col) ; // TDC column
//*************************************************************

// GRINCH_mult_vs_pmt = new TH2F("GRINCH_mult_vs_pmt"," ; PMT # ; PMT hit multiplicity", 512,0,511,20,1,20);


  cout<<"Event # = "<<Event<<endl;


  if ( c_heat != NULL)
    {
      c_heat->Clear();
    }
  else
    {
      c_heat=new TCanvas("c_heat");
    }

  c_heat -> Range(0,0,1.5, 1.8);// This changes the range of the canvas
  c_heat -> SetCanvasSize(1000, 1000);
  c_heat -> SetWindowSize(400, 2500);

//  if( c2 )
//    {
//      c_heat->cd();
//    }

  draw_event_display();
  const Int_t nChanVETROC = 512;


  TH1F *h_GRINCH_mult = new TH1F("h_GRINCH_mult"," ; PMT hit multiplicity", 512,0,511);
  TH2F *GRINCH_mult_vs_pmt = new TH2F("GRINCH_mult_vs_pmt"," ; PMT # ; PMT hit multiplicity", 512,0,511,20,1,20);

//Long64_t nentries = eventtree->GetEntries();
// for (int i = 0; i < nentries; i++) {


// Long64_t nentries = eventtree->GetEntries();
 for (int i = 0; i < NoOfEve; i++) {

     if (i%1000==0) cout << " Entry = " << i << endl;

  //TH1F *h_GRINCH_mult = new TH1F("h_GRINCH_mult"," ; PMT hit multiplicity", 512,0,511);
  //TH2F *GRINCH_mult_vs_pmt = new TH2F("GRINCH_mult_vs_pmt"," ; PMT # ; PMT hit multiplicity", 512,0,511,20,1,20);


     for(int j=0; j<nChanVETROC; j++)
       {
          // ALL THE STUFF
        h_GRINCH_mult->Fill(grinch_tdc_mult[j]);
        GRINCH_mult_vs_pmt->Fill( grinch_tdc_pmt[j] , grinch_tdc_mult[j] );

        tdcTimeArray[j] = h_GRINCH_mult->GetBinContent(j);

       } //PMT loop
     } // event loop

           if(tdcTimeArray[j] > 0)
           //if(true)
           {
             Int_t d = j;
             Int_t color_shift = tdcTimeArray[d]-1;


             //      cout<<"PMT: "<<d<<" value: "<<tdcTime<<endl;
             EvtTubeHits[d] = EvtTubeHits[d] + 1;

           //  Signal_Label[d]->SetFillColor(hit_color[color_shift]);

           //if(tdcTimeArray[j]>0)
           if(true)
           {
             Signal_Label[d]->SetTextColor(kBlack);
             PMT[d]->SetFillColor(hit_color[color_shift]);
             PMT_Label[d]->SetFillColor(hit_color[color_shift]);

           }

          Signal_Label[d]->SetTextSize(2);
          cout<<"tdcTimeArray[j]: "<<tdcTimeArray[d]<<" d: "<<d<<endl;
          Signal_Label[d]->SetLabel(Form("%i",tdcTimeArray[d]));

          Signal_Label[d]->Draw("same");

        }


  allC_heat->cd()->SetLogz(1);
  //style(0);
  gStyle->SetOptStat(0);
//  spe->GetYaxis()->SetRangeUser(0,50);
  GRINCH_mult_vs_pmt->SetXTitle("PMT #");
  GRINCH_mult_vs_pmt->SetYTitle("multiplicity");
  GRINCH_mult_vs_pmt->Draw("COLZ");
  GRINCH_mult_vs_pmt->Draw("text same");
  //spe2->Draw("same");

  c_heat->SetTitle(Form("Event %d",Event));
  allC_heat->SetTitle(Form("Event %d",Event));
  c_heat->Modified(); c_heat->Update(); gSystem->ProcessEvents();
  allC_heat->Modified(); allC_heat->Update(); gSystem->ProcessEvents();
  //allC2->Modified(); allC2->Update(); gSystem->ProcessEvents();
  //allC3->Modified(); allC3->Update(); gSystem->ProcessEvents();

  //c->SetTitle(Form("Event %d",Event));
  //allC->SetTitle(Form("Event %d",Event));

} // END HEAT MAP


// LOAD THE GUI WITH BUTTONS
void show(int option=0)
{

  cout<<" Option: "<<option<<endl;

  int event;
  int n;



  double* adc=new double[N_PMT];
//  Double_t* tdc=new double[N_PMT][70];
  int* Nadc=new int[N_PMT];
  int* Ntdc=new int[N_PMT];

   N_evts = eventEntries-1;//Max index of events is number of entries-1  

//   N_evts = 100; // For testing

  //if(N_evts>100) N_evts=100;//for debugging puposes (CA)
  cout << "TOTAL EVENTS: " << eventEntries << endl;

  cout << "BEFORE EVENT: " << event_cnt << endl;


  //MENU SELECTION

  if ( option==-2 )   //First Event
    {
      fill_event(0);
      event_cnt = 0;
    }
  else if ( option==-3 ) //Last Event 
    {
      fill_event(N_evts);
      event_cnt = N_evts;
    }
  else if ( option==0 ) //Ask Event
    {
      Printf("Please Enter Event Number:");
      cin>>usr_evt;
      fill_event(usr_evt);
      event_cnt = usr_evt;
    }

     else if ( option==1 ) //Next Event
    {
      //event_cnt++;
      fill_event(event_cnt+1);
      event_cnt++;
    }


  else if ( option==2 ) //Next Event with hit
    {
      event_cnt++;
      if ( event_cnt<0 )
        {
          fill_event(0);
          event_cnt = 0;
        }
      else if ( event_cnt>N_evts )
        {
          fill_event(N_evts);
          event_cnt = N_evts;
        }
//      j=0;
//      while (event_cnt > Hit_Events[j] && Hit_Events[j] > -1 && event_cnt < N_evts )
//      {
//        j++;
//      }
//      fill_event(Hit_Events[j]);
//      event_cnt = Hit_Events[j];

   //   j=event_cnt;
   //   while(tdcTimeArray[j]==0)
   //   {
   //     j++;
   //   }
      //if(tdcTimeArray[event_cnt]>0){fill_event(event_cnt);}
      //event_cnt = tdcTimeArray[j];

      for(int j=event_cnt; j<N_evts; j++)
    {
      for(int co = 0; co < 1; co++)
        {
          //if ( (tdc_event[j][co] == -1000)||(tdc_event[j][co]<380 ||tdc_event[j][co]>450) ) break;//the extra conditions is to take only the cosmic peaks
          if(true)
          //if ( grinch_tdc_le[j] > 0 )
          {
           //event_cnt = eventtree->GetEntries(Form("fall_%i>220&&fall_%i<270", j)); // select events in the TDC peak
           fill_event(event_cnt);
          }
        }
     }

    }

  else if ( option==3 ) //Prev Event with hit
    {
      event_cnt--;
      if ( event_cnt<0 )
        {
          fill_event(0);
          event_cnt = 0;
        }
      else if ( event_cnt>N_evts )
        {
          fill_event(N_evts);
          event_cnt = N_evts;
        }
      j=0;
      while ( event_cnt>Hit_Events[j] && Hit_Events[j]>-1 && event_cnt<N_evts )
        {
          j++;
        }
      if ( j>0 )
        {
          fill_event(Hit_Events[j-1]);
        }
      else
        {
          Event=Hit_Events[0];
        }
    }


  else if ( option == 4)   //Create heat map of all tube hits. (add if needed)
    {
      event_cnt = N_evts;
      heat_map(N_evts);
      //event_cnt = N_evts;
    }

  else if ( option == 5)   //Event counter (add if needed)
    {
      //fill_event(0);
      //draw_event_display();
      //rate_counter(N_evts);
      //fill_event(0);
    }

  else if ( option == 1)
    {
      event_cnt++;
      if ( event_cnt<0 )
        {
          Printf("You reached the first event");
          fill_event(N_evts);
          event_cnt = N_evts;
        }

      else if ( event_cnt>N_evts )
        {
          Printf("You reached the last event");
          fill_event(0);
          event_cnt = 0;
        }
      else
        {
          fill_event(event_cnt);
        }
    }

  else if ( option == -1)
    {
      event_cnt--;
      if ( event_cnt<0 )
        {
          Printf("You reached the first event");
          fill_event(N_evts);
          event_cnt = N_evts;
        }

      else if ( event_cnt>N_evts )
        {
          Printf("You reached the last event");
          fill_event(0);
          event_cnt = 0;
        }
      else
        {
          fill_event(event_cnt);
        }
    }

  cout<<"AFTER EVENT: "<<event_cnt<<endl;

}// END show function


void GRINCH_eventDisplay()
{
  load_data();

  //Create GUI
  bar = new TControlBar("vertical", "GRINCH Event Display");
  bar->AddButton("GRINCH Event Display", "");
  bar->AddButton("", "");
  bar->AddButton("First Event", "show(-2)");
  bar->AddButton("Next Event", "show(1)");
  bar->AddButton("Prev. Event", "show(-1)");
  bar->AddButton("Last Event", "show(-3)");
  bar->AddButton("", "");
  //bar->AddButton("Next with Hit", "show(2)");
  //bar->AddButton("Prev whit Hit", "show(3)");
  bar->AddButton("Choose Event #", "show(0)");
  bar->AddButton("", "");
  bar->AddButton("Heat Map", "show(4)");
  //bar->AddButton("PMT Counter", "show(5)");
  bar->AddButton("", "");
  bar->AddButton("Exit",".q");
  bar->Show();

}

