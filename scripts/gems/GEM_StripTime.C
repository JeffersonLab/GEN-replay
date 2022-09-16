//#include "GEM_cosmic_tracks.C"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TVector3.h"
#include "TRotation.h"
#include "TEventList.h"
#include "TCut.h"
#include <iostream>
#include <fstream>
#include "TMinuit.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TProfile.h"
#include "TMatrixD.h"
#include "TVectorD.h"

double PI = TMath::Pi();

double PulseFunc( double *x, double *par ){
  double t = x[0];
  double A = par[0];
  double t0 = par[1];
  double tau = par[2];
  double Baseline = par[3];
  
  //When t = t0 + tau, f(t) = A * exp(-1)

  if( t < t0 ){
    return Baseline;
  } else {
    return Baseline + A * (t-t0)/tau * exp(-(t-t0)/tau );
  }
}
//The question here is should we do this by module, by APV card? How granular do we want to be?

void GEM_StripTime( const char *configfilename, const char *outfilename="GEM_StripTime_temp.root" ){
  cout << "attempting to open config file " << configfilename << endl;
  
  ifstream configfile(configfilename);

  TF1 *PulseShapeFit = new TF1( "PulseShapeFit", PulseFunc, -200, 200, 4 );
  double startpar[4] = {exp(-1.0), 25.0, 56.0, 0.0 };
  PulseShapeFit->SetParameters(startpar);

  // PulseShapeFit->SetParLimits( 2, 0.0, 1000.0 );
  // PulseShapeFit->SetParLimits( 0, 0.0, 3000.0 );
  
  TCut globalcut = "";

  int nmodules;

  TString prefix = "bb.gem";
  
  TChain *C = new TChain("T");

  cout << "Parsing config file..." << endl;

  int fixtau = 0; //fix time constant in pulse fit?
  double tau0 = 56.0; // ns

  double ttrig0 = 0.0;
  double ttrig_slope = 0.32;
  
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

	  if ( skey == "nmodules" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    nmodules = stemp.Atoi();
	  }

	  if( skey == "prefix" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    prefix = stemp;
	  }

	  if( skey == "fixtau" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    fixtau = stemp.Atoi();
	  }

	  if( skey == "tau0" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    tau0 = stemp.Atof();
	  }

	  if( skey == "ttrig0" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    ttrig0 = stemp.Atof();
	  }

	   if( skey == "ttrig_slope" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    ttrig_slope = stemp.Atof();
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

  
  
  cout << "Creating event list..." << endl;
  
  TEventList *elist = new TEventList("elist");

  C->Draw(">>elist",globalcut);

  cout << "Number of events passing global cut = " << elist->GetN() << endl;

  double ntracks;
  double besttrack;
  double tracknhits[10000];
  double trackX[10000],trackY[10000], trackXp[10000], trackYp[10000], trackChi2NDF[10000];
  
  //Needed "hit" variables (others can be ignored for now:
  //The data types for all the branches are "double". Hope that doesn't cause problems: 
  double ngoodhits;
  double hit_trackindex[10000];
  double hit_module[10000];
  double hit_ulocal[10000];
  double hit_vlocal[10000];
  double hit_UtimeMaxStrip[10000];
  double hit_VtimeMaxStrip[10000];
  double hit_deltat[10000];
  double hit_ADCmaxsampU[10000];
  double hit_ADCmaxsampV[10000];
  double hit_ADCmaxstripU[10000];
  double hit_ADCmaxstripV[10000];
  // double hit_ADCmaxsampU[10000];
  // double hit_ADCmaxsampV[10000];
  double hit_ADCfrac0_Umax[10000];
  double hit_ADCfrac1_Umax[10000];
  double hit_ADCfrac2_Umax[10000];
  double hit_ADCfrac3_Umax[10000];
  double hit_ADCfrac4_Umax[10000];
  double hit_ADCfrac5_Umax[10000];

  double hit_ADCfrac0_Vmax[10000];
  double hit_ADCfrac1_Vmax[10000];
  double hit_ADCfrac2_Vmax[10000];
  double hit_ADCfrac3_Vmax[10000];
  double hit_ADCfrac4_Vmax[10000];
  double hit_ADCfrac5_Vmax[10000];

  double hit_nstripu[10000];
  double hit_nstripv[10000];
  
  double bb_hodotdc_nclus;
  double bb_hodotdc_clus_tmean[10000];
  double bb_hodotdc_clus_trackindex[10000];
  double bb_tdctrig_tdcelemID[10000];
  double bb_tdctrig_tdc[10000];

  
  
  TString branchname;

  C->SetBranchStatus("*",0);

  C->SetBranchStatus( branchname.Format( "%s.track.ntrack", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.track.besttrack", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.track.nhits", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ngoodhits", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.trackindex", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.track.x", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.track.y", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.track.xp", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.track.yp", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.track.chi2ndf", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.module", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.u", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.v", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.nstripu", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.nstripv", prefix.Data() ), 1 );
  
  C->SetBranchStatus( branchname.Format( "%s.hit.deltat", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCmaxstripU", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCmaxstripV", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCmaxsampU", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCmaxsampV", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.UtimeMaxStrip", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.VtimeMaxStrip", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac0_Umax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac1_Umax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac2_Umax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac3_Umax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac4_Umax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac5_Umax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac0_Vmax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac1_Vmax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac2_Vmax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac3_Vmax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac4_Vmax", prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format( "%s.hit.ADCfrac5_Vmax", prefix.Data() ), 1 );
  //Hodoscope variables (probably shouldn't be hard-coded, I know):
  C->SetBranchStatus("bb.hodotdc.nclus", 1 );
  C->SetBranchStatus("bb.hodotdc.clus.tmean", 1 );
  C->SetBranchStatus("bb.hodotdc.clus.trackindex", 1 );
  //Trigger TDC variables:
  C->SetBranchStatus("Ndata.bb.tdctrig.tdcelemID", 1);
  C->SetBranchStatus("bb.tdctrig.tdcelemID", 1 );
  C->SetBranchStatus("bb.tdctrig.tdc", 1 );
  
  //C->SetBranchStatus( branchname.Format( "%s.track.ntrack", prefix.Data() ), 1 );
  
  //This SHOULD give us everything we need from the ROOT tree:
  C->SetBranchAddress( branchname.Format( "%s.track.ntrack", prefix.Data() ), &ntracks );
  C->SetBranchAddress( branchname.Format( "%s.track.besttrack", prefix.Data() ), &besttrack );
  C->SetBranchAddress( branchname.Format( "%s.track.nhits", prefix.Data() ), tracknhits );
  C->SetBranchAddress( branchname.Format( "%s.hit.ngoodhits", prefix.Data() ), &ngoodhits );
  C->SetBranchAddress( branchname.Format( "%s.hit.trackindex", prefix.Data() ), hit_trackindex );
  C->SetBranchAddress( branchname.Format( "%s.track.x", prefix.Data() ), trackX );
  C->SetBranchAddress( branchname.Format( "%s.track.y", prefix.Data() ), trackY );
  C->SetBranchAddress( branchname.Format( "%s.track.xp", prefix.Data() ), trackXp );
  C->SetBranchAddress( branchname.Format( "%s.track.yp", prefix.Data() ), trackYp );
  C->SetBranchAddress( branchname.Format( "%s.track.chi2ndf", prefix.Data() ), trackChi2NDF );
  C->SetBranchAddress( branchname.Format( "%s.hit.module", prefix.Data() ), hit_module );
  C->SetBranchAddress( branchname.Format( "%s.hit.u", prefix.Data() ), hit_ulocal );
  C->SetBranchAddress( branchname.Format( "%s.hit.v", prefix.Data() ), hit_vlocal );
  C->SetBranchAddress( branchname.Format( "%s.hit.nstripu", prefix.Data() ), hit_nstripu );
  C->SetBranchAddress( branchname.Format( "%s.hit.nstripv", prefix.Data() ), hit_nstripv );
  C->SetBranchAddress( branchname.Format( "%s.hit.deltat", prefix.Data() ), hit_deltat );
  C->SetBranchAddress( branchname.Format( "%s.hit.UtimeMaxStrip", prefix.Data() ), hit_UtimeMaxStrip );
  C->SetBranchAddress( branchname.Format( "%s.hit.VtimeMaxStrip", prefix.Data() ), hit_VtimeMaxStrip );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCmaxstripU", prefix.Data() ), hit_ADCmaxstripU );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCmaxstripV", prefix.Data() ), hit_ADCmaxstripV );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCmaxsampU", prefix.Data() ), hit_ADCmaxsampU );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCmaxsampV", prefix.Data() ), hit_ADCmaxsampV );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac0_Umax", prefix.Data() ), hit_ADCfrac0_Umax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac1_Umax", prefix.Data() ), hit_ADCfrac1_Umax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac2_Umax", prefix.Data() ), hit_ADCfrac2_Umax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac3_Umax", prefix.Data() ), hit_ADCfrac3_Umax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac4_Umax", prefix.Data() ), hit_ADCfrac4_Umax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac5_Umax", prefix.Data() ), hit_ADCfrac5_Umax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac0_Vmax", prefix.Data() ), hit_ADCfrac0_Vmax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac1_Vmax", prefix.Data() ), hit_ADCfrac1_Vmax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac2_Vmax", prefix.Data() ), hit_ADCfrac2_Vmax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac3_Vmax", prefix.Data() ), hit_ADCfrac3_Vmax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac4_Vmax", prefix.Data() ), hit_ADCfrac4_Vmax );
  C->SetBranchAddress( branchname.Format( "%s.hit.ADCfrac5_Vmax", prefix.Data() ), hit_ADCfrac5_Vmax );

  C->SetBranchAddress("bb.hodotdc.nclus", &bb_hodotdc_nclus );
  C->SetBranchAddress("bb.hodotdc.clus.tmean", bb_hodotdc_clus_tmean );
  C->SetBranchAddress("bb.hodotdc.clus.trackindex", bb_hodotdc_clus_trackindex );

  int nhits_tdctrig;
  C->SetBranchAddress("Ndata.bb.tdctrig.tdcelemID", &nhits_tdctrig );
  C->SetBranchAddress("bb.tdctrig.tdcelemID", bb_tdctrig_tdcelemID );
  C->SetBranchAddress("bb.tdctrig.tdc", bb_tdctrig_tdc );

  TFile *fout = new TFile(outfilename, "RECREATE" );
  
  TH1D *hA_U = new TH1D( "hA_U","U Amplitude",200,0.0,4000.0);
  TH1D *hB_U = new TH1D( "hB_U","U baseline", 200, -500, 500 );
  TH1D *ht0_U = new TH1D( "ht0_U", "U Start time", 300,-100,100);
  TH1D *htau_U = new TH1D( "htau_U", "U time constant #tau (ns)", 250, 0.0, 300.0);
  TH1D *htdiff_hodo_U = new TH1D( "htdiff_hodo_U", "U tGEM - tHODO (ns)", 300, -100, 100 );
  TH1D *htdiff_trig_U = new TH1D( "htdiff_trig_U", "U tGEM - ttrig (ns)", 300, -100, 100 );
  TH1D *htdiff_fit_mean_U = new TH1D( "htdiff_fit_mean_U", "U tGEM(fit)-tGEM(wavg) (ns)", 300, -100,100);
  TH2D *htGEM_vs_thodo_U = new TH2D( "htGEM_vs_thodo_U", "U strips;thodo (ns); tGEM (ns)", 150, -20,20,150,-100,100);
  TH2D *htGEM_vs_ttrig_U = new TH2D( "htGEM_vs_ttrig_U", "U strips;ttrig (ns); tGEM (ns)", 150, -30,30,150,-100,100);
  TH2D *htdiff_hodo_vs_A_U = new TH2D( "htdiff_hodo_vs_A_U", "U strips; Amplitude; tGEM - thodo (ns)", 150,0,4000,150,-100,100);
  TH2D *htdiff_fit_mean_vs_A_U = new TH2D( "htdiff_fit_mean_vs_A_U", "U strips; Amplitude; tGEM(fit - wavg) (ns)", 150,0,4000,150,-100,100);
  TH1D *hchi2dof_U = new TH1D("hchi2dof_U", "Fit chi2/dof, U strips", 200, 0.0, 50.0 );
  TH2D *htau_vs_A_U = new TH2D("htau_vs_A_U", "U strips ;Amplitude; #tau (ns)", 150,0,4000,150,0,300);

  TProfile *htGEM_vs_ttrig_U_prof = new TProfile("htGEM_vs_ttrig_U_prof", "U strips ;t_{trig} (ns) - 368;t_{GEM} (ns)", 100,-30,30);
  TH1D *htdiff_mean_hodo_U = new TH1D("htdiff_mean_hodo_U",";t(wavg)-thodo (ns);", 200,-100,100);
  TH2D *htdiff_mean_vs_hodo_U = new TH2D("htdiff_mean_vs_hodo_U",";thodo (ns);t(wavg) (ns)", 150,-30,30,150,-100,100);

  TH1D *htdiff_mean_trig_U = new TH1D("htdiff_mean_trig_U",";t(wavg)-ttrig (ns);", 200,-100,100);
  TH2D *htdiff_mean_vs_trig_U = new TH2D("htdiff_mean_vs_trig_U",";ttrig (ns);t(wavg) (ns)", 150,-30,30,150,-100,100);

  TH1D *htdiff_trig_corr_U = new TH1D("htdiff_trig_corr_U",";t0-ttrig_corr (ns);",200,-100,100);
  
  
  TH1D *hA_V = new TH1D( "hA_V","V Amplitude",200,0.0,4000.0);
  TH1D *hB_V = new TH1D( "hB_V","V baseline", 200, -500, 500 );
  TH1D *ht0_V = new TH1D( "ht0_V", "V Start time", 300,-100,100);
  TH1D *htau_V = new TH1D( "htau_V", "V time constant #tau (ns)", 250, 0.0, 300.0);
  TH1D *htdiff_hodo_V = new TH1D( "htdiff_hodo_V", "V tGEM - tHODO (ns)", 300, -100, 100 );
  TH1D *htdiff_trig_V = new TH1D( "htdiff_trig_V", "V tGEM - ttrig (ns)", 300, -100, 100 );
  TH1D *htdiff_fit_mean_V = new TH1D( "htdiff_fit_mean_V", "V tGEM(fit)-tGEM(wavg) (ns)", 300, -100,100);
  TH2D *htGEM_vs_thodo_V = new TH2D( "htGEM_vs_thodo_V", "V strips;thodo (ns); tGEM (ns)", 150, -20,20,150,-100,100);
  TH2D *htGEM_vs_ttrig_V = new TH2D( "htGEM_vs_ttrig_V", "V strips;ttrig (ns); tGEM (ns)", 150, -30,30,150,-100,100);
  TH2D *htdiff_hodo_vs_A_V = new TH2D( "htdiff_hodo_vs_A_V", "V strips; Amplitude; tGEM - thodo (ns)", 150,0,4000,150,-100,100);
  TH2D *htdiff_fit_mean_vs_A_V = new TH2D( "htdiff_fit_mean_vs_A_V", "V strips; Amplitude; tGEM(fit - wavg) (ns)", 150,0,4000,150,-100,100);
  TH1D *hchi2dof_V = new TH1D("hchi2dof_V", "Fit chi2/dof, V strips", 200, 0.0, 50.0 );
  TH2D *htau_vs_A_V = new TH2D("htau_vs_A_V", "V strips ;Amplitude; #tau (ns)", 150,0,4000,150,0,300);

  TProfile *htGEM_vs_ttrig_V_prof = new TProfile("htGEM_vs_ttrig_V_prof", "V strips ;t_{trig} (ns) - 368;t_{GEM} (ns)", 100,-30,30);
  TH1D *htdiff_mean_hodo_V = new TH1D("htdiff_mean_hodo_V",";t(wavg)-thodo (ns);", 200,-100,100);
  TH2D *htdiff_mean_vs_hodo_V = new TH2D("htdiff_mean_vs_hodo_V",";thodo (ns);t(wavg) (ns)", 150,-30,30,150,-100,100);

  TH1D *htdiff_mean_trig_V = new TH1D("htdiff_mean_trig_V",";t(wavg)-ttrig (ns);", 200,-100,100);
  TH2D *htdiff_mean_vs_trig_V = new TH2D("htdiff_mean_vs_trig_V",";ttrig (ns);t(wavg) (ns)", 150,-30,30,150,-100,100);

  TH1D *htdiff_trig_corr_V = new TH1D("htdiff_trig_corr_V",";t0-ttrig_corr (ns);",200,-100,100);

  //////////
  
  TH1D *htdiff_UVmean = new TH1D("htdiff_UVmean","tUmean-tVmean (ns)", 250, -50, 50 );
  TH2D *htdiff_UvsVmean = new TH2D("htdiff_UvsVmean",";tVmean (ns);tUmean (ns)", 150,-100,100,150,-100,100);

  TH1D *htdiff_UV = new TH1D("htdiff_UV", "t_{U}-t_{V} (ns)", 250,-50,50);
  TH2D *htUvsV = new TH2D("htUvsV", ";t_{V} (ns);t_{U} (ns)", 250,-100,100, 250,-100,100);
  
  TH2D *hADCsampUdeconv = new TH2D("hADCsampUdeconv","Deconv. ADC U samples",6,-0.5,5.5,250,-1000,2000);
  TH2D *hADCsampVdeconv = new TH2D("hADCsampVdeconv","Deconv. ADC V samples",6,-0.5,5.5,250,-1000,2000);

  TH2D *hADCfracUdeconv = new TH2D("hADCfracUdeconv","Deconv. ADC U samples (fractional)",6,-0.5,5.5,250,-1.5,1.5);
  TH2D *hADCfracVdeconv = new TH2D("hADCfracVdeconv","Deconv. ADC V samples (fractional)",6,-0.5,5.5,250,-1.5,1.5);

  TH2D *hADCfracU = new TH2D("hADCfracU","ADC U samples (fractional)",6,0.0,6.*24.,250,-1.5,1.5);
  TH2D *hADCfracV = new TH2D("hADCfracV","ADC V samples (fractional)",6,0.0,6.*24.,250,-1.5,1.5);

  TH1D *hiSampMaxDeconvU = new TH1D("hiSampMaxDeconvU", "max deconvoluted time sample", 6, -0.5, 5.5 );
  TH1D *hiSampMaxDeconvV = new TH1D("hiSampMaxDeconvV", "max deconvoluted time sample", 6, -0.5, 5.5 );

  TH1D *hADCmaxDeconvU = new TH1D("hADCmaxDeconvU", "maximum deconvoluted U ADC sample", 150, 0, 2000 );
  TH1D *hADCmaxDeconvV = new TH1D("hADCmaxDeconvV", "maximum deconvoluted V ADC sample", 150, 0, 2000 );

  TH1D *hADCsumDeconvU = new TH1D("hADCsumDeconvU", "sum of deconvoluted U ADC samples", 250, -500, 3000 );
  TH1D *hADCsumDeconvV = new TH1D("hADCsumDeconvV", "sum of deconvoluted V ADC samples", 250, -500, 3000 );
  
  TH1D *hADCmaxFracDeconvU = new TH1D("hADCmaxFracDeconvU", "maximum deconvoluted U ADC sample (fractional)", 150, -2, 2 );
  TH1D *hADCmaxFracDeconvV = new TH1D("hADCmaxFracDeconvV", "maximum deconvoluted V ADC sample (fractional)", 150, -2, 2 );

  TH1D *hADCmaxDeconvRatioU = new TH1D("hADCmaxDeconvRatioU", "Ratio max. deconvoluted ADC sample/max. ADC sample, U", 150, -1, 5);
  TH1D *hADCmaxDeconvRatioV = new TH1D("hADCmaxDeconvRatioV", "Ratio max. deconvoluted ADC sample/max. ADC sample, V", 150, -1, 5);
  
  TH2D *hADCmaxDeconvVSmaxSampU = new TH2D("hADCmaxDeconvVSmaxSampU", "U strips;max ADC sample;max deconvoluted sample", 150,-200,3000,150,-500,2000);
  TH2D *hADCmaxDeconvVSmaxSampV = new TH2D("hADCmaxDeconvVSmaxSampV", "V strips;max ADC sample;max deconvoluted sample", 150,-200,3000,150,-500,2000);

  TH2D *hUtimeMaxStrip_vs_module = new TH2D("hUtimeMaxStrip_vs_module",";module; Max U strip time (ns)",nmodules, -0.5,nmodules-0.5, 150,0,150);

  TH2D *hVtimeMaxStrip_vs_module = new TH2D("hVtimeMaxStrip_vs_module",";module; Max V strip time (ns)",nmodules, -0.5,nmodules-0.5, 150,0,150);


  TH1D *hADCmaxComboDeconvU = new TH1D("hADCmaxComboDeconvU", "U strips; max combination of two deconvoluted ADC samples", 300, 0, 6000);
  TH1D *hIsampMaxComboDeconvU = new TH1D("hIsampMaxComboDeconvU", "U strips; first sample of max two-sample pair", 7, -0.5, 6.5 );

  TH1D *hADCmaxComboDeconvV = new TH1D("hADCmaxComboDeconvV", "V strips; max combination of two deconvoluted ADC samples", 300, 0, 6000);
  TH1D *hIsampMaxComboDeconvV = new TH1D("hIsampMaxComboDeconvV", "V strips; first sample of max two-sample pair", 7, -0.5, 6.5 );
  
  TH1D *hADCfracMaxComboU = new TH1D("hADCfracMaxComboU", "U strip; max deconv two-sample combo/max ADC sample", 300, -0.5,2.5);
  TH1D *hADCfracMaxComboV = new TH1D("hADCfracMaxComboV", "V strip; max deconv two-sample combo/max ADC sample", 300, -0.5,2.5); 

  TH1D *hTdeconvU = new TH1D("hTdeconvU", "U strips; Deconvoluted strip time (ns)", 300, -50, 250.0);
  TH1D *hTdeconvV = new TH1D("hTdeconvV", "V strips; Deconvoluted strip time (ns)", 300, -50, 250.0);

  TH1D *hTdeconvUVdiff = new TH1D("hTdeconvUVdiff", "Deconv. strip time; t_{U}-t_{V} (ns)", 200,-50,50);

  TH2D *hTdeconvVvsU = new TH2D("hTdeconvVvsU", ";Deconv. U strip time (ns);Deconv. V strip time (ns)",
				150,-50,250,150,-50,250 );
  
  long nevent=0;

  double ADCsampU[6]={0,0,0,0,0,0},ADCsampV[6]={0,0,0,0,0,0},dADCsampU[6]={20,20,20,20,20,20},dADCsampV[6]={20,20,20,20,20,20};
  double tsamp[6] = {12.0, 36.0, 60.0, 84.0, 108.0, 132.0 };
  double dtsamp[6] = {0,0,0,0,0,0};

  double ADCsampUdeconv[6], ADCsampVdeconv[6];

  double tdeconv;
  
  //It is possible we should make ONE TGraph and just update the data to avoid the overhead
  // of all these new... delete... calls, but for now I'm lazy. 
  TGraphErrors *gPulseU = new TGraphErrors( 6, tsamp, ADCsampU, dtsamp, dADCsampU );
  TGraphErrors *gPulseV = new TGraphErrors( 6, tsamp, ADCsampV, dtsamp, dADCsampV );
  
  while( C->GetEntry( elist->GetEntry(nevent++) ) ){
    if( nevent % 1000 == 0 ) cout << nevent << endl;
    
    double ttrig, thodo;

    TMatrixD Hdeconv(6,6);
    
    TVectorD Hvect(6);

    double t0deconv = 0.0;
    
    if( ngoodhits < 10000 && int(ntracks) == 1 && int(bb_hodotdc_nclus) == 1 && int(nhits_tdctrig) >= 1 ){
      //first get trigger time:
      for( int ihit=0; ihit<nhits_tdctrig; ihit++ ){
	if( int(bb_tdctrig_tdcelemID[ihit]) == 5 ){
	  ttrig = bb_tdctrig_tdc[ihit] - 368.;

	  //Once we know ttrig, we can 

	  //t0deconv = ttrig0 + ttrig_slope * ttrig;

	  // cout << "t0 for deconvolution = " << t0deconv << endl;
	  
	}
      }

      for( int isamp=0; isamp<6; isamp++ ){
	if( tsamp[isamp]>t0deconv ) {
	  Hvect(isamp) = (tsamp[isamp]-t0deconv)/tau0 * exp(-(tsamp[isamp]-t0deconv)/tau0 +1.0); //h(tau) = e^(-1+1 = 0) = 1
	} else {
	  Hvect(isamp) = 0.0;
	}
	
	for( int jsamp=0; jsamp<=isamp; jsamp++ ){
	  Hdeconv(isamp,jsamp) = Hvect(isamp-jsamp);
	}
      }
      // cout << "before inversion, Hdeconv = " << endl;
      // Hdeconv.Print();
      
      // Hdeconv.Invert();

      // cout << "after inversion, Hdeconv = " << endl;
      // Hdeconv.Print();
      
      //Next get hodoscope mean time:
      for( int ihit=0; ihit<bb_hodotdc_nclus; ihit++ ){
	if( int(bb_hodotdc_clus_trackindex[ihit]) == 0 ){
	  thodo = bb_hodotdc_clus_tmean[ihit];
	}
      }
      
      //loop over all GEM hits on good tracks:
      for( int ihit=0; ihit<int(ngoodhits); ihit++ ){
	if( hit_trackindex[ihit] == 0 && hit_nstripu[ihit] > 1 && hit_nstripv[ihit] > 1){
	  ADCsampU[0] = hit_ADCmaxstripU[ihit] * hit_ADCfrac0_Umax[ihit];
	  ADCsampU[1] = hit_ADCmaxstripU[ihit] * hit_ADCfrac1_Umax[ihit];
	  ADCsampU[2] = hit_ADCmaxstripU[ihit] * hit_ADCfrac2_Umax[ihit];
	  ADCsampU[3] = hit_ADCmaxstripU[ihit] * hit_ADCfrac3_Umax[ihit];
	  ADCsampU[4] = hit_ADCmaxstripU[ihit] * hit_ADCfrac4_Umax[ihit];
	  ADCsampU[5] = hit_ADCmaxstripU[ihit] * hit_ADCfrac5_Umax[ihit];

	  
	  ADCsampV[0] = hit_ADCmaxstripV[ihit] * hit_ADCfrac0_Vmax[ihit];
	  ADCsampV[1] = hit_ADCmaxstripV[ihit] * hit_ADCfrac1_Vmax[ihit];
	  ADCsampV[2] = hit_ADCmaxstripV[ihit] * hit_ADCfrac2_Vmax[ihit];
	  ADCsampV[3] = hit_ADCmaxstripV[ihit] * hit_ADCfrac3_Vmax[ihit];
	  ADCsampV[4] = hit_ADCmaxstripV[ihit] * hit_ADCfrac4_Vmax[ihit];
	  ADCsampV[5] = hit_ADCmaxstripV[ihit] * hit_ADCfrac5_Vmax[ihit];

	  double ADCsumUdeconv = 0.0;
	  double ADCsumVdeconv = 0.0;

	  int isampmaxdeconvU=-1;
	  int isampmaxdeconvV=-1;
	  
	  double maxdeconvU = 0.0;
	  double maxdeconvV = 0.0;
	  
	  for( int isamp=0; isamp<6; isamp++ ){

	    gPulseU->SetPointY(isamp,ADCsampU[isamp]);
	    gPulseV->SetPointY(isamp,ADCsampV[isamp]);

	    hADCfracU->Fill( tsamp[isamp], ADCsampU[isamp]/hit_ADCmaxstripU[ihit] );
	    hADCfracV->Fill( tsamp[isamp], ADCsampV[isamp]/hit_ADCmaxstripV[ihit] );
	    
	    ADCsampUdeconv[isamp] = 0.0;
	    ADCsampVdeconv[isamp] = 0.0;

	    double w1, w2=0, w3=0;
	    double x = 24.0/tau0;
	    w1 = exp( x - 1.0 )/x;
	    w2 = -2.*exp(-1.0)/x;
	    w3 = exp(-1.0-x)/x;
	    
	    // for( int jsamp=0; jsamp<6; jsamp++ ){
	    //   ADCsampUdeconv[isamp] += Hdeconv(isamp,jsamp)*ADCsampU[jsamp];
	    //   ADCsampVdeconv[isamp] += Hdeconv(isamp,jsamp)*ADCsampV[jsamp];
	    // }

	    if( isamp == 0 ){
	      ADCsampUdeconv[isamp] = w1 * ADCsampU[isamp];
	      ADCsampVdeconv[isamp] = w1 * ADCsampV[isamp];
	    } else if( isamp == 1 ){
	      ADCsampUdeconv[isamp] = w1 * ADCsampU[isamp] + w2 * ADCsampU[isamp-1];
	      ADCsampVdeconv[isamp] = w1 * ADCsampV[isamp] + w2 * ADCsampV[isamp-1];
	    } else {
	      ADCsampUdeconv[isamp] = w1 * ADCsampU[isamp] + w2 * ADCsampU[isamp-1] + w3 * ADCsampU[isamp-2];
	      ADCsampVdeconv[isamp] = w1 * ADCsampV[isamp] + w2 * ADCsampV[isamp-1] + w3 * ADCsampV[isamp-2];
	    }
	    
	    ADCsumUdeconv += ADCsampUdeconv[isamp];
	    ADCsumVdeconv += ADCsampVdeconv[isamp];
	    
	    hADCsampUdeconv->Fill(isamp,ADCsampUdeconv[isamp]);
	    hADCsampVdeconv->Fill(isamp,ADCsampVdeconv[isamp]);

	    
	    
	    if( isamp == 0 || ADCsampUdeconv[isamp] > maxdeconvU ){
	      maxdeconvU = ADCsampUdeconv[isamp]; 
	      isampmaxdeconvU = isamp;
	    }

	    if( isamp == 0 || ADCsampVdeconv[isamp] > maxdeconvV ){
	      maxdeconvV = ADCsampVdeconv[isamp]; 
	      isampmaxdeconvV = isamp;
	    }

		
	  }

	  int isampmaxcomboU=-1;
	  int isampmaxcomboV=-1;
	  
	  double maxcomboU=0.0;
	  double maxcomboV=0.0;

	  double tdeconvU = 0.0;
	  double tdeconvV = 0.0;
	  
	  for( int isamp=0; isamp<6; isamp++ ){
	    double comboU = ADCsampUdeconv[isamp];
	    double comboV = ADCsampVdeconv[isamp];
	    
	    if( isamp<5 ){
	      comboU += ADCsampUdeconv[isamp+1];
	      comboV += ADCsampVdeconv[isamp+1];
	    }

	    if( isampmaxcomboU < 0 || comboU > maxcomboU ){
	      isampmaxcomboU = isamp;
	      maxcomboU = comboU;
	    }

	    if( isampmaxcomboV < 0 || comboV > maxcomboV ){
	      isampmaxcomboV = isamp;
	      maxcomboV = comboV;
	    }
	    
	    hADCfracUdeconv->Fill( isamp, ADCsampUdeconv[isamp]/ADCsumUdeconv );
	    hADCfracVdeconv->Fill( isamp, ADCsampVdeconv[isamp]/ADCsumVdeconv );

	    tdeconvU += ADCsampUdeconv[isamp]*tsamp[isamp]/ADCsumUdeconv;
	    tdeconvV += ADCsampVdeconv[isamp]*tsamp[isamp]/ADCsumVdeconv;
	  }

	  // if( isampmaxcomboU < 5 ){
	  //   double t1 = tsamp[isampmaxcomboU];
	  //   double t2 = tsamp[isampmaxcomboU+1];
	  //   double ADC1 = ADCsampUdeconv[isampmaxcomboU];
	  //   double ADC2 = ADCsampUdeconv[isampmaxcomboU+1];

	  //   tdeconvU = (t1 * ADC1 + t2 * ADC2)/maxcomboU;
	  // } else {
	  //   tdeconvU = tsamp[5];
	  // }

	  // if( isampmaxcomboV < 5 ){
	  //   double t1 = tsamp[isampmaxcomboV];
	  //   double t2 = tsamp[isampmaxcomboV+1];
	  //   double ADC1 = ADCsampVdeconv[isampmaxcomboV];
	  //   double ADC2 = ADCsampVdeconv[isampmaxcomboV+1];

	  //   tdeconvV = (t1 * ADC1 + t2 * ADC2)/maxcomboV;
	  // } else {
	  //   tdeconvV = tsamp[5];
	  // }
	  

	  hTdeconvU->Fill( tdeconvU );
	  hTdeconvV->Fill( tdeconvV );
	  hTdeconvUVdiff->Fill( tdeconvU-tdeconvV );
	  hTdeconvVvsU->Fill( tdeconvU, tdeconvV );
	  
	  hADCmaxComboDeconvU->Fill( maxcomboU );
	  hADCmaxComboDeconvV->Fill( maxcomboV );

	  hIsampMaxComboDeconvU->Fill( isampmaxcomboU );
	  hIsampMaxComboDeconvV->Fill( isampmaxcomboV );

	  hADCfracMaxComboU->Fill( maxcomboU/hit_ADCmaxsampU[ihit] );
	  hADCfracMaxComboV->Fill( maxcomboV/hit_ADCmaxsampV[ihit] );
	  
	  hADCsumDeconvU->Fill( ADCsumUdeconv );
	  hADCsumDeconvV->Fill( ADCsumVdeconv );
	  
	  hiSampMaxDeconvU->Fill( isampmaxdeconvU );
	  hiSampMaxDeconvV->Fill( isampmaxdeconvV );
	  hADCmaxDeconvU->Fill( maxdeconvU );
	  hADCmaxDeconvV->Fill( maxdeconvV );
	  hADCmaxFracDeconvU->Fill( maxdeconvU/ADCsumUdeconv );
	  hADCmaxFracDeconvV->Fill( maxdeconvV/ADCsumVdeconv );
	  
	  hADCmaxDeconvVSmaxSampU->Fill( hit_ADCmaxsampU[ihit], maxdeconvU );
	  hADCmaxDeconvVSmaxSampV->Fill( hit_ADCmaxsampV[ihit], maxdeconvV );

	  hADCmaxDeconvRatioU->Fill( maxdeconvU/hit_ADCmaxsampU[ihit] );
	  hADCmaxDeconvRatioV->Fill( maxdeconvV/hit_ADCmaxsampV[ihit] );
	  
	  //Guess starting parameters:
	  // Pulse max. height is A e(-1) = Amax --> A = Amax exp(1.0);
	  startpar[0] = hit_ADCmaxsampU[ihit] * exp(1.0);
	  startpar[2] = tau0;
	  startpar[1] = 0.0;
	  startpar[3] = 0.0;

	  PulseShapeFit->SetParameters(startpar);
	  if( fixtau != 0 ) PulseShapeFit->FixParameter(2,tau0);
	  TFitResultPtr fU = gPulseU->Fit( PulseShapeFit, "S0Q" );

	  hchi2dof_U->Fill( fU->Chi2()/fU->Ndf() );
	  
	  double Afit = PulseShapeFit->GetParameter(0);
	  double t0fit = PulseShapeFit->GetParameter(1);
	  double taufit = PulseShapeFit->GetParameter(2);
	  double baseline = PulseShapeFit->GetParameter(3);
	  
	  hA_U->Fill( Afit*exp(-1.0) );
	  hB_U->Fill( baseline );
	  ht0_U->Fill( t0fit );
	  htau_U->Fill( taufit );
	  htdiff_hodo_U->Fill( t0fit - thodo );
	  htdiff_trig_U->Fill( t0fit - ttrig );
	  htdiff_fit_mean_U->Fill( t0fit + tau0 - hit_UtimeMaxStrip[ihit] );
	  htGEM_vs_thodo_U->Fill( thodo, t0fit );
	  htGEM_vs_ttrig_U->Fill( ttrig, t0fit );
	  htdiff_hodo_vs_A_U->Fill( Afit, t0fit-thodo );
	  htdiff_fit_mean_vs_A_U->Fill( Afit, t0fit + tau0 -hit_UtimeMaxStrip[ihit] );

	  htau_vs_A_U->Fill( Afit, taufit );

	  htGEM_vs_ttrig_U_prof->Fill( ttrig, t0fit );

	  htdiff_mean_hodo_U->Fill( hit_UtimeMaxStrip[ihit] - tau0 - thodo );
	  htdiff_mean_vs_hodo_U->Fill( thodo, hit_UtimeMaxStrip[ihit] - tau0 );

	  htdiff_mean_trig_U->Fill( hit_UtimeMaxStrip[ihit] - tau0 - ttrig );
	  htdiff_mean_vs_trig_U->Fill( ttrig, hit_UtimeMaxStrip[ihit] - tau0 );
	  
	  double tU = t0fit;

	  htdiff_trig_corr_U->Fill( tU - ttrig0 - ttrig_slope*ttrig );
	  
	  startpar[0] = hit_ADCmaxsampV[ihit] * exp(1.0);
	  startpar[2] = tau0;
	  startpar[1] = 0.0;
	  startpar[3] = 0.0;

	  PulseShapeFit->SetParameters(startpar);
	  if( fixtau != 0 ) PulseShapeFit->FixParameter(2,tau0);
	  TFitResultPtr fV = gPulseV->Fit( PulseShapeFit, "S0Q" );

	  hchi2dof_V->Fill( fV->Chi2()/fV->Ndf() );
	  
	  Afit = PulseShapeFit->GetParameter(0);
	  t0fit = PulseShapeFit->GetParameter(1);
	  taufit = PulseShapeFit->GetParameter(2);
	  baseline = PulseShapeFit->GetParameter(3);

	  hA_V->Fill( Afit*exp(-1.0) );
	  hB_V->Fill( baseline );
	  ht0_V->Fill( t0fit );
	  htau_V->Fill( taufit );
	  htdiff_hodo_V->Fill( t0fit - thodo );
	  htdiff_trig_V->Fill( t0fit - ttrig );
	  htdiff_fit_mean_V->Fill( t0fit + tau0 - hit_VtimeMaxStrip[ihit] );
	  htGEM_vs_thodo_V->Fill( thodo, t0fit );
	  htGEM_vs_ttrig_V->Fill( ttrig, t0fit );
	  htdiff_hodo_vs_A_V->Fill( Afit, t0fit-thodo );
	  htdiff_fit_mean_vs_A_V->Fill( Afit, t0fit + tau0 -hit_VtimeMaxStrip[ihit] );

	  htau_vs_A_V->Fill( Afit, taufit );

	  htGEM_vs_ttrig_V_prof->Fill( ttrig, t0fit );

	  htdiff_mean_hodo_V->Fill( hit_VtimeMaxStrip[ihit] - tau0 - thodo );
	  htdiff_mean_vs_hodo_V->Fill( thodo, hit_VtimeMaxStrip[ihit] - tau0 );

	  htdiff_mean_trig_V->Fill( hit_VtimeMaxStrip[ihit] - tau0 - ttrig );
	  htdiff_mean_vs_trig_V->Fill( ttrig, hit_VtimeMaxStrip[ihit] - tau0 );
	  
	  double tV = t0fit;

	  htdiff_trig_corr_V->Fill( tV - ttrig0 - ttrig_slope*ttrig );
	  
	  htdiff_UV->Fill( tU-tV );
	  htUvsV->Fill( tV, tU );

	  htdiff_UVmean->Fill( hit_UtimeMaxStrip[ihit]-hit_VtimeMaxStrip[ihit] );
	  htdiff_UvsVmean->Fill( hit_VtimeMaxStrip[ihit]-tau0,hit_UtimeMaxStrip[ihit]-tau0 );
	  
	  hUtimeMaxStrip_vs_module->Fill( hit_module[ihit], hit_UtimeMaxStrip[ihit] );
	  hVtimeMaxStrip_vs_module->Fill( hit_module[ihit], hit_VtimeMaxStrip[ihit] );
	  //gPulseU->Delete();
	  //gPulseV->Delete();
	}
      }
    }
  }

  ofstream outfile("GEM_striptimecuts_temp.dat");

  
  
  for( int i=1; i<=nmodules; i++ ){

   
    
    TH1D *htempU = hUtimeMaxStrip_vs_module->ProjectionY( TString::Format("hUtime_mod%d", i-1 ), i, i );
    TH1D *htempV = hVtimeMaxStrip_vs_module->ProjectionY( TString::Format("hVtime_mod%d", i-1 ), i, i );

    double meanU, rmsU, meanV, rmsV;

    meanU = htempU->GetMean();
    rmsU = htempU->GetRMS();
    meanV = htempV->GetMean();
    rmsV = htempV->GetRMS();

    outfile << TString::Format( "%s.m%d.maxstrip_t0 = %8.3g", prefix.Data(), i-1, 0.5*(meanU+meanV) ) << endl;
    outfile << TString::Format( "%s.m%d.maxstrip_tcut = %8.3g", prefix.Data(), i-1, 3.5*(rmsU+rmsV)/2.0 ) << endl;

    
  }
  
  
  elist->Delete();
  fout->Write(); 
};
