//option = 0/1 (layer / module)
void efficiency_hit_map(TString hdidhit, TString hshouldhit, int layer=0, int option = 0 ){
  
  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);
  

  TH2D* Hdid;
  TH2D* Hshould;
  
  Hdid = (TH2D*) gFile->Get(hdidhit)->Clone("hnew");
  Hshould = (TH2D*) gFile->Get(hshouldhit);

  double Ntr = Hshould->GetEntries();
  double Nhit = Hdid->GetEntries();
  
   
  Hdid->Draw("colz");
  Hdid->Divide(Hshould);

  gPad->SetBottomMargin(0.15);
  
  
  //detection efficiency = did/should
  // Ntrials = should hit
  // mean (Nsuccesses) = eps * N
  // sigma^2 = N eps * (1-eps) --> sigma = sqrt(N * eps * (1-eps)
  
  double efficiency = Nhit/Ntr;

  double defficiency = sqrt( efficiency * (1.0 - efficiency) / Ntr );

  if(option == 0){
    if(layer < 4){
      Hdid->GetXaxis()->SetRangeUser(-0.3,0.3);
      Hdid->GetYaxis()->SetRangeUser(-0.8,0.8);
    }
    else{
      Hdid->GetXaxis()->SetRangeUser(-0.4,0.4);
      Hdid->GetYaxis()->SetRangeUser(-1.2,1.2); 
    }

     Hdid->SetTitle(Form("Layer %i, Efficiency = %g",layer,efficiency*100));
  }

  else{
    if(layer < 4){
      Hdid->GetXaxis()->SetRangeUser(-0.3,0.3);
      Hdid->GetYaxis()->SetRangeUser(-0.8,0.8);
    }
    else{
      Hdid->GetXaxis()->SetRangeUser(-0.4,0.4);
      Hdid->GetYaxis()->SetRangeUser(-0.6,0.6); 
    }

    Hdid->SetTitle(Form("Module %i, Efficiency = %g",layer,efficiency*100));
  }


  
}
