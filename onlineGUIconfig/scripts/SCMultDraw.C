void SCMultDraw()
{

  TH2F *hRawMult2D = (TH2F*)gDirectory->Get("h2d_RawMult");  
  
  TH1F *hRawMultL = new TH1F("hRawMultL","",90,0,90);
  TH1F *hRawMultR = new TH1F("hRawMultR","",90,0,90);
  
  TH1F *hRawMult = (TH1F*)hRawMult2D->ProjectionY("hRawMult");

  for(int i=0; i<hRawMult->GetNbinsX(); i++ ){
    if( i <= 89 )
      hRawMultL->SetBinContent(i, hRawMult->GetBinContent(i));
    else if( i >= 90 )
      hRawMultR->SetBinContent(i-90, hRawMult->GetBinContent(i));
  } 

  hRawMultL->SetTitle("Total Hits per PMT / Bar");  
  hRawMultL->SetLineColor(2);  
  hRawMultL->Sumw2(kFALSE);  
  hRawMultL->Draw();  

  hRawMultR->SetLineColor(4);  
  hRawMultR->Sumw2(kFALSE);  
  hRawMultR->Draw("sames");  

  TH2F *hBarMult2D = (TH2F*)gDirectory->Get("h2d_BarMT");  
  TH1F *hBarMult = (TH1F*)hBarMult2D->ProjectionY("hBarMultL");
  hBarMult->SetLineColor(1);  
  hBarMult->Sumw2(kFALSE);  
  hBarMult->Draw("sames");  

  TLatex* tex = new TLatex( 0.5, 0.25, "Bar");
  tex->SetNDC(1);
  tex->SetTextFont(42);
  tex->SetTextColor(1);
  tex->SetTextSize(0.05);
  tex->Draw();

  tex = new TLatex( 0.5, 0.20, "Left PMT");
  tex->SetNDC(1);
  tex->SetTextFont(42);
  tex->SetTextColor(2);
  tex->SetTextSize(0.05);
  tex->Draw();

  tex = new TLatex( 0.5, 0.15, "Right PMT");
  tex->SetNDC(1);
  tex->SetTextFont(42);
  tex->SetTextColor(4);
  tex->SetTextSize(0.05);
  tex->Draw();

}
