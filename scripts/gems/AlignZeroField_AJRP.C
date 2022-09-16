#include "TVector3.h"
#include "TMinuit.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TRandom3.h"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCut.h"
#include "TCutG.h"
#include "TEventList.h"
#include "TMinuit.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TLine.h"
#include "TText.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>

const int nxsieve = 13;
const int nysieve = 7;

const double PI = TMath::Pi();

//Example code to align the GEM stack relative to the spectrometer coordinate system and determine distance from target to sieve plate using
//known dimensions and hole spacings of sieve and known dimensions and internal alignments of GEMs:
//We are going to be lazy and use MINUIT even though this problem could easily be linearized:

const double sigma_pos = 0.0001; //0.1 mm (wild guess)
const double sigma_slope = sigma_pos/1.6; //0.1 mm/1.6 m ~= 6e-5 gives rough order of magnitude for slope uncertainty, of course multiple scattering might invalidate that.

int NTRACKS;

//These arrays will be filled by Holly's code to select tracks going through the individual sieve holes:
vector<double> XTRACK,YTRACK,XPTRACK,YPTRACK; //Measured track parameters in GEM internal coordinate system
vector<double> XSIEVE,YSIEVE; //known x and y center positions of the holes the tracks went through.
//We assume the single foil position defines the global origin and acts as a point source of straight-line rays coming through the sieve holes into the GEMS

void CHI2_FCN( int &npar, double *gin, double &f, double *par, int flag ){
  double chi2 = 0.0;

  //Parameters are:
  
  // zsieve: distance from target to sieve along spectrometer central ray
  // (x0,y0,z0): coordinates of the GEM origin relative to the target center in spectrometer coordinates
  // (theta,phi): polar and azimuthal angles of the Z axis of the GEM stack relative to the spectrometer central ray:
  TVector3 GEMPOS(par[0], par[1], par[2] );
  
  double ZSIEVE = par[3];
  double thetaGEM = par[4];
  double phiGEM = par[5]; 

  //Define unit vectors along the GEM internal axes with respect to the global system:
  TVector3 GEM_zaxis( sin(thetaGEM)*cos(phiGEM), sin(thetaGEM)*sin(phiGEM), cos(thetaGEM) );

  TVector3 Global_yaxis(0,1,0);
  //Not clear if we will need these:
  TVector3 Global_xaxis(1,0,0);
  TVector3 Global_zaxis(0,0,1); 
  
  TVector3 GEM_xaxis = (Global_yaxis.Cross( GEM_zaxis )).Unit();
  TVector3 GEM_yaxis = (GEM_zaxis.Cross( GEM_xaxis )).Unit();
  
  chi2 = 0.0;

  for( int i=0; i<NTRACKS; i++ ){
    //Track position at sieve plate: XSIEVE and YSIEVE have to be determined by Holly's code:
    TVector3 TrackSievePos( XSIEVE[i], YSIEVE[i], ZSIEVE );

    //Track direction 
    TVector3 TrackDirGlobal = TrackSievePos.Unit();

    //Now compute track intersection with plane of first GEM:
    //Since all tracks start at origin, 
    //Equation is: ( s * TrackDir - GEMPOS ) dot GEM_zaxis = 0
    // OR: s = (GEMPOS dot GEM_zaxis)/(TrackDir dot GEM_zaxis)
    
    double sintersect = GEMPOS.Dot( GEM_zaxis )/(TrackDirGlobal.Dot( GEM_zaxis ) );

    TVector3 TrackIntersect_FirstGEMplane = sintersect * TrackDirGlobal;

    double xfp_expect = (TrackIntersect_FirstGEMplane - GEMPOS).Dot( GEM_xaxis );
    double yfp_expect = (TrackIntersect_FirstGEMplane - GEMPOS).Dot( GEM_yaxis );
    double xpfp_expect = TrackDirGlobal.Dot( GEM_xaxis )/TrackDirGlobal.Dot( GEM_zaxis );
    double ypfp_expect = TrackDirGlobal.Dot( GEM_yaxis )/TrackDirGlobal.Dot( GEM_zaxis );

    chi2 += ( pow( xfp_expect - XTRACK[i], 2 ) + pow( yfp_expect - YTRACK[i], 2 ) ) / (sigma_pos*sigma_pos) +
      ( pow( xpfp_expect - XPTRACK[i], 2 ) + pow( ypfp_expect - YPTRACK[i], 2 ) ) / (sigma_slope*sigma_slope);
    
  }
  
  f = chi2;
}

void AlignZeroField( const char *configfilename ){

  TChain *C = new TChain("T");
  
  ifstream configfile(configfilename);

  TString prefix = "bb.gem";

  //These are in spectrometer transport coordinates for an assumed magnet distance of 1.85 m
  double GEMX0=-0.1637, GEMY0=0.0, GEMZ0=2.958;
  double ZSIEVE=1.85-13.37*2.54/100.0; // = 1.51 m
  double XOFFSIEVE = 0.0;
  double YOFFSIEVE = 0.0; 
  double GEMtheta = 10.0*PI/180.0; //this will become the pitch angle
  double GEMphi = 180.0*PI/180.0; 

  TCut globalcut = "";
  
  if( configfile ){
    TString currentline;
    
    while( currentline.ReadLine(configfile) && !currentline.BeginsWith("endlist") ){
      if( !currentline.BeginsWith("#") ){
	C->Add(currentline.Data());
      }
    }

    while( currentline.ReadLine(configfile) && !currentline.BeginsWith("endconfig")){
      if( !currentline.BeginsWith("#") ){
	TObjArray *tokens = currentline.Tokenize(" ");
	
	int ntokens = tokens->GetEntries();

	if( ntokens >= 2 ){
	  TString skey = ( (TObjString*) (*tokens)[0] )->GetString();

	  if( skey == "prefix" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    prefix = stemp;
	  }

	  if( skey == "GEMX0" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMX0 = stemp.Atof();
	  }
	  if( skey == "GEMY0" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMY0 = stemp.Atof();
	  }

	  if( skey == "GEMZ0" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMZ0 = stemp.Atof();
	  }

	  if( skey == "X0SIEVE" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    XOFFSIEVE = stemp.Atof();
	  }

	  if( skey == "Y0SIEVE" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    YOFFSIEVE = stemp.Atof();
	  }

	  if( skey == "ZSIEVE" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    ZSIEVE = stemp.Atof();
	  }

	  if( skey == "GEMtheta" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMtheta = stemp.Atof()*PI/180.0;
	  }

	  if( skey == "GEMphi" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMphi = stemp.Atof()*PI/180.0;
	  }
	  
	}
      }
    }

    while( currentline.ReadLine(configfile) && !currentline.BeginsWith("endcut") ){
      if( !currentline.BeginsWith("#") ){
	globalcut += currentline;
      }
    }
  } else {
    return;
  }

  TEventList *elist = new TEventList("elist");
  
  C->Draw(">>elist",globalcut);
  
  cout << "Number of events passing global cut = " << elist->GetN() << endl;

  //TODO: need to declare variables and set tree branch addresses

  UInt_t MAXNTRACKS = 10000;
  
  double ntracks, besttrack;

  //We'll use the GEM track variables directly rather than the BigBite ones to avoid confusion initially.
  //The GEM track variables are guaranteed to be given in the internal GEM coordinates:
  vector<double> tracknhits(MAXNTRACKS);
  vector<double> trackChi2NDF(MAXNTRACKS);
  vector<double> trackX(MAXNTRACKS);
  vector<double> trackY(MAXNTRACKS);
  vector<double> trackXp(MAXNTRACKS);
  vector<double> trackYp(MAXNTRACKS); 

  C->SetBranchStatus("*",0);

  TString branchname;
  
  C->SetBranchStatus( branchname.Format("%s.track.ntrack",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.chi2ndf",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.x",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.y",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.xp",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.yp",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.nhits",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.besttrack",prefix.Data() ), 1 );
  C->SetBranchStatus( "bb.ps.e", 1 );
  C->SetBranchStatus( "bb.sh.e", 1 );
  //main event loop here:
  C->SetBranchAddress( branchname.Format("%s.track.ntrack",prefix.Data() ), &ntracks );
  C->SetBranchAddress( branchname.Format("%s.track.chi2ndf",prefix.Data() ), &(trackChi2NDF[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.x",prefix.Data() ), &(trackX[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.y",prefix.Data() ), &(trackY[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.xp",prefix.Data() ), &(trackXp[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.yp",prefix.Data() ), &(trackYp[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.nhits",prefix.Data() ), &(tracknhits[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.besttrack",prefix.Data() ), &besttrack );
  
  TObjArray HList(0);
  TH2D *hxyfp = new TH2D("hxyfp"," ; y_{fp} (m) ; x_{fp} (m)",100,-0.3,0.3,250,-1.1,1.1); HList.Add(hxyfp);
  TH2D *hxysieve = new TH2D("hxysieve", " ; y_{sieve} (m) ; x_{sieve} (m)", 200,-0.2,0.2,200,-0.4,0.4); HList.Add(hxysieve);
  TH2F *hYpFpYFp = new TH2F("hYpFpYFp"," ; Ypfp ; Yfp",100,-.3,.3,100,-0.3,0.3); HList.Add(hYpFpYFp);
  TH2F *hXpFpXFp = new TH2F("hXpFpXFp"," ; Xpfp ; Xfp",100,-.7,.7,100,-0.7,0.7); HList.Add(hXpFpXFp);
  TH2D *hxyfp_sel = new TH2D("hxyfp_sel","with sieve cuts; y_{fp} (m) ; x_{fp} (m)",100,-0.3,0.3,250,-1.1,1.1); HList.Add(hxyfp);
  TH2D *hxysieve_sel = new TH2D("hxysieve_sel", "with sieve cuts; y_{sieve} (m) ; x_{sieve} (m)", 200,-0.4,0.4,200,-0.4,0.4); HList.Add(hxysieve);
  TH2F *hYpFpYFp_sel = new TH2F("hYpFpYFp_sel","with sieve cuts; Ypfp ; Yfp",100,-.3,.3,100,-0.3,0.3); HList.Add(hYpFpYFp);
  TH2F *hXpFpXFp_sel = new TH2F("hXpFpXFp_sel","with sieve cuts; Xpfp ; Xfp",100,-.7,.7,100,-0.7,0.7); HList.Add(hXpFpXFp);
  
  long nevent=0;

  TVector3 Global_yaxis(0,1,0);
  //Not clear if we will need these:
  TVector3 Global_xaxis(1,0,0);
  TVector3 Global_zaxis(0,0,1);

  TVector3 GEMzaxis( sin(GEMtheta)*cos(GEMphi), sin(GEMtheta)*sin(GEMphi), cos(GEMtheta) );
  TVector3 GEMxaxis = (Global_yaxis.Cross(GEMzaxis)).Unit();
  TVector3 GEMyaxis = (GEMzaxis.Cross(GEMxaxis)).Unit();
  TVector3 GEMorigin( GEMX0, GEMY0, GEMZ0 );
  TVector3 SieveOrigin(0,0,ZSIEVE);

  cout << endl << "GEM z axis in global coordinates:" << endl;
  GEMzaxis.Print();

  cout << endl << "GEM x axis in global coordinates:" << endl;
  GEMxaxis.Print();

  cout << endl << "GEM y axis in global coordinates:" << endl;
  GEMyaxis.Print();

  cout << endl << "GEM origin in global coordinates:" << endl;
  GEMorigin.Print();

  cout << endl << "Sieve slit origin in global coordinates: " << endl;
  SieveOrigin.Print();

  TString outCutFile;
  outCutFile=Form("sieve_cut.root");
  cout << "outCutFile =  " << outCutFile << endl;
  TFile fcut(outCutFile,"RECREATE");
  TString hname_cut;

  //Here are the desired sieve positions:
  vector <Double_t> xs_cent{-(0.3+0.0492)+0.0493/cos(18.*PI/180.),
      -(0.3+0.0492)+(0.0493+0.0492)/cos(18.*PI/180.),
      -(0.3+0.0492)+0.1493/cos(9.*PI/180.),
      -(0.3+0.0492)+(0.1493+0.0492)/cos(9.*PI/180.),
      -(0.3+0.0492)+(0.1493+0.0492*2.)/cos(9.*PI/180.),
      -0.0492,
      0.0,
      0.0492,
      0.3+0.0492-(0.1493+0.0492*2.)/cos(9.*PI/180.),
      0.3+0.0492-(0.1493+0.0492)/cos(9.*PI/180.),
      0.3+0.0492-0.1493/cos(9.*PI/180.),
      0.3+0.0492-(0.0493+0.0492)/cos(18.*3.14/180),
      0.3+0.0492-0.0493/cos(18.*PI/180.)};
   
  //Add in any XSIEVE offset defined by the user:
  for( int ix=0; ix<nxsieve; ix++ ){
    xs_cent[ix] += XOFFSIEVE;
  }

  vector <Double_t> ys_cent;
  for (Int_t nys=0;nys<nysieve;nys++) {
    Double_t pos=nys*0.0381-0.0381*3;//old sieve
    ys_cent.push_back(pos + YOFFSIEVE);
  }
  
  while( C->GetEntry( elist->GetEntry( nevent++ ) ) ){

    if( nevent % 1000 == 0 ) cout << "nevent = " << nevent << endl;
    
    //do stuff: grab track info from tree, apply cuts, fill the track arrays defined near the top of this macro (see XTRACK, YTRACK, XPTRACK, YPTRACK, XSIEVE, YSIEVE above)
    int itr = int(besttrack);
    if( besttrack >= 0 && ntracks <= MAXNTRACKS ){
      hxyfp->Fill( trackY[itr], trackX[itr] );
      hYpFpYFp->Fill(trackYp[itr],trackY[itr]);
      hXpFpXFp->Fill(trackXp[itr],trackX[itr]);
  
      TVector3 TrackPos_local( trackX[itr], trackY[itr], 0.0 );
      TVector3 TrackDir_local( trackXp[itr], trackYp[itr], 1.0 );
      TrackDir_local = TrackDir_local.Unit();
      
      TVector3 TrackDir_global = TrackDir_local.X() * GEMxaxis +
	TrackDir_local.Y() * GEMyaxis +
	TrackDir_local.Z() * GEMzaxis;

      TVector3 TrackPos_global = GEMorigin + TrackPos_local.X() * GEMxaxis + TrackPos_local.Y() * GEMyaxis + TrackPos_local.Z() * GEMzaxis; 
      
      //Now compute intersection of track ray in global coordinates with sieve slit:
      // (trackpos + s * trackdir - sieveorigin) dot globalzaxis = 0
      // --> s * trackdir dot globalzaxis = (sieveorigin - trackpos) dot globalzaxis
      double sintersect = (SieveOrigin - TrackPos_global).Dot( Global_zaxis ) /
	( TrackDir_global.Dot( Global_zaxis ) );
      TVector3 TrackSievePos = TrackPos_global + sintersect * TrackDir_global;

      hxysieve->Fill( TrackSievePos.Y(), TrackSievePos.X() );
      //cout<<"ysieve: "<<TrackSievePos.Y()<<" xsieve: "<<TrackSievePos.X()<<endl;
    }
  }//end while event

  //Set the sieve hole selection
  gROOT->Reset();
  TCanvas *histView_Cut; 
  histView_Cut= new TCanvas("histView_Cut","cut",1400,1000);
  histView_Cut->Divide(1,1);
  histView_Cut->cd(1);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetLogz(); 
  
  TCutG*t;
  histView_Cut->Clear();
  hxysieve->Draw("colz");
  hxysieve->SetMinimum(1.);	

  histView_Cut->Update();
  gSystem->ProcessEvents();
  histView_Cut->Update();
  //hxysieve->GetYaxis()->SetRangeUser(-1,1);
  //hxysieve->GetXaxis()->SetRangeUser(-1,1);
  //draw the ideal positions
  TLine* ys_line[nysieve];
  TText* ys_text[nysieve];
  TLine* xs_line[nxsieve];
  TText* xs_text[nxsieve];
  
  for (Int_t nys=0;nys<nysieve;nys++) {
    Double_t pos=ys_cent[nys];//for old nysieve=7	
    ys_line[nys]= new TLine(pos,-0.4,pos,0.4);
    ys_text[nys]= new TText(pos,-0.4,Form("%d",nys));
    ys_text[nys]->SetTextColor(2);
    ys_line[nys]->SetLineColor(2);
    ys_line[nys]->SetLineWidth(1);
    ys_line[nys]->Draw("same");
    ys_text[nys]->Draw("same");
  }
  for (Int_t nxs=0;nxs<nxsieve;nxs++) {
    Double_t pos=xs_cent[nxs];//for old nysieve=7	
    xs_line[nxs]= new TLine(-0.2,pos,0.2,pos);
    xs_text[nxs]= new TText(-0.2,pos,Form("%d",nxs));
    xs_text[nxs]->SetTextColor(2);
    xs_line[nxs]->SetLineColor(2);
    xs_line[nxs]->SetLineWidth(1);
    xs_line[nxs]->Draw("same");
    xs_text[nxs]->Draw("same");
  }
  
  histView_Cut->Update();
  gSystem->ProcessEvents();
    
  //in some kind of loop
  int nloop=0;
  while (nloop !=-1 ) {
    fcut.cd();
    for  (Int_t nys=0;nys<nysieve;nys++) {
      for (Int_t nxs=0; nxs<nxsieve;nxs++){
	hname_cut= Form("sievecut_%d_%d",nys,nxs);
	t=(TCutG*)gROOT->FindObject(hname_cut);
	if(t) {
	  fcut.cd();
	  t->Draw("same");
	  t->SetLineColor(1);
	  Double_t xcut,ycut;
	  t->GetPoint(0,xcut,ycut);
	  TText* ystext = new TText(xcut,ycut,Form("(%d,%d)",nys,nxs));
	  ystext->Draw();
	  histView_Cut->Update();
	  gSystem->ProcessEvents();
	}
      }
    }
  
    Int_t yscol=0;
    Int_t xscol=0;
    
    cout <<" Action ( 0 ( set cut) , -10 delete cut,  -100 exit, -1 done entering cuts) "  << endl;
    cin >> nloop ;
    if (nloop == -100) return;
    if (!(nloop == -10 || nloop==0 || nloop==-1 || nloop==-100)) continue;//return;
    if (nloop==-10 || nloop ==0) {
      cout << " Which xsieve hole ? " << " xscol = " << xscol << endl;
      cin >> xscol;
      cout << " Which ysieve hole ? " << " yscol = " << yscol << endl;
      cin >> yscol;
      if (yscol >=nysieve)yscol=0;
      if (xscol >=nxsieve)xscol=0;
      hname_cut= Form("sievecut_%d_%d",yscol,xscol);
    }
    if(nloop==-10) {
      fcut.cd();
      fcut.Delete(hname_cut+";*");
      fcut.Delete(hname_cut);
      t=(TCutG*)gROOT->FindObject(hname_cut);
      gROOT->Remove(t);
      t=(TCutG*)gROOT->FindObject(hname_cut);
      if (!t) cout << " delete cut = " <<hname_cut  << endl;
      if (t) cout << " delete cut? = " <<hname_cut  << endl;
      fcut.Write("",TObject::kOverwrite);
    }
    if (nloop==0) {
      t=(TCutG*)gROOT->FindObject(hname_cut);	      
      if (t) {
	fcut.cd();
	fcut.Delete(hname_cut);
	hname_cut= Form("sievecut_%d_%d",yscol,xscol);
	t=(TCutG*)gROOT->FindObject(hname_cut);
	gROOT->Remove(t);
      }
      gPad->Modified();
      gPad->Update();
      gSystem->ProcessEvents();
      
      TCutG*cutg=(TCutG*)gPad->WaitPrimitive("CUTG","CutG");
      histView_Cut->Update();
      gSystem->ProcessEvents();
      TCutG*tmpg= (TCutG*)gROOT->GetListOfSpecials()->FindObject("CUTG");
      hname_cut= Form("sievecut_%d_%d",yscol,xscol);
      TCutG*mycutg=(TCutG*)(tmpg->Clone(hname_cut));

      
      fcut.cd();
      mycutg->Write(0,TObject::kOverwrite);
    }
  } //end while for setting cuts

  fcut.cd();
  //  TFile hsimc(outCutFile,"recreate");
  HList.Write();

  cout<<"ready to load up the cuts now"<<endl;
  
  //load up the sieve hole cuts
  nevent = 0;
  vector<vector<TCutG*>> sieve_cut;
  vector<vector<bool> > cutexists;

  sieve_cut.resize(nysieve);
  cutexists.resize(nysieve);
  for (Int_t iy=0;iy<nysieve;iy++){
    sieve_cut[iy].resize(nxsieve);
    cutexists[iy].resize(nxsieve);
  }
  
  for (Int_t iy=0; iy<nysieve; iy++){
    for (Int_t ix=0;ix<nxsieve;ix++){
      TCutG* tempg = (TCutG*) fcut.Get(Form("sievecut_%d_%d",iy,ix));
      
      if (tempg)  {
	Int_t npt = tempg->GetN();
	cout<<"found one"<<endl;
	//sieve_cut[iy][ix].push_back(tempg);
	sieve_cut[iy][ix] = tempg;
	cutexists[iy][ix] = true;
      }
      else{
	cout<<"didn't find one"<<endl;
	//sieve_cut[iy][ix].push_back(tempg);
	sieve_cut[iy][ix] = NULL;
	cutexists[iy][ix] = false;
      }
    }
  }  

  while( C->GetEntry( elist->GetEntry( nevent++ ) ) ){
    //In this second loop, make explicit the association between sieve holes and tracks:

    int itr = int(besttrack);
    if( besttrack >= 0 && ntracks <= MAXNTRACKS ){
      
      TVector3 TrackPos_local( trackX[itr], trackY[itr], 0.0 );
      TVector3 TrackDir_local( trackXp[itr], trackYp[itr], 1.0 );
      TrackDir_local = TrackDir_local.Unit();
      
      TVector3 TrackDir_global = TrackDir_local.X() * GEMxaxis +
	TrackDir_local.Y() * GEMyaxis +
	TrackDir_local.Z() * GEMzaxis;

      TVector3 TrackPos_global = GEMorigin + TrackPos_local.X() * GEMxaxis + TrackPos_local.Y() * GEMyaxis + TrackPos_local.Z() * GEMzaxis; 
      
      //Now compute intersection of track ray in global coordinates with sieve slit:
      // (trackpos + s * trackdir - sieveorigin) dot globalzaxis = 0
      // --> s * trackdir dot globalzaxis = (sieveorigin - trackpos) dot globalzaxis
      double sintersect = (SieveOrigin - TrackPos_global).Dot( Global_zaxis ) /
	( TrackDir_global.Dot( Global_zaxis ) );
      TVector3 TrackSievePos = TrackPos_global + sintersect * TrackDir_global;

      int iy_found=-1;
      int ix_found=-1;
      
      for (Int_t iy=0; iy<nysieve; iy++){
	for (Int_t ix=0;ix<nxsieve;ix++){ 

	  if( cutexists[iy][ix] ){
	    if (sieve_cut[iy][ix]->IsInside(TrackSievePos.Y(),TrackSievePos.X())){
	      iy_found = iy; ix_found=ix;
	      hxysieve_sel->Fill( TrackSievePos.Y(), TrackSievePos.X() );

	    }
	  }
	}
      }
      //we have an event going through a hole
      if (iy_found != -1 && ix_found != -1){
	cout << "found a track going through a hole, (iy, ix)=(" << iy_found << ", " << ix_found
	     << ")" << endl;
	
	hxyfp_sel->Fill( trackY[itr], trackX[itr] );
	hYpFpYFp_sel->Fill(trackYp[itr],trackY[itr]);
	hXpFpXFp_sel->Fill(trackXp[itr],trackX[itr]);
	XTRACK.push_back(trackX[itr]);
	YTRACK.push_back(trackY[itr]);
	XPTRACK.push_back(trackXp[itr]);
	YPTRACK.push_back(trackYp[itr]);
	XSIEVE.push_back(xs_cent[ix_found]);
	YSIEVE.push_back(ys_cent[iy_found]);
	NTRACKS++;
      }
    }
  }


  //do the thing
  TMinuit *FitZeroField = new TMinuit( 6 );
  
  FitZeroField->SetFCN( CHI2_FCN );
  
  int ierflg = 0;
  
  FitZeroField->mnparm( 0, "GEMX0", GEMX0, 0.01,0,0,ierflg ); //guesstimate 1 cm as initial position accuracy
  FitZeroField->mnparm( 1, "GEMY0", GEMY0, 0.03,0,0,ierflg ); //start with 3-cm uncertainty for y position
  FitZeroField->mnparm( 2, "GEMZ0", GEMZ0, 0.01,0,0,ierflg );
  FitZeroField->mnparm( 3, "ZSIEVE", ZSIEVE, 0.01,0,0,ierflg );
  FitZeroField->mnparm( 4, "GEMtheta", GEMtheta, 0.3*PI/180.0, 0, 0, ierflg ); //guesstimate 0.3 degrees as initial angular accuracy
  FitZeroField->mnparm( 5, "GEMphi", GEMphi, 0.3*PI/180.0, 0, 0, ierflg );
    
  double arglist[10];
  arglist[0]=1;
  FitZeroField->mnexcm("SET ERR",arglist,1,ierflg);
  
  arglist[0] = 5000;
  arglist[1] = 1.;
  
  //Uncomment this line when we are actually set up to do a fit:   
  FitZeroField->mnexcm("MIGRAD",arglist,2,ierflg);
  
  //TODO: grab parameters, write them out to file. Profit.
  double fitpar[6];
  double fitparerr[6];
  for (int ii=0;ii<6;ii++){
    //fitpar[ii]=FitZeroField->GetParameter(ii);fitparerr[ii]=FitZeroField->GetParameter(ii);?????
  }
  
 
}//end program
