void hcal_timing(){

  cout << "Processing macro.." << endl;

  TStopwatch *st = new TStopwatch();
  st->Start(kTRUE);

  Double_t sbs_hcal_clus_blk_tdctime[30] = {0.}, sbs_hcal_clus_blk_atime[30] = {0.}, sbs_hcal_nclus = 0.; 

  TH2D *h2_hcal_cluster_tdc_vs_adc = new TH2D("h2_hcal_cluster_tdc_vs_adc","HCal Timing Check; HCal TDC; HCal ADC time",300,-300,300,80,0,160);

  // Declare branches
  TTree *T = (TTree*) gDirectory->Get("T");
  T->SetBranchStatus("*",0);
  //T->SetBranchStatus("sbs.hcal.*",1);
  T->SetBranchStatus("sbs.hcal.clus_blk.atime",1);
  T->SetBranchStatus("sbs.hcal.clus_blk.tdctime",1);
  T->SetBranchStatus("sbs.hcal.nclus",1);

  T->SetBranchAddress("sbs.hcal.clus_blk.atime", &sbs_hcal_clus_blk_atime);
  T->SetBranchAddress("sbs.hcal.clus_blk.tdctime", &sbs_hcal_clus_blk_tdctime);
  T->SetBranchAddress("sbs.hcal.nclus", &sbs_hcal_nclus);

  // Acquire the number of entries
  Long64_t nevents = T->GetEntries();
  
  for(Long64_t nevent=0; nevent<nevents; nevent++){

    T->GetEntry(nevent);
    
    if( sbs_hcal_clus_blk_atime[0]<1 || sbs_hcal_clus_blk_tdctime[0]>1000 || sbs_hcal_nclus==0 ) continue;
    
    h2_hcal_cluster_tdc_vs_adc->Fill( sbs_hcal_clus_blk_tdctime[0], sbs_hcal_clus_blk_atime[0] );

  }
  
  gStyle->SetPalette(53);
  h2_hcal_cluster_tdc_vs_adc->SetMinimum(-0.1);
  h2_hcal_cluster_tdc_vs_adc->SetStats(0);
  h2_hcal_cluster_tdc_vs_adc->Draw("colz");

  cout << "Processed macro with " << nevents << " entries." << endl;
  
  st->Stop();
  cout << "CPU time = " << st->CpuTime() << " s " << " Real time = " << st->RealTime() << " s " << endl;
}
