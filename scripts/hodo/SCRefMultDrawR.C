void SCRefMultDrawR()
{

  gPad->SetLogy();

  TH2F *hRefMult2D = (TH2F*)gDirectory->Get("h2d_RefMult");  

  TH1F *hRefMultR = (TH1F*)hRefMult2D->ProjectionX("hRefMultR",1,2);
  hRefMultR->Draw();
}
