void SCRefMultDrawL()
{

  gPad->SetLogy();

  TH2F *hRefMult2D = (TH2F*)gDirectory->Get("h2d_RefMult");  

  TH1F *hRefMultL = (TH1F*)hRefMult2D->ProjectionX("hRefMultL",0,1);
  hRefMultL->Draw();
}
