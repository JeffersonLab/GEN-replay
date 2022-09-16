void SBSBCM()
{
  gPad->SetLogy();
  
  TH1D* fIunserVsTime = (TH1D*)gDirectory->Get("fIunserVsTime");
  TH1D* fIu1VsTime = (TH1D*)gDirectory->Get("fIu1VsTime");
  TH1D* fIunewVsTime = (TH1D*)gDirectory->Get("fIunewVsTime");
  TH1D* fIdnewVsTime = (TH1D*)gDirectory->Get("fIdnewVsTime");
  TH1D* fId1VsTime = (TH1D*)gDirectory->Get("fId1VsTime");
  TH1D* fId3VsTime = (TH1D*)gDirectory->Get("fId3VsTime");
  TH1D* fId10VsTime = (TH1D*)gDirectory->Get("fId10VsTime");
  
  TLegend* LegL = new TLegend(0.1, 0.7, 0.5, 0.9, "scaler rates");
  TLegend* LegR = new TLegend(0.5, 0.7, 0.9, 0.9);
  LegL->AddEntry(fIunserVsTime, "unser", "P");
  LegL->AddEntry(fIu1VsTime, "u1", "P");
  LegL->AddEntry(fIunewVsTime, "unew", "P");
  LegR->AddEntry(fIdnewVsTime, "dnew", "P");
  LegR->AddEntry(fId1VsTime, "d1", "P");
  LegR->AddEntry(fId3VsTime, "d3", "P");
  LegR->AddEntry(fId10VsTime, "d10", "P");
  
  fIunserVsTime->GetXaxis()->SetRangeUser(0., 60);
  fIu1VsTime->GetXaxis()->SetRangeUser(0., 60);
  fIunewVsTime->GetXaxis()->SetRangeUser(0., 60);
  fIdnewVsTime->GetXaxis()->SetRangeUser(0., 60);
  fId1VsTime->GetXaxis()->SetRangeUser(0., 60);
  fId3VsTime->GetXaxis()->SetRangeUser(0., 60);
  fId10VsTime->GetXaxis()->SetRangeUser(0., 60);

  fIunserVsTime->SetMarkerColor(1);
  fIu1VsTime->SetMarkerColor(2);
  fIunewVsTime->SetMarkerColor(3);
  fIdnewVsTime->SetMarkerColor(4);
  fId1VsTime->SetMarkerColor(5);
  fId3VsTime->SetMarkerColor(6);
  fId10VsTime->SetMarkerColor(7);

  fIunserVsTime->SetMarkerStyle(21);
  fIu1VsTime->SetMarkerStyle(22);
  fIunewVsTime->SetMarkerStyle(23);
  fIdnewVsTime->SetMarkerStyle(24);
  fId1VsTime->SetMarkerStyle(25);
  fId3VsTime->SetMarkerStyle(26);
  fId10VsTime->SetMarkerStyle(27);

  fIunserVsTime->Draw("P");
  fIunserVsTime->SetMinimum(1.e3);
  fIunserVsTime->SetMaximum(25*fIunserVsTime->GetMaximum());
  fIu1VsTime->Draw("P, same");
  fIunewVsTime->Draw("P, same");
  fIdnewVsTime->Draw("P, same");
  fId1VsTime->Draw("P, same");
  fId3VsTime->Draw("P, same");
  fId10VsTime->Draw("P, same");
  LegL->Draw("same");
  LegR->Draw("same");
  
}
