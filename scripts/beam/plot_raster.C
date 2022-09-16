R__ADD_INCLUDE_PATH($SBS/include)
R__ADD_LIBRARY_PATH($SBS/lib64)
R__ADD_LIBRARY_PATH($SBS/lib)
R__LOAD_LIBRARY(libsbs.so)

#if !defined(__CLING__) || defined(__ROOTCLING__)
#include <cstdlib>
#include <iostream>
#include <string>

#include "TStyle.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TSystem.h"

#include "THaRun.h"
#include "THaEvent.h"
#include "THaAnalyzer.h"
#include "THaApparatus.h"
#include "THaVarList.h"
#include "THaInterface.h"
#include "THaHRS.h"
#include "THaDecData.h"

#include "SBSRasteredBeam.h"
#endif

using namespace std;

const std::string SCRIPT = "[plot_raster]: "; 

int plot_raster(TString codafname,Int_t runNo,Int_t firsteve,Int_t lasteve,TString rootfname){

  gStyle->SetOptStat(0);
  string exp = getenv("EXPERIMENT");
    
  gStyle->SetPalette(1);
  gStyle->SetNdivisions(605);
  TString* ca_title = new TString("Raster Size (");
  ca_title->Append(codafname);
  ca_title->Append(")");

  // TCanvas* c1 = new TCanvas("c1",ca_title->Data(),1800,1200);

  // c1->Divide(3,2);
  // c1->cd(1);
  // TLatex *t = new TLatex();
  // t->SetTextFont(32);
  // t->SetTextColor(1);
  // t->SetTextSize(0.015);
  // t->SetTextAlign(12);
  // t->DrawLatex(0.1,0.335,ca_title->Data());

  TCanvas* fc1 = new TCanvas("fc1",ca_title->Data(),1800,1200);

  fc1->Divide(3,2);

  TLatex *ft = new TLatex();
  ft->SetTextFont(32);
  ft->SetTextColor(1);
  ft->SetTextSize(0.015);
  ft->SetTextAlign(12);
  ft->DrawLatex(0.01,0.335,ca_title->Data());

  gStyle->SetOptStat(0);
  gStyle->SetStatH(0.3);
  gStyle->SetStatW(0.3);
  gStyle->SetTitleH(0.10);
  gStyle->SetTitleW(0.5);
  gStyle->SetLabelSize(0.06,"x");
  gStyle->SetLabelSize(0.06,"y");
  gROOT->ForceStyle();

  std::cout << SCRIPT << Form("Trying file '%s'...",rootfname.Data()) << std::endl;
  TFile *spotFile = TFile::Open(rootfname);
  if(spotFile==NULL){
     std::cout << SCRIPT << Form("ERROR! Cannot open file '%s'.  Exiting.",rootfname.Data()) << std::endl;
     return 1;
  }
  if( spotFile->IsOpen() ){
     std::cout << SCRIPT << Form("File '%s' opened successfully",rootfname.Data()) << std::endl;
  }else{
     std::cout << SCRIPT << Form("ERROR! Cannot open file '%s'.  Exiting.",rootfname.Data()) << std::endl;
     return 1;
  }

//  TH1F *bpma_x; spotFile->GetObject("bpma_x", bpma_x);
//  TH1F *bpma_y; spotFile->GetObject("bpma_y", bpma_y);
//  TH1F *bpmb_x; spotFile->GetObject("bpmb_x", bpmb_x);
//  TH1F *bpmb_y; spotFile->GetObject("bpmb_y", bpmb_y);
//  TH2F *bpma_xy; spotFile->GetObject("bpma_xy", bpma_xy);
//  TH2F *bpmb_xy; spotFile->GetObject("bpmb_xy", bpmb_xy);
//  TH2F *beam_xy; spotFile->GetObject("beam_xy", beam_xy);
//  TH2F *rastxy1; spotFile->GetObject("rastxy1", rastxy1);
//  TH2F *rastxy2; spotFile->GetObject("rastxy2", rastxy2);
//  TH2F *rastx1x2; spotFile->GetObject("rastx1x2", rastx1x2);
//  TH2F *rasty1y2; spotFile->GetObject("rasty1y2", rasty1y2);
//  TH2F *rastx1y2; spotFile->GetObject("rastx1y2", rastx1y2);
//  TH2F *rastx2y1; spotFile->GetObject("rastx2y1", rastx2y1);
  
//  TH1F *beam_x; spotFile->GetObject("beam_x", beam_x);
//  TH1F *beam_y; spotFile->GetObject("beam_y", beam_y);

// TH1F *bpmaraw1; spotFile->GetObject("bpmaraw1", bpmaraw1);
// TH1F *bpmaraw2; spotFile->GetObject("bpmaraw2", bpmaraw2);
// TH1F *bpmaraw3; spotFile->GetObject("bpmaraw3", bpmaraw3);
// TH1F *bpmaraw4; spotFile->GetObject("bpmaraw4", bpmaraw4);
// TH1F *bpmbraw1; spotFile->GetObject("bpmbraw1", bpmbraw1);
// TH1F *bpmbraw2; spotFile->GetObject("bpmbraw2", bpmbraw2);
// TH1F *bpmbraw3; spotFile->GetObject("bpmbraw3", bpmbraw3);
// TH1F *bpmbraw4; spotFile->GetObject("bpmbraw4", bpmbraw4);
// TH1F *bpmaraws; spotFile->GetObject("bpmaraws", bpmaraws);
// TH1F *bpmbraws; spotFile->GetObject("bpmbraws", bpmbraws);

// TH1F *URastX; spotFile->GetObject("URastX", URastX);
// TH1F *URastY; spotFile->GetObject("URastY", URastY);
// TH1F *DRastX; spotFile->GetObject("DRastX", DRastX);
// TH1F *DRastY; spotFile->GetObject("DRastY", DRastY);

// TH2F *urastraw_bpma_x; spotFile->GetObject("urastraw_bpma_x", urastraw_bpma_x);
// TH2F *urastraw_bpma_y; spotFile->GetObject("urastraw_bpma_y", urastraw_bpma_y);
// TH2F *urastraw_bpmb_x; spotFile->GetObject("urastraw_bpmb_x", urastraw_bpmb_x);
// TH2F *urastraw_bpmb_y; spotFile->GetObject("urastraw_bpmb_y", urastraw_bpmb_y);

// TH2F *urastraw_y_bpma_x; spotFile->GetObject("urastraw_y_bpma_x", urastraw_y_bpma_x);
// TH2F *urastraw_x_bpma_y; spotFile->GetObject("urastraw_x_bpma_y", urastraw_x_bpma_y);
// TH2F *urastraw_y_bpmb_x; spotFile->GetObject("urastraw_y_bpmb_x", urastraw_y_bpmb_x);
// TH2F *urastraw_x_bpmb_y; spotFile->GetObject("urastraw_x_bpmb_y", urastraw_x_bpmb_y);

// TH2F *drastraw_bpma_x; spotFile->GetObject("drastraw_bpma_x", drastraw_bpma_x);
// TH2F *drastraw_bpma_y; spotFile->GetObject("drastraw_bpma_y", drastraw_bpma_y);
// TH2F *drastraw_bpmb_x; spotFile->GetObject("drastraw_bpmb_x", drastraw_bpmb_x);
// TH2F *drastraw_bpmb_y; spotFile->GetObject("drastraw_bpmb_y", drastraw_bpmb_y);

// TH2F *drastraw_y_bpma_x; spotFile->GetObject("drastraw_y_bpma_x", drastraw_y_bpma_x);
// TH2F *drastraw_x_bpma_y; spotFile->GetObject("drastraw_x_bpma_y", drastraw_x_bpma_y);
// TH2F *drastraw_y_bpmb_x; spotFile->GetObject("drastraw_y_bpmb_x", drastraw_y_bpmb_x);
// TH2F *drastraw_x_bpmb_y; spotFile->GetObject("drastraw_x_bpmb_y", drastraw_x_bpmb_y);
// BELOW IS FOR FADC FADC FADC --THREE TIMES FOR EVERYTHING IMPORTANT

 TH1F *fbpma_x; spotFile->GetObject("fbpma_x", fbpma_x);
 TH1F *fbpma_y; spotFile->GetObject("fbpma_y", fbpma_y);
 TH1F *fbpmb_x; spotFile->GetObject("fbpmb_x", fbpmb_x);
 TH1F *fbpmb_y; spotFile->GetObject("fbpmb_y", fbpmb_y);
 TH2F *fbpma_xy; spotFile->GetObject("fbpma_xy", fbpma_xy);
 TH2F *fbpmb_xy; spotFile->GetObject("fbpmb_xy", fbpmb_xy);
 TH2F *fbeam_xy; spotFile->GetObject("fbeam_xy", fbeam_xy);
 TH2F *frastxy1; spotFile->GetObject("frastxy1", frastxy1);
 TH2F *frastxy2; spotFile->GetObject("frastxy2", frastxy2);
 TH2F *frastxy2_spatial; spotFile->GetObject("frastxy2_spatial", frastxy2_spatial);
 TH2F *frastx1x2; spotFile->GetObject("frastx1x2", frastx1x2);
 TH2F *frasty1y2; spotFile->GetObject("frasty1y2", frasty1y2);
 TH2F *frastx1y2; spotFile->GetObject("frastx1y2", frastx1y2);
 TH2F *frastx2y1; spotFile->GetObject("frastx2y1", frastx2y1);
  
 TH1F *fbeam_x; spotFile->GetObject("fbeam_x", fbeam_x);
 TH1F *fbeam_y; spotFile->GetObject("fbeam_y", fbeam_y);

 TH1F *fbpmaraw1; spotFile->GetObject("fbpmaraw1", fbpmaraw1);
 TH1F *fbpmaraw2; spotFile->GetObject("fbpmaraw2", fbpmaraw2);
 TH1F *fbpmaraw3; spotFile->GetObject("fbpmaraw3", fbpmaraw3);
 TH1F *fbpmaraw4; spotFile->GetObject("fbpmaraw4", fbpmaraw4);
 TH1F *fbpmbraw1; spotFile->GetObject("fbpmbraw1", fbpmbraw1);
 TH1F *fbpmbraw2; spotFile->GetObject("fbpmbraw2", fbpmbraw2);
 TH1F *fbpmbraw3; spotFile->GetObject("fbpmbraw3", fbpmbraw3);
 TH1F *fbpmbraw4; spotFile->GetObject("fbpmbraw4", fbpmbraw4);
 TH1F *fbpmaraws; spotFile->GetObject("fbpmaraws", fbpmaraws);
 TH1F *fbpmbraws; spotFile->GetObject("fbpmbraws", fbpmbraws);

 TH1F *fURastX; spotFile->GetObject("fURastX", fURastX);
 TH1F *fURastY; spotFile->GetObject("fURastY", fURastY);
 TH1F *fDRastX; spotFile->GetObject("fDRastX", fDRastX);
 TH1F *fDRastY; spotFile->GetObject("fDRastY", fDRastY);
 TH1F *fDRastX_spatial; spotFile->GetObject("fDRastX_spatial", fDRastX_spatial);
 TH1F *fDRastY_spatial; spotFile->GetObject("fDRastY_spatial", fDRastY_spatial);

 TH2F *furastraw_bpma_x; spotFile->GetObject("furastraw_bpma_x", furastraw_bpma_x);
 TH2F *furastraw_bpma_y; spotFile->GetObject("furastraw_bpma_y", furastraw_bpma_y);
 TH2F *furastraw_bpmb_x; spotFile->GetObject("furastraw_bpmb_x", furastraw_bpmb_x);
 TH2F *furastraw_bpmb_y; spotFile->GetObject("furastraw_bpmb_y", furastraw_bpmb_y);

 TH2F *furastraw_y_bpma_x; spotFile->GetObject("furastraw_y_bpma_x", furastraw_y_bpma_x);
 TH2F *furastraw_x_bpma_y; spotFile->GetObject("furastraw_x_bpma_y", furastraw_x_bpma_y);
 TH2F *furastraw_y_bpmb_x; spotFile->GetObject("furastraw_y_bpmb_x", furastraw_y_bpmb_x);
 TH2F *furastraw_x_bpmb_y; spotFile->GetObject("furastraw_x_bpmb_y", furastraw_x_bpmb_y);

 TH2F *fdrastraw_bpma_x; spotFile->GetObject("fdrastraw_bpma_x", fdrastraw_bpma_x);
 TH2F *fdrastraw_bpma_y; spotFile->GetObject("fdrastraw_bpma_y", fdrastraw_bpma_y);
 TH2F *fdrastraw_bpmb_x; spotFile->GetObject("fdrastraw_bpmb_x", fdrastraw_bpmb_x);
 TH2F *fdrastraw_bpmb_y; spotFile->GetObject("fdrastraw_bpmb_y", fdrastraw_bpmb_y);

 TH2F *fdrastraw_y_bpma_x; spotFile->GetObject("fdrastraw_y_bpma_x", fdrastraw_y_bpma_x);
 TH2F *fdrastraw_x_bpma_y; spotFile->GetObject("fdrastraw_x_bpma_y", fdrastraw_x_bpma_y);
 TH2F *fdrastraw_y_bpmb_x; spotFile->GetObject("fdrastraw_y_bpmb_x", fdrastraw_y_bpmb_x);
 TH2F *fdrastraw_x_bpmb_y; spotFile->GetObject("fdrastraw_x_bpmb_y", fdrastraw_x_bpmb_y);

 //For projecting onto collimator and target
 // TH1F *col_x; spotFile->GetObject("col_x", col_x);
 // TH1F *col_y; spotFile->GetObject("col_y", col_y);
 TH1F *targ_x; spotFile->GetObject("targ_x", targ_x);
 TH1F *targ_y; spotFile->GetObject("targ_y", targ_y);

 // TH2F *col_xy; spotFile->GetObject("col_xy", col_xy);
 TH2F *targ_xy; spotFile->GetObject("targ_xy", targ_xy);

 fbpma_x->GetXaxis()->SetLabelSize(0.075);
 fbpma_x->GetYaxis()->SetLabelSize(0.075);
 fbpma_y->GetXaxis()->SetLabelSize(0.075);
 fbpma_y->GetYaxis()->SetLabelSize(0.075);
 fbpma_xy->GetXaxis()->SetLabelSize(0.075);
 fbpma_xy->GetYaxis()->SetLabelSize(0.075);
 fbpmb_x->GetXaxis()->SetLabelSize(0.075);
 fbpmb_x->GetYaxis()->SetLabelSize(0.075);

 fbpmb_y->GetXaxis()->SetLabelSize(0.075);
 fbpmb_y->GetYaxis()->SetLabelSize(0.075);
 fbpmb_xy->GetXaxis()->SetLabelSize(0.075);
 fbpmb_xy->GetYaxis()->SetLabelSize(0.075);
 fbeam_x->GetXaxis()->SetLabelSize(0.075);
 fbeam_x->GetYaxis()->SetLabelSize(0.075);
 fbeam_y->GetXaxis()->SetLabelSize(0.075);
 fbeam_y->GetYaxis()->SetLabelSize(0.075);
 fbeam_xy->GetXaxis()->SetLabelSize(0.075);
 fbeam_xy->GetYaxis()->SetLabelSize(0.075);

  // BELOW IS FOR FADC FADC FADC --THREE TIMES FOR EVERYTHING IMPORTANT////
  fc1->cd(1);
  fbpma_xy->Draw("colz");
  gPad->SetGridx();  
  gPad->SetGridy();
  fc1->cd(4);
  fbpmb_xy->Draw("colz");
  gPad->SetGridx();
  gPad->SetGridy();
 
  fc1->cd(2);
  fbpma_x->Draw();
  fc1->cd(5);
  fbpmb_x->Draw();
 
  fc1->cd(3);
  fbpma_y->Draw();
  fc1->cd(6);
  fbpmb_y->Draw();

  TCanvas* fc2 =  new TCanvas("fc2","FADC Upstream Raster Plots ",1200,600);
  fc2->Divide(3,1);
  fc2->cd(1);
  // Added to fix axes, show raster x-y in full window
  // TH2F *fhaxis = new TH2F("fhaxis","FADC Fast Raster 1 X vs.Y",500,60000,70000,500,60000,70000);
  //fhaxis->Draw();
  frastxy1->SetStats(0); 
  frastxy1->Draw("colz");
  frastxy1->SetTitle("FADC Upstream Raster Y vs. X");
  frastxy1->SetAxisRange(0, 150000,"Y");
  frastxy1->SetAxisRange(0, 150000,"X");
  frastxy1->GetXaxis()->SetLabelSize(.04);
  frastxy1->GetYaxis()->SetLabelSize(.03);
  frastxy1->GetXaxis()->SetTitle("X");
  frastxy1->GetYaxis()->SetTitle("Y");
  gPad->SetGrid(1,1);
  gStyle->SetOptStat(1);

  fc2->cd(2);
  fURastX->Draw();
  fURastX->SetTitle("FADC Upstream Raster X");
  fURastX->GetXaxis()->SetLabelSize(.04);
  fURastX->GetYaxis()->SetLabelSize(.04);

  fc2->cd(3);
  fURastY->SetTitle("FADC Upstream Raster Y");
  fURastY->Draw();
  fURastY->GetXaxis()->SetLabelSize(.04);
  fURastY->GetYaxis()->SetLabelSize(.04);

  TCanvas* fc2A =  new TCanvas("fc2A","FADC Downstream Raster Plots",1800,600);
  fc2A->Divide(3,1);
  fc2A->cd(1);
  // Added to fix axes, show raster x-y in full window
  // TH2F *fhaxis2 = new TH2F("fhaxis","FADC Fast Raster 2 X vs.Y",500,60000,70000,500,60000,70000);
  // fhaxis2->Draw();
  frastxy2->SetStats(0);  
  frastxy2->Draw("colz");
  frastxy2->SetTitle("FADC Downstream Raster Y vs X");
  frastxy2->GetXaxis()->SetLabelSize(.04);
  frastxy2->GetYaxis()->SetLabelSize(.03);
  frastxy2->GetXaxis()->SetTitle("X");
  frastxy2->GetYaxis()->SetTitle("Y");
  gPad->SetGrid(1,1);
  //frastxy2->Draw("same colz");

  fc2A->cd(2);
  fDRastX->Draw();
  fDRastX->SetTitle("FADC Downstream Raster X");
  fDRastX->GetXaxis()->SetLabelSize(.04);
  fDRastX->GetYaxis()->SetLabelSize(.04);

  fc2A->cd(3);
  fDRastY->Draw();
  fDRastY->SetTitle("FADC Downstream Raster Y");
  fDRastY->GetXaxis()->SetLabelSize(.04);
  fDRastY->GetYaxis()->SetLabelSize(.04);
 
  TCanvas* fc2B =  new TCanvas("fc2B","fadc check",1800,1200);
  //  gStyle->SetOptStat(0);
  fc2B->Divide(2,1);
  fc2B->cd(1);
  frastx1x2->Draw("colz");
  frastx1x2->GetXaxis()->SetLabelSize(.05);
  frastx1x2->GetYaxis()->SetLabelSize(.038);
  gPad->SetGrid(1,1);
  gStyle->SetOptStat(1);
  fc2B->cd(2);
  frasty1y2->Draw("colz");
  frasty1y2->GetXaxis()->SetLabelSize(.05);
  frasty1y2->GetYaxis()->SetLabelSize(.038);
  gPad->SetGrid(1,1);
  
  TCanvas* fc3 =  new TCanvas("fc3","FADC BPM Antenna Plots",1800,1200);
  //  gStyle->SetOptStat(0);
  fc3->Divide(4,2);

  fc3->cd(1);
  fbpmaraw1->Draw();
//  fbpmaraw1->Fit("gaus");
  fbpmaraw1->GetXaxis()->SetLabelSize(.04);
  fc3->cd(2);
  fbpmaraw2->Draw();
  fbpmaraw2->GetXaxis()->SetLabelSize(.04);

  fc3->cd(3);
  fbpmaraw3->Draw();
  fbpmaraw3->GetXaxis()->SetLabelSize(.04);

  fc3->cd(4);
  fbpmaraw4->Draw(); 
  fbpmaraw4->GetXaxis()->SetLabelSize(.04);

  fc3->cd(5);
  fbpmbraw1->Draw();
  fbpmbraw1->GetXaxis()->SetLabelSize(.04);

  fc3->cd(6);
  fbpmbraw2->Draw();
  fbpmbraw2->GetXaxis()->SetLabelSize(.04);

  fc3->cd(7);
  fbpmbraw3->Draw();
  fbpmbraw3->GetXaxis()->SetLabelSize(.04);

  fc3->cd(8);
  fbpmbraw4->Draw(); 
  fbpmbraw4->GetXaxis()->SetLabelSize(.04);


  TCanvas* fc4 =  new TCanvas("c4","FADC Raster vs BPM Plots",1600,800);
  //  gStyle->SetOptStat(0);
  fc4->Divide(4,2);

 /* fc4->cd(1);
  furastraw_bpma_x->Draw("col");
  fc4->cd(2);
  furastraw_bpma_y->Draw("col");
  fc4->cd(3);
  furastraw_bpmb_x->Draw("col");
  fc4->cd(4);
  furastraw_bpmb_y->Draw("col");*/
  fc4->cd(5);
  fdrastraw_bpma_x->Draw("col");
  fc4->cd(6);
  fdrastraw_bpma_y->Draw("col");
  fc4->cd(7);
  fdrastraw_bpmb_x->Draw("col");
  fc4->cd(8);
  fdrastraw_bpmb_y->Draw("col");

  TCanvas* fc5 =  new TCanvas("fc5","FADC Beam Positions (A,B,Target)",1200,1200);
  fc5->Divide(3,3);

  fc5->cd(1);
  gPad->SetGridx();  
  gPad->SetGridy();
  fbpma_xy->Draw("col");
  fc5->cd(2);
  fbpma_x->Draw();
  fc5->cd(3);
  fbpma_y->Draw();
  fc5->cd(4);
  gPad->SetGridx();  
  gPad->SetGridy();
  fbpmb_xy->Draw("col");
  fc5->cd(5);
  fbpmb_x->Draw();
  fc5->cd(6);
  fbpmb_y->Draw();
  fc5->cd(7);
  gPad->SetGridx();  
  gPad->SetGridy();
  fbeam_xy->Draw("col");
  fc5->cd(8);
  fbeam_x->Draw();
  fc5->cd(9);
  fbeam_y->Draw();


  TCanvas* fc6 = new TCanvas("fc6", "Beam Projections");
  fc6->SetFixedAspectRatio();
  fc6->Divide(3,3);
  fc6->cd(1);
  targ_xy->Draw("col");
  gPad->SetGridx();  
  gPad->SetGridy();
  fc6->cd(2);
  targ_x->Draw();
  fc6->cd(3);
  targ_y->Draw();
  fc6->cd(4);
  // col_xy->Draw("col");
  gPad->SetGridx();  
  gPad->SetGridy();
  fc6->cd(5);
  // col_x->Draw();
  fc6->cd(6);
  // col_y->Draw();

  std::cout << SCRIPT << "++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << SCRIPT << "****** Beam Check For SBS ******" << std::endl;
  std::cout << SCRIPT << "++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << SCRIPT << "CODA-file:   " << codafname       << std::endl;
  std::cout << SCRIPT << "first event: " << firsteve        << std::endl;
  std::cout << SCRIPT << "last event:  " << lasteve         << std::endl;
  std::cout << SCRIPT << "++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << SCRIPT << "Type .q when you are done"        << std::endl;

  TString RUN = Form("%d",runNo); 

  TString name4=Form("FADC_%s_",exp.c_str());
  name4.Append(RUN);
  name4.Append(".pdf[");

  TString name5=Form("FADC_%s_",exp.c_str());
  name5.Append(RUN);
  name5.Append(".pdf");

  TString name6=Form("FADC_%s_",exp.c_str());
  name6.Append(RUN);
  name6.Append(".pdf]");

  //c5 countains c1
  // c5->Print(name1);
  // c5->Print(name2);
  // c3->Print(name2);
  // c2->Print(name2);
  // c2A->Print(name2);
  // c2B->Print(name2);
  // c2B->Print(name3);

  fc5->SaveAs(name4);
  fc5->SaveAs(name5);
  fc3->SaveAs(name5);
  fc2->SaveAs(name5);
  fc2A->SaveAs(name5);
  fc2B->SaveAs(name5);
  fc6->SaveAs(name5);
  fc2B->SaveAs(name6);

  return 0;
}
