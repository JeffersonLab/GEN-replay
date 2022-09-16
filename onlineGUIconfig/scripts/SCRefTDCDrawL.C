void SCRefTDCDrawL()
{

  gPad->SetLogy();

  TH2F *hRefLE2D = (TH2F*)gDirectory->Get("h2d_RefLE");  
  TH2F *hRefTE2D = (TH2F*)gDirectory->Get("h2d_RefTE");

  TH1F *hRefLEL = (TH1F*)hRefLE2D->ProjectionX("hRefLEL",0,1);
  hRefLEL->SetLineColor(2);
  hRefLEL->Draw();

  TH1F *hRefTEL = (TH1F*)hRefTE2D->ProjectionX("hRefTEL",0,1);
  hRefTEL->SetLineColor(4);
  hRefTEL->Draw("sames");
  
}
