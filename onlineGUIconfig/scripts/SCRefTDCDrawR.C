void SCRefTDCDrawR()
{

  gPad->SetLogy();

  TH2F *hRefLE2D = (TH2F*)gDirectory->Get("h2d_RefLE");  
  TH2F *hRefTE2D = (TH2F*)gDirectory->Get("h2d_RefTE");

  TH1F *hRefLER = (TH1F*)hRefLE2D->ProjectionX("hRefLER",1,2);
  hRefLER->SetLineColor(2);
  hRefLER->Draw();

  TH1F *hRefTER = (TH1F*)hRefTE2D->ProjectionX("hRefTER",1,2);
  hRefTER->SetLineColor(4);
  hRefTER->Draw("sames");
  
  
}
