#include "TH1D.h"
#include "TH2D.h"
#include "TChain.h"
#include "TTree.h"
#include "TCut.h"
#include "TEventList.h"
#include "TFile.h"
#include "TROOT.h"
#include "TF1.h"
#include <iostream>
#include <fstream>
//#include <ROOT/RDataFrame.hxx>

//using namespace ROOT;

void GetTrackingCuts( const char *rootfilename, const char *outfilename="GMNtrackingcuts.root", int nmodules=12 ){

  ROOT::EnableImplicitMT(10);
  //We want this macro to do several things:
  // 1. Set tracking constraints
  // 2. Set thresholds

  //TFile *fin = new TFile(rootfilename,"READ");

  TChain *T = new TChain("T");
  T->Add(rootfilename);

 
  
  //ROOT::RDataFrame F(*C);

  TCut global_cut =  "bb.tr.n==1&&abs(bb.tr.vz[0])<0.08&&bb.gem.track.chi2ndf[0]<10&&bb.gem.track.nhits[0]>3&&bb.ps.e>0.25&&abs(bb.etot_over_p-1.)<0.25";
  
  //TCut global_cut = "bb.tr.n==1&&abs(bb.tr.vz)<0.08&&bb.gem.track.chi2ndf<10&&bb.gem.track.nhits>3&&bb.ps.e>0.25&&abs(bb.etot_over_p-1.)<0.25";

  TEventList *elist = new TEventList("elist");

  T->Draw(">>elist",global_cut);

  T->SetEventList(elist);
  
  TFile *fout = new TFile( outfilename, "RECREATE" );
  
  TH1D *hdxfcp = new TH1D("hdxfcp", ";x_{track}-x_{fcp} (m);", 300, -0.15, 0.15 );
  TH1D *hdyfcp = new TH1D("hdyfcp", ";y_{track}-y_{fcp} (m);", 300, -0.15, 0.15 );
  TH1D *hdxbcp = new TH1D("hdxbcp", ";x_{track}+x'_{track}z_{bcp}-x_{bcp} (m);", 300, -0.15, 0.15 );
  TH1D *hdybcp = new TH1D("hdybcp", ";y_{track}+y'_{track}z_{bcp}-y_{bcp} (m);", 300, -0.15, 0.15 );

  TH2D *hdxdyfcp = new TH2D("hdxdyfcp", ";y_{track}-y_{fcp} (m);x_{track}-x_{fcp} (m)", 150, -0.15, 0.15, 150, -0.15, 0.15 );
  TH2D *hdxdybcp = new TH2D("hdxdybcp", ";y_{track}+y'_{track}z_{bcp}-y_{bcp} (m);x_{track}+x'_{track}z_{bcp}-x_{bcp} (m)", 150, -0.15, 0.15, 150, -0.15, 0.15 );

  TH1D *hdthcp = new TH1D("hdthcp", "; x'_{track}-x'_{constraint};", 300, -0.1, 0.1 );
  TH1D *hdphcp = new TH1D("hdphcp", "; y'_{track}-y'_{constraint};", 300, -0.1, 0.1 );
  
  T->Project( "hdxbcp", "bb.tr.x+bb.tr.th*bb.z_bcp-bb.x_bcp" );
  T->Project( "hdybcp", "bb.tr.y+bb.tr.ph*bb.z_bcp-bb.y_bcp" );
  T->Project( "hdxdybcp", "bb.tr.x+bb.tr.th*bb.z_bcp-bb.x_bcp:bb.tr.y+bb.tr.ph*bb.z_bcp-bb.y_bcp" );
  T->Project( "hdxfcp", "bb.tr.x+bb.tr.th*bb.z_fcp-bb.x_fcp" );
  T->Project( "hdyfcp", "bb.tr.y+bb.tr.ph*bb.z_fcp-bb.y_fcp" );
  T->Project( "hdxdyfcp", "bb.tr.x+bb.tr.th*bb.z_fcp-bb.x_fcp:bb.tr.y+bb.tr.ph*bb.z_fcp-bb.y_fcp" );
  T->Project( "hdthcp", "bb.tr.th-(bb.x_bcp-bb.x_fcp)/(bb.z_bcp-bb.z_fcp)" );
  T->Project( "hdphcp", "bb.tr.ph-(bb.y_bcp-bb.y_fcp)/(bb.z_bcp-bb.z_fcp)" );

  TString fname_db = outfilename;
  fname_db.ReplaceAll(".root",".dat");

  ofstream dbfile(fname_db.Data());

  dbfile << "bb.frontconstraint_x0 = " << hdxfcp->GetMean() << endl;
  dbfile << "bb.frontconstraint_y0 = " << hdyfcp->GetMean() << endl;
  dbfile << "bb.backconstraint_x0 = " << hdxbcp->GetMean() << endl;
  dbfile << "bb.backconstraint_y0 = " << hdybcp->GetMean() << endl << endl;

  dbfile << "bb.frontconstraintwidth_x = " << hdxfcp->GetRMS() * 4.5 << endl;
  dbfile << "bb.frontconstraintwidth_y = " << hdyfcp->GetRMS() * 4.5 << endl;
  dbfile << "bb.backconstraintwidth_x = " << hdxbcp->GetRMS() * 4.5 << endl;
  dbfile << "bb.backconstraintwidth_y = " << hdybcp->GetRMS() * 4.5 << endl << endl;

  dbfile << "bb.gem.constraintwidth_theta = " << hdthcp->GetRMS() * 4.5 << endl;
  dbfile << "bb.gem.constraintwidth_phi = " << hdphcp->GetRMS() * 4.5 << endl;

  

  double threshsamp_module[nmodules], threshstrip_module[nmodules], threshclust_module[nmodules];

  dbfile << endl;

  

  
  //clustercut += ;
  
  //get individual sample width for each module and check it against track-based sample threshold; apply 5sigma or track-based, whichever is higher:
  for( int imod=0; imod<nmodules; imod++ ){
    TCut clustercut = (TString::Format("bb.gem.hit.nstripu>1&&bb.gem.hit.nstripv>1&&bb.gem.hit.module==%d",imod)).Data();
    
    TString histname;
    histname.Form("hADCpedsubU_allstrips_bb_gem_m%d", imod );

    TFile *fin = T->GetFile();
    
    TH1D *htemp;
    fin->GetObject( histname.Data(), htemp );

    int binmax = htemp->GetMaximumBin();
    double max = htemp->GetBinContent(binmax);

    int binlow=binmax, binhigh=binmax;

    while( htemp->GetBinContent(binlow) >= 0.4*max && binlow > 1 ){binlow--;}
    while( htemp->GetBinContent(binhigh) >= 0.4*max && binhigh < htemp->GetNbinsX() ){binhigh++; }

    double sigmaU = 20.0, sigmaV = 20.0;
    
    if( htemp->GetEntries() >= 100 ){
      htemp->Fit("gaus", "", "", htemp->GetBinCenter(binlow), htemp->GetBinCenter(binhigh) );

      sigmaU = ( ( (TF1*) htemp->GetListOfFunctions()->FindObject("gaus") ) )->GetParameter("Sigma");
    }
    //
    histname.Form("hADCpedsubV_allstrips_bb_gem_m%d", imod );

    TH1D *htempV;
    fin->GetObject( histname.Data(), htempV );

    binmax = htempV->GetMaximumBin();
    max = htempV->GetBinContent(binmax);

    binlow=binmax;
    binhigh=binmax;

    while( htempV->GetBinContent(binlow) >= 0.4*max && binlow > 1 ){binlow--;}
    while( htempV->GetBinContent(binhigh) >= 0.4*max && binhigh < htempV->GetNbinsX() ){binhigh++; }

    if( htempV->GetEntries() >= 100 ){
      htempV->Fit("gaus", "", "", htempV->GetBinCenter(binlow), htempV->GetBinCenter(binhigh) );
      
      sigmaV = ( (TF1*) (htempV->GetListOfFunctions()->FindObject("gaus") ) )->GetParameter("Sigma");
    }
    
    double sigma = 0.5*(sigmaU+sigmaV);
    
    threshsamp_module[imod] =  5.0*sigma;

    
    TString hsampname,hstripname,hclustname;
    hsampname.Form("hADCmaxsamp_m%d",imod);
    hstripname.Form("hADCmaxstrip_m%d",imod);
    hclustname.Form("hADCclust_m%d",imod);

    //fout->cd();
    
    TH1D *hADCmaxsamp = new TH1D(hsampname.Data(),";max ADC sample;", 1500, 0, 3000 );
    TH1D *hADCmaxstrip = new TH1D(hstripname.Data(), ";strip ADC sum;", 1500, 0, 15000 );
    TH1D *hADCclust = new TH1D(hclustname.Data(), ";cluster ADC sum;", 1500, 0, 30000 );
    
    T->Project( hsampname.Data(), "0.5*(bb.gem.hit.ADCmaxsampU+bb.gem.hit.ADCmaxsampV)", clustercut );

    double fraction = 0.003;

    int ibin=1;

    ibin=1;
    while( hADCmaxsamp->Integral(1,ibin) < fraction * hADCmaxsamp->GetEntries() ){ ibin++; }
    double threshsamp_avg = hADCmaxsamp->GetBinCenter( ibin );

    threshsamp_module[imod] = std::max( 3.5*sigma, std::min( threshsamp_avg, 5.0*sigma ) );

    TString samplecut;
    samplecut.Form("0.5*(bb.gem.hit.ADCmaxsampU+bb.gem.hit.ADCmaxsampV)>%g",threshsamp_module[imod]);

    cout << "\"" << samplecut << "\"" << endl;
    
    TCut modulecut = clustercut;
    modulecut += samplecut;

    cout << "\"" << modulecut << "\"" << endl;
    
					
    T->Project( hstripname.Data(), "0.5*(bb.gem.hit.ADCmaxstripU+bb.gem.hit.ADCmaxstripV)", modulecut );
    T->Project( hclustname.Data(), "bb.gem.hit.ADCavg", modulecut );

    hADCmaxsamp->Print();
    hADCmaxstrip->Print();
    hADCclust->Print();
    
    fraction = 0.002;
    
    ibin=1;
    while( hADCmaxstrip->Integral(1,ibin) < fraction * hADCmaxstrip->GetEntries() ){ ibin++; }
    double threshstrip_avg = hADCmaxstrip->GetBinCenter( ibin );
    
    ibin=1;
    while( hADCclust->Integral(1,ibin) < fraction * hADCclust->GetEntries() ){ ibin++; }
    
    double threshclust_avg = hADCclust->GetBinCenter( ibin );
    
    threshstrip_module[imod] = threshstrip_avg;
    threshclust_module[imod] = threshclust_avg;

    TString dbline;
    dbfile << dbline.Format( "bb.gem.m%d.threshold_sample = %g", imod, threshsamp_module[imod] ) << endl;
    dbfile << dbline.Format( "bb.gem.m%d.threshold_stripsum = %g", imod, threshstrip_module[imod] ) << endl;
    dbfile << dbline.Format( "bb.gem.m%d.threshold_clustersum = %g", imod, threshclust_module[imod] ) << endl << endl;
  }
  
  

  

  

  //  double fraction = 1.0-0.999;
  
  

  fout->Write();
  
}
