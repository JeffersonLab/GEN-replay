#include <iostream>
#include "TChain.h"
#include "TFile.h"
#include "TMatrixD.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "gmn_rec_tree.C"

// filename
void BBCalCalibration(const char* filename, const int Nmin = 1000, const double minMBratio = 0.1, const int printstats = 0)
{
  TFile *_file0 = TFile::Open(filename);
  TChain *C = (TChain*)_file0->Get("T");
  gmn_rec_tree *T = new gmn_rec_tree(C);
  
  TH1D* h1_DeltaE = new TH1D("h1_DeltaE", ";1-E_{Rec}/p_{MC}", 100, -1, 1);
  
  TH2D* h1_EclusViClus_ps = new TH2D("h1_EclusViClus_ps", "PS;i_{clus};E_clus (GeV)", 15, 0, 15, 140, 0, 7);
  TH2D* h1_EclusViClus_sh = new TH2D("h1_EclusViClus_sh", "SH;i_{clus};E_clus (GeV)", 15, 0, 15, 140, 0, 7);
  
  Long64_t NEvts = C->GetEntries();
  
  //convention: 0-188: SH; 189-240: PS.
  TMatrixD M_jk(241, 241);
  TMatrixD B_j(241, 1);
  
  int nevts[241];
  memset(nevts, 0, 241*sizeof(int));
  
  double A_j[241];
  
  double E_i = 0;
  double A_i = 0;
  
  for(Long64_t nevent = 0; nevent<NEvts; nevent++){
    if( nevent%1000 == 0){
      cout << nevent << "/" << NEvts << endl;
    }
    T->GetEntry(nevent);
    
    E_i = A_i = 0;
    memset(A_j, 0, 241*sizeof(double));
    
    //select good track first:
    //assumption: the best track has the highest momentum
    double pmax = -10;
    int imax = -1;
    for(int i = 0; i<T->bb_tr_n; i++){
      if(T->bb_tr_p[i]>pmax){
	pmax = T->bb_tr_p[i];
	imax = i;
      }
    }
    E_i = pmax;
    // for(int i = 0; i<T->MC_nbbtracks; i++){
    //   if(T->MC_bbtrack_mid[i]==0 && T->MC_bbtrack_pid[i]==11){
    // 	E_i = T->MC_bbtrack_p[i];
    //   }
    // }
    
    //if no clean reference, continue;
    if(E_i==0 || imax<0)continue;

    /*
    for(int i = 0; i<T->bb_sh_nclus; i++){
      h1_EclusViClus_sh->Fill(i, T->bb_sh_clus_e_c[i]);
    }
    for(int i = 0; i<T->bb_sh_nclus; i++){
      h1_EclusViClus_ps->Fill(i, T->bb_ps_clus_e_c[i]);
    }
    A_i=T->bb_ps_clus_e_c[0]+T->bb_sh_clus_e_c[0];
    h1_DeltaE->Fill(1.-A_i/E_i);
    */
    
    //double x_tr = T->bb_tr_x[imax]+1.95715*T->bb_tr_th[imax];
    //double y_tr = T->bb_tr_y[imax]+1.95715*T->bb_tr_ph[imax];
    
    //cout << T->bb_tr_x[imax] << " " << T->bb_tr_th[imax] << " " << T->bb_tr_p[imax] << endl;
    
    //if no cluster, skip the event.
    if(T->bb_sh_nclus==0 || T->bb_ps_nclus==0)continue;
    
    
    int k_max = -1;
    double nblk = -1;
    double Emax = -10;
    // loop on clusters first: select highest energy
    for(int k = 0; k<T->bb_sh_nclus; k++){
      if(T->bb_sh_clus_e_c[k]>Emax){
	Emax = T->bb_sh_clus_e_c[k];
	k_max = k;
	nblk = T->bb_sh_clus_nblk[k];
      }
    }
    //reject events with max on the edge
    if(T->bb_sh_clus_row[k_max]==0 || T->bb_sh_clus_row[k_max]==26 ||
       T->bb_sh_clus_col[k_max]==0 || T->bb_sh_clus_col[k_max]==6)continue;
    
    for(int i = 0; i<nblk;i++){
      nevts[int(T->bb_sh_clus_blk_id[i])]++;
      A_j[int(T->bb_sh_clus_blk_id[i])]+=T->bb_sh_clus_blk_e[i];
    }
    
    k_max = -1;
    nblk = -1;
    Emax = -10;
    // loop on clusters first: select highest energy
    for(int k = 0; k<T->bb_ps_nclus; k++){
      if(T->bb_ps_clus_e_c[k]>Emax){
	Emax = T->bb_ps_clus_e_c[k];
	k_max = k;
	nblk = T->bb_ps_clus_nblk[k];
      }
    }

    for(int i = 0; i<nblk;i++){
      nevts[189+int(T->bb_ps_clus_blk_id[i])]++;
      A_j[189+int(T->bb_ps_clus_blk_id[i])]+=T->bb_ps_clus_blk_e[i];
    }
    
    
    //for(int i = 0; i<T->bb_sh_ngoodADChits; i++){
    //if(fabs(T->bb_sh_goodblock_x[i]-x_tr)<0.085*2 || fabs(T->bb_sh_goodblock_y[i]-y_tr)<0.085*2)
    //A_j[int(T->bb_sh_goodblock_id[i])]+=T->bb_sh_goodblock_e[i];
    //}

    //x_tr = T->bb_tr_x[imax]+1.75045*T->bb_tr_th[imax];
    //y_tr = T->bb_tr_y[imax]+1.75045*T->bb_tr_ph[imax];
    
    //for(int i = 0; i<T->bb_ps_ngoodADChits; i++){
    //if(fabs(T->bb_ps_goodblock_x[i]-x_tr)<0.09*2 || fabs(T->bb_sh_goodblock_y[i]-y_tr)<0.225)
    //A_j[189+int(T->bb_ps_goodblock_id[i])]+=T->bb_ps_goodblock_e[i];
    //}
    
    for(int j = 0; j<241; j++){
      B_j(j, 0)+= A_j[j]*E_i;
      for(int k = 0; k<241; k++){
	M_jk(j, k)+= A_j[j]*A_j[k];
      } 
    }
    
  }
  
  int k = 0;
  bool badcells[241];
  if(printstats>0){
    cout << "Printing counts per element: shower" << endl;
    for(int i = 0; i<27; i++){
      for(int j = 0; j<7; j++){
	cout << nevts[k] << "  ";
	k++;
      }
      cout << endl;
    }
    cout << endl;
    cout << "Printing counts per element: preshower" << endl;
    for(int i = 0; i<26; i++){
      for(int j = 0; j<2; j++){
	cout << nevts[k] << "  ";
	k++;
      }
      cout << endl;
    }
    cout << endl;
    
    if(printstats>1){
      cout << "Printing matrix elements: shower" << endl;
      for(int i = 0; i<27; i++){
	for(int j = 0; j<7; j++){
	  cout << M_jk(k,k) << " >=? "<< minMBratio*B_j(k, 0) << endl;
	  k++;
	}
      }
      cout << endl;  
      
      cout << "Printing matrix elements: preshower" << endl;
      for(int i = 0; i<26; i++){
	for(int j = 0; j<2; j++){
	  cout << M_jk(k,k) << " >=? "<< minMBratio*B_j(k, 0) << endl;
	  k++;
	}
      }
      cout << endl;   
    }
  }
  
  for(int j = 0; j<241; j++){
    badcells[j]=false;
    if(nevts[j]<Nmin || M_jk(j, j)< minMBratio*B_j(j, 0)){
      B_j(j, 0) = 1.;
      M_jk(j, j) = 1.;
      for(int k = 0; k<241; k++){
	if(k!=j){
	  M_jk(j, k) = 0.;
	  M_jk(k, j) = 0.;
	}
      }
      badcells[j]=true;
    }
  }
  
  //B_j.Print();
  
  //M_jk.Print();
  
  TMatrixD M_jk_inv = M_jk.Invert();
  TMatrixD C_j = M_jk_inv*B_j;
  
  int nsh = 0;
  int nps = 0;
  double avg_sh = 0;
  double avg_ps = 0;
  for(int k = 0; k<189; k++){
    if(!badcells[k]){
      nsh++;
      avg_sh+= C_j(k, 0);
    }
  }
  avg_sh/= nsh;
  for(int k = 189; k<241; k++){
    if(!badcells[k]){
      nps++;
      avg_ps+= C_j(k, 0);
    }
  }
  avg_ps/= nps;
  
  
  cout << endl;
  k = 0;
  //C_j.Print();
  cout << " *** add the following lines in your bigbite shower database *** " << endl;
  cout << endl;
  cout << "bb.sh.adc.gain = " << endl;
  for(int i = 0; i<27; i++){
    for(int j = 0; j<7; j++){
      if(badcells[k]){
	cout << avg_sh << "  ";
      }else{
	cout << C_j(k, 0) << "  ";
      }
      k++;
    }
    cout << endl;
  }
  cout << endl;
  
  cout << " *** add the following lines in your bigbite preshower database *** " << endl;
  cout << endl;
  cout << "bb.ps.adc.gain = " << endl;
  for(int i = 0; i<26; i++){
    for(int j = 0; j<2; j++){
      if(badcells[k]){
	cout << avg_ps << "  ";
      }else{
	cout << C_j(k, 0) << "  ";
      }
      k++;
    }
    cout << endl;
  }
  cout << endl;
  
  //from hcal script: Waht would Etarg be???
  //xErr[i] = E_targ*sqrt(fabs(M(i,i)));
  k = 0;
  cout << "Shower calibration coefficients with errors= " << endl;
  for(int i = 0; i<27; i++){
    for(int j = 0; j<7; j++){
      if(badcells[k]){
	cout << avg_sh << " +- " << 1.0 << endl;
      }else{
	cout << C_j(k, 0) << " +- "<< sqrt(fabs(M_jk(k,k))) << endl;
      }
      k++;
    }
  }
  cout << endl;  

  cout << "PreShower calibration coefficients with errors= " << endl;
  for(int i = 0; i<26; i++){
    for(int j = 0; j<2; j++){
      if(badcells[k]){
	cout << avg_ps << " +- " << 1.0 << endl;
      }else{
	cout << C_j(k, 0) << " +- "<< sqrt(fabs(M_jk(k,k))) << endl;
      }
      k++;
    }
  }
  cout << endl;  
  /*
  TCanvas* C0 = new TCanvas();
  h1_DeltaE->Draw();
  TCanvas* C1 = new TCanvas("C1", "C1", 800, 400);
  C1->Divide(2, 1);
  C1->cd(1);
  h1_EclusViClus_ps->Draw("colz");
  C1->cd(2);
  h1_EclusViClus_sh->Draw("colz");
  */
}
