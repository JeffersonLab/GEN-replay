void landau_fit(TString Thist, double right_bound){


  TH1D *ADC = (TH1D*) gFile->Get(Thist);

  ADC->Fit("landau","","",0,right_bound);

  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gPad->SetLogy();

}
