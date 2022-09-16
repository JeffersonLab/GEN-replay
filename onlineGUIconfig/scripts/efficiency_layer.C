void efficiency_layer(TString hdidhit, TString hshouldhit, int layer=0 ){
  TH1D* Hdid;
  TH1D* Hshould;
  
  Hdid = (TH1D*) gFile->Get(hdidhit);
  Hshould = (TH1D*) gFile->Get(hshouldhit);

  //Hshould->SetXTitle("x (m)");
  //Hshou

  Hshould->SetTitleSize(0.07,"X");
  Hshould->SetLabelSize(0.07,"XYZ");
  
  
  Hdid->SetStats(false);
  Hshould->SetStats(false);
  Hdid->SetTitle("");
  Hshould->SetTitle("");
  
  // if(golden){
  //   Hdid->SetLineColor(30);
  //   Hdid->SetFillColor(30);
  //   Hdid->SetFillStyle(3027);
  //   Hshould->SetLineColor(46);
  //   Hshould->SetFillColor(46);
  //   Hshould->SetFillStyle(3027);
  // } else {
  Hdid->SetLineColor(4);
  Hshould->SetLineColor(2);
  
  Hdid->SetLineWidth(2);
  Hshould->SetLineWidth(2);
  Hshould->SetLineStyle(10);
  
  // Hdid->SetFillColorAlpha(4,0.2);
  // Hshould->SetFillColorAlpha(2,0.2);
 
  Hshould->Draw();
  Hdid->Draw("same");

  gPad->SetBottomMargin(0.15);
  
  double Ntr = Hshould->GetEntries();
  double Nhit = Hdid->GetEntries();

  //detection efficiency = did/should
  // Ntrials = should hit
  // mean (Nsuccesses) = eps * N
  // sigma^2 = N eps * (1-eps) --> sigma = sqrt(N * eps * (1-eps)
  
  double efficiency = Nhit/Ntr;

  double defficiency = sqrt( efficiency * (1.0 - efficiency) / Ntr );

  double ymax = Hshould->GetBinContent( Hshould->GetMaximumBin() );

  double xmin = Hshould->GetXaxis()->GetXmin();
  double xmax = Hshould->GetXaxis()->GetXmax();
  
  Hshould->GetYaxis()->SetRangeUser( 0.0, 1.5*ymax );

  TPaveText *p = new TPaveText( xmin, ymax*1.01, xmax, 1.5*ymax, "br" );
  p->SetFillStyle(0);
  p->SetBorderSize(0);

  TString text;

  p->AddText( text.Format( "Layer %d Average", layer ) );
  
  text.Form( "Efficiency = (%5.2f #pm %5.2f) ", 100.*efficiency, 100.*defficiency );

  text += '%';
  
  p->AddText( text.Data() );
  p->AddText( text.Format("N. did hit = %d", int(Nhit) ) );
  p->AddText( text.Format("N. should hit = %d", int(Ntr) ) );

  p->Draw();
  
}
