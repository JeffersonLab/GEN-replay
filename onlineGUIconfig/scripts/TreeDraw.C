void TreeDraw( const char *varexp, const char *cutexp="", const char *drawopt="" ){
  TTree *T = (TTree*) gFile->Get("T");

  //gStyle->SetPalette(kRainBow);
  gStyle->SetPalette(53);

  gPad->cd();

  if( T ){
    T->Draw(varexp, cutexp, drawopt);
  }
}
