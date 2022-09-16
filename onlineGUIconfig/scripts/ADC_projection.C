void ADC_projection(TString Thist, int module){


  TH2D *ADC2D = (TH2D*) gFile->Get(Thist);

  TH1D *ADC1D = ADC2D->ProjectionY();

  ADC1D->SetTitle(Form("module %i", module));
  
  gPad->SetLogy();

  ADC1D->Draw();

 
}
