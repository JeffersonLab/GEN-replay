#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <iostream>
#include <fstream>

void MakeOdef_and_cfg_files( const char *configfilename ){ 
  ifstream configfile(configfilename);

  TString detname="bb.gem";
  TString detname_nodots;

  int nmodules=12;
  int nlayers=5;
  
  double maxlayersizeX = 2.2; //meters
  double maxlayersizeY = 0.75; //meters 

  double maxTrackXp = 1.2;
  double maxTrackYp = 0.4; 
  
  vector<int> mod_nstripu,mod_nstripv;
  vector<double> mod_xsize, mod_ysize;
  vector<double> mod_x0, mod_y0, mod_z0;
  vector<TString> modname;
  vector<TString> modname_nodots;

  vector<TString> moddesc;
  
  TString currentline;

  int maxstrips = 100, maxclust=50;
  
  while( currentline.ReadLine( configfile ) ){
    if( !currentline.BeginsWith("#") ){
      TObjArray *tokens = currentline.Tokenize( " " );

      if( tokens->GetEntries() >= 2 ){
	TString skey = ( (TObjString*) (*tokens)[0] )->GetString();

	if( skey == "maxstrips" ){ //set upper limit of strip multiplicity histogram:
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	  maxstrips = stemp.Atoi();
	}

	if( skey == "maxclust" ){ //set upper limit of cluster multiplicity histogram:
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	  maxclust = stemp.Atoi();
	}
	
	if( skey == "name" ){
	  detname = ( (TObjString*) (*tokens)[1] )->GetString();
	}

	if( skey == "nmodules" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();

	  nmodules = stemp.Atoi();
	  mod_nstripu.resize( nmodules );
	  mod_nstripv.resize( nmodules );
	  mod_xsize.resize( nmodules );
	  mod_ysize.resize( nmodules );
	  mod_x0.resize( nmodules );
	  mod_y0.resize( nmodules );
	  mod_z0.resize( nmodules );

	  for( int i=0; i<nmodules; i++ ){
	    TString nametemp = detname;

	    modname.push_back( nametemp.Format( "%s.m%d", nametemp.Data(), i ) );
	    
	    nametemp.ReplaceAll(".","_");

	    modname_nodots.push_back( nametemp.Format("%s_m%d", nametemp.Data(), i ) );
	    
	    moddesc.push_back( modname[i] ); //default mod description to mod name
	  }
	}

	if( skey == "moddesc" ){ //override module descriptions (for config file):

	  TString moddescstring = currentline;
	  moddescstring.ReplaceAll("moddesc","");

	  TObjArray *moddesc_tokens = moddescstring.Tokenize(";"); 

	  if( moddesc_tokens->GetEntries() >= nmodules ){
	    for( int i=0; i<nmodules; i++ ){
	      moddesc[i] = ( ( TObjString*) (*moddesc_tokens)[i] )->GetString();
	      moddesc[i].Remove(TString::kBoth, ' '); //this should remove leading and trailing spaces

	      cout << "\"" << moddesc[i] << "\"" << endl;
	      
	    }
	  }
	}
	
	if( skey == "nlayers" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();

	  nlayers = stemp.Atoi();
	}

	if( skey == "layersizeX" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();

	  maxlayersizeX = stemp.Atof();
	}

	if( skey == "layersizeY" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();

	  maxlayersizeY = stemp.Atof();
	}

	if( skey == "maxtrackxp" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();

	  maxTrackXp = stemp.Atof();
	}

	if( skey == "maxtrackyp" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();

	  maxTrackYp = stemp.Atof();
	}
	
	if( skey == "mod_nstripu" && tokens->GetEntries() >= nmodules+1 ){
	  for( int i=1; i<=nmodules; i++ ){
	    TString sn = ( (TObjString*) (*tokens)[i] )->GetString();

	    int mod = i-1;
	    
	    mod_nstripu[mod] = sn.Atoi();
	    
	  }
	}

	if( skey == "mod_nstripv" && tokens->GetEntries() >= nmodules + 1){
	  for( int i=1; i<=nmodules; i++ ){
	    TString sn = ( (TObjString*) (*tokens)[i] )->GetString();

	    int mod = i-1;
	    
	    mod_nstripv[mod] = sn.Atoi();
	    
	  }
	}

	if( skey == "mod_xsize" && tokens->GetEntries() >= nmodules + 1 ){
	  for( int i=1; i<=nmodules; i++ ){
	    TString sn = ( (TObjString*) (*tokens)[i] )->GetString();

	    int mod = i-1;
	    
	    mod_xsize[mod] = sn.Atof();
	    
	  }
	}

	if( skey == "mod_ysize" && tokens->GetEntries() >= nmodules + 1 ){
	  for( int i=1; i<=nmodules; i++ ){
	    TString sn = ( (TObjString*) (*tokens)[i] )->GetString();

	    int mod = i-1;
	    
	    mod_ysize[mod] = sn.Atof();
	    
	  }
	}

	if( skey == "mod_x0" && tokens->GetEntries() >= nmodules + 1 ){
	  for( int i=1; i<=nmodules; i++ ){
	    TString sn = ( (TObjString*) (*tokens)[i] )->GetString();

	    int mod = i-1;
	    
	    mod_x0[mod] = sn.Atof();
	    
	  }
	}


	if( skey == "mod_y0" && tokens->GetEntries() >= nmodules + 1 ){
	  for( int i=1; i<=nmodules; i++ ){
	    TString sn = ( (TObjString*) (*tokens)[i] )->GetString();

	    int mod = i-1;
	    
	    mod_y0[mod] = sn.Atof();
	    
	  }
	}

	if( skey == "mod_z0" && tokens->GetEntries() >= nmodules + 1 ){
	  for( int i=1; i<=nmodules; i++ ){
	    TString sn = ( (TObjString*) (*tokens)[i] )->GetString();

	    int mod = i-1;
	    
	    mod_z0[mod] = sn.Atof();
	    
	  }
	}	
      }
    } 
  }

  detname_nodots = detname;
  detname_nodots.ReplaceAll(".","_");
  
  TString odef_fname;
  odef_fname.Form( "replay_%s.odef", detname_nodots.Data() );

  ofstream odef_file(odef_fname.Data());


  TString cfg_fname;
  cfg_fname.Form( "%s.cfg", detname_nodots.Data() );

  ofstream cfg_file(cfg_fname.Data());

  TString cfg_fname_lowlevel;
  cfg_fname_lowlevel.Form("%s_basic.cfg", detname_nodots.Data() );

  ofstream cfg_file_lowlevel(cfg_fname_lowlevel.Data() );
  
  TString odefline;

  vector<TString> treevarnames;
  treevarnames.push_back( "track.*" );
  treevarnames.push_back( "hit.*" );
  treevarnames.push_back( "nlayershit" );
  treevarnames.push_back( "nlayershitu" );
  treevarnames.push_back( "nlayershitv" );
  treevarnames.push_back( "nlayershituv" );
  treevarnames.push_back( "nstripsu_layer" );
  treevarnames.push_back( "nstripsv_layer" );
  treevarnames.push_back( "nclustu_layer" );
  treevarnames.push_back( "nclustv_layer" );
  treevarnames.push_back( "n2Dhit_layer" );
  treevarnames.push_back( "*.time.T0_by_APV" );
  treevarnames.push_back( "*.time.Tref_coarse" );
  treevarnames.push_back( "*.time.Tcoarse_by_APV" );
  treevarnames.push_back( "*.time.Tfine_by_APV" );
  treevarnames.push_back( "*.time.EventCount_by_APV" );
  treevarnames.push_back( "*.time.T_ns_by_APV" );
  treevarnames.push_back( "*.strip.nstripsfired" );
  treevarnames.push_back( "*.strip.nstrips_keep" );
  treevarnames.push_back( "*.strip.nstrips_keep_lmax" );
  treevarnames.push_back( "*.strip.nstrips_keepU" );
  treevarnames.push_back( "*.strip.nstrips_keepV" );
  treevarnames.push_back( "*.strip.nstrips_keep_lmaxU" );
  treevarnames.push_back( "*.strip.nstrips_keep_lmaxV" );
  treevarnames.push_back( "*.clust.nclustu" );
  treevarnames.push_back( "*.clust.nclustv" );
  treevarnames.push_back( "*.clust.nclustu_tot" );
  treevarnames.push_back( "*.clust.nclustv_tot" );


  odef_file << "#tree variables: " << endl;
  
  for( int i=0; i<treevarnames.size(); i++ ){
    if( treevarnames[i].Contains("*") ){ //assume a block of variables:
      odefline.Form( "block %s.%s", detname.Data(), treevarnames[i].Data() );
    } else {
      odefline.Form( "variable %s.%s", detname.Data(), treevarnames[i].Data() );
    }

    if( odefline.Contains("*.time") ) odefline.Prepend("#");
    
    odef_file << odefline << endl;
  }

  odef_file << endl;
  //don't worry about the 
  // vector<TString> cutnames just yet, we can add those manually

  odef_file << "#Cuts: " << endl;
  TString trackcut;
  trackcut.Form( "cut singletrack %s.track.ntrack==1", detname.Data() );
  
  odef_file << trackcut << endl;
  odef_file << trackcut.Format( "cut anytrack %s.track.ntrack>0", detname.Data() ) << endl << endl;
  
  odef_file << "# Histogram definitions: " << endl;
  odef_file << "# layers fired, strip and cluster multiplicities" << endl;

  TString histdef;

  
  odef_file << histdef.Format( "th1d h%s_Nlayers_hit ';Number of layers with fired strips (u/x OR v/y);' %s.nlayershit %d %g %g",
			       detname_nodots.Data(), detname.Data(), nlayers+1, -0.5, nlayers+0.5 ) << endl;
  odef_file << histdef.Format( "th1d h%s_Nlayers_hitu ';Number of layers with fired U/X strips;' %s.nlayershitu %d %g %g",
			       detname_nodots.Data(), detname.Data(), nlayers+1, -0.5, nlayers+0.5 ) << endl;

  odef_file << histdef.Format( "th1d h%s_Nlayers_hitv ';Number of layers with fired V/Y strips;' %s.nlayershitv %d %g %g",
			       detname_nodots.Data(), detname.Data(), nlayers+1, -0.5, nlayers+0.5 ) << endl;
  odef_file << histdef.Format( "th1d h%s_Nlayers_hituv ';Number of layers with 2D hits;' %s.nlayershituv %d %g %g",
			       detname_nodots.Data(), detname.Data(), nlayers+1, -0.5, nlayers+0.5 ) << endl << endl;


  
  odef_file << histdef.Format( "th2d h%s_NstripsU_layer ';layer;Number of U/X strips fired' [I] %s.nstripsu_layer[I] %d %g %g %d -0.5 %g",
			       detname_nodots.Data(), detname.Data(), nlayers, -0.5, nlayers-.5, maxstrips+1, maxstrips+0.5) << endl;
  odef_file << histdef.Format( "th2d h%s_NstripsV_layer ';layer;Number of V/Y strips fired' [I] %s.nstripsv_layer[I] %d %g %g %d -0.5 %g",
			       detname_nodots.Data(), detname.Data(), nlayers, -0.5, nlayers-0.5, maxstrips+1, maxstrips+0.5 ) << endl;


  odef_file << histdef.Format( "th2d h%s_NclustU_layer ';layer;Number of U/X clusters' [I] %s.nclustu_layer[I] %d %g %g %d -0.5 %g",
			       detname_nodots.Data(), detname.Data(), nlayers, -0.5, nlayers-0.5, maxclust+1, maxclust+0.5 ) << endl;
  odef_file << histdef.Format( "th2d h%s_NclustV_layer ';layer;Number of V/Y clusters' [I] %s.nclustv_layer[I] %d %g %g %d -0.5 %g",
			       detname_nodots.Data(), detname.Data(), nlayers, -0.5, nlayers-0.5, maxclust+1, maxclust+0.5 ) << endl;
  odef_file << histdef.Format( "th2d h%s_Nclust2D_layer ';layer;Number of 2D hits' [I] %s.n2Dhit_layer[I] %d %g %g %d -0.5 %g",
			       detname_nodots.Data(), detname.Data(), nlayers, -0.5, nlayers-0.5, maxclust+1, maxclust+0.5 ) << endl << endl;


  odef_file << histdef.Format( "th1d h%s_clustwidthU 'U/X clusters on tracks; cluster size in strips' %s.hit.nstripu 10 0.5 10.5 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_clustwidthV 'V/Y clusters on tracks; cluster size in strips' %s.hit.nstripv 10 0.5 10.5 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th2d h%s_clustwidthVvsU '2D hits on tracks; U/X cluster size ; V/Y cluster size' %s.hit.nstripu %s.hit.nstripv 10 0.5 10.5 10 0.5 10.5 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data() ) << endl;


  odef_file << histdef.Format( "th2d h%s_clustwidthU_vs_module ';module;U/X cluster size' %s.hit.module %s.hit.nstripu %d -0.5 %g 10 0.5 10.5 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nmodules, nmodules-0.5 ) << endl;
  odef_file << histdef.Format( "th2d h%s_clustwidthV_vs_module ';module;V/Y cluster size' %s.hit.module %s.hit.nstripv %d -0.5 %g 10 0.5 10.5 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nmodules, nmodules-0.5 ) << endl;
  
  
  odef_file << histdef.Format( "th1d h%s_clust_Utime ';U/X cluster time (ns);' %s.hit.Utime 150 0 150 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_clust_Vtime ';V/Y cluster time (ns);' %s.hit.Vtime 150 0 150 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th2d h%s_clust_UVtime ';U/X cluster time (ns); V/Y cluster time (ns)' %s.hit.Utime %s.hit.Vtime 150 0 150 150 0 150 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data() ) << endl;

  odef_file << histdef.Format( "th1d h%s_clust_deltat ';t_{U/X} - t_{V/Y} (ns);' %s.hit.deltat 200 -50 50 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th2d h%s_clust_deltat_vs_tavg ';t_{avg} (ns); t_{diff} (ns)' %s.hit.Tavg %s.hit.deltat 150 0 150 200 -50 50 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data() ) << endl;
  
  odef_file << histdef.Format( "th1d h%s_iSampMaxU ';Peak time sample, U/X strips;' %s.hit.isampmaxUclust 6 -0.5 5.5",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_iSampMaxV ';Peak time sample, V/Y strips;' %s.hit.isampmaxVclust 6 -0.5 5.5",
			       detname_nodots.Data(), detname.Data() ) << endl;
  
  odef_file << histdef.Format( "th1d h%s_CorrCoeff_clust ';Corr. Coeff., cluster sums;' %s.hit.ccor_clust 202 -1.01 1.01 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_CorrCoeff_maxstrip ';Corr. Coeff., max strips;' %s.hit.ccor_strip 202 -1.01 1.01 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;

  odef_file << histdef.Format( "th1d h%s_ADCasym ';(ADCU-ADCV)/(ADCU+ADCV)' %s.hit.ADCasym 202 -1.01 1.01 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl << endl;

  odef_file << histdef.Format( "th1d h%s_ADCavg 'hits on tracks ;(ADCU+ADCV)/2;' %s.hit.ADCavg 1500 0 30000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th2d h%s_ADCasym_vs_ADCavg ';ADC avg;ADC asym.' %s.hit.ADCavg %s.hit.ADCasym 250 0 25000 250 -1.01 1.01",
			       detname_nodots.Data(), detname.Data(), detname.Data() ) << endl;
  
  odef_file << histdef.Format( "th1d h%s_ADCU_clust ';ADC cluster sum (U/X strips);' %s.hit.ADCU 1500 0 30000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_ADCV_clust ';ADC cluster sum (V/Y strips);' %s.hit.ADCV 1500 0 30000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th2d h%s_ADCVvsU_clust ';ADC cluster sum (U/X); ADC cluster sum (V/Y)' %s.hit.ADCU %s.hit.ADCV 200 0 30000 200 0 30000 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data() ) << endl;

  odef_file << histdef.Format( "th2d h%s_ADCavg_vs_module ';module; Cluster ADC avg' %s.hit.module %s.hit.ADCavg %d -0.5 %g 250 0 25000",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nmodules, nmodules-0.5 ) << endl;
  
  odef_file << histdef.Format( "th1d h%s_ADCU_maxstrip ';Max strip ADC sum (U/X strips);' %s.hit.ADCmaxstripU 1500 0 15000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_ADCV_maxstrip ';Max strip ADC sum (V/Y strips);' %s.hit.ADCmaxstripV 1500 0 15000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th2d h%s_ADCVvsU_maxstrip ';Max strip ADC sum (U/X); Max strip ADC sum (V/Y)' %s.hit.ADCmaxstripU %s.hit.ADCmaxstripV 200 0 12000 200 0 12000 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data() ) << endl;



  odef_file << histdef.Format( "th1d h%s_ADCU_maxsample ';U/X max strip max sample;' %s.hit.ADCmaxsampU 500 0 4000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_ADCV_maxsample ';V/Y max strip max sample;' %s.hit.ADCmaxsampV 500 0 4000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th2d h%s_ADCVvsU_maxsample ';Max strip max sample (U/X);Max strip max sample (V/Y)' %s.hit.ADCmaxsampU %s.hit.ADCmaxsampV 200 0 4000 200 0 4000 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data() ) << endl;


  odef_file << histdef.Format( "th1d h%s_ADCU_maxclustsample ';Max cluster-summed U/X sample;' %s.hit.ADCmaxsampUclust 500 0 8000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_ADCV_maxclustsample ';Max cluster-summed V/Y sample;' %s.hit.ADCmaxsampVclust 500 0 8000 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th2d h%s_ADCVvsU_maxclustsample ';Max cluster-summed sample (U/X);Max cluster-summed sample (V/Y)' %s.hit.ADCmaxsampUclust %s.hit.ADCmaxsampVclust 200 0 8000 200 0 8000 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data() ) << endl << endl;
  
  
  //Now put together the track stuff:

  odef_file << histdef.Format( "th1d h%s_Ntracks ';Number of tracks found;' %s.track.ntrack 11 -0.5 10.5",
			       detname_nodots.Data(), detname.Data() ) << endl;
  
  odef_file << histdef.Format( "th1d h%s_TrackNhits ';Number of hits on first/best track;' %s.track.nhits[I] %d -0.5 %g singletrack",
			       detname_nodots.Data(), detname.Data(), nlayers+1, nlayers+0.5 ) << endl;
  odef_file << histdef.Format( "th1d h%s_TrackX ';Best track X(z=0), m;' %s.track.x[I] 200 %g %g singletrack",
			       detname_nodots.Data(), detname.Data(), -maxlayersizeX/2.0, maxlayersizeX/2.0 ) << endl;
  odef_file << histdef.Format( "th1d h%s_TrackY ';Best track Y(z=0), m;' %s.track.y[I] 200 %g %g singletrack",
			       detname_nodots.Data(), detname.Data(), -maxlayersizeY/2.0, maxlayersizeY/2.0 ) << endl;

  odef_file << histdef.Format( "th1d h%s_TrackXp ';Best track dx/dz;' %s.track.xp[I] 200 %g %g singletrack",
			       detname_nodots.Data(), detname.Data(), -maxTrackXp, maxTrackXp ) << endl;
  odef_file << histdef.Format( "th1d h%s_TrackYp ';Best track dy/dz;' %s.track.yp[I] 200 %g %g singletrack",
			       detname_nodots.Data(), detname.Data(), -maxTrackYp, maxTrackYp) << endl;
  
  odef_file << histdef.Format( "th2d h%s_TrackXY 'Best track; y(m); x(m)' %s.track.y[I] %s.track.x[I] 150 %g %g 150 %g %g singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), -maxlayersizeY/2.0, maxlayersizeY/2.0, -maxlayersizeX/2.0, maxlayersizeX/2.0 ) << endl;

  odef_file << histdef.Format( "th2d h%s_TrackXpYp 'Best track ;dy/dz;dx/dz' %s.track.yp[I] %s.track.xp[I] 150 %g %g 150 %g %g singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), -maxTrackYp, maxTrackYp, -maxTrackXp, maxTrackXp ) << endl;

  odef_file << histdef.Format( "th1d h%s_TrackChi2NDF ';Best track chi2/ndf;' %s.track.chi2ndf[I] 150 0 40 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl << endl;


  //what is next: tracking residuals inclusive and exclusive, all hits:
  odef_file << histdef.Format( "th1d h%s_residu_allhits 'All hits; Track u/x incl. residuals (m);' %s.hit.residu 200 -0.002 0.002 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_residv_allhits 'All hits; Track v/y incl. residuals (m);' %s.hit.residv 200 -0.002 0.002 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;

  odef_file << histdef.Format( "th1d h%s_eresidu_allhits 'All hits; Track u/x excl. residuals (m);' %s.hit.eresidu 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;
  odef_file << histdef.Format( "th1d h%s_eresidv_allhits 'All hits; Track v/y excl. residuals (m);' %s.hit.eresidv 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data() ) << endl;

  //inclusive tracking residuals by layer and module:
  odef_file << histdef.Format( "th2d h%s_residu_vs_layer ';layer; Track u/x incl. residuals (m)' %s.hit.layer %s.hit.residu %d -0.5 %g 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nlayers, nlayers - 0.5 ) << endl;
  odef_file << histdef.Format( "th2d h%s_residv_vs_layer ';layer; Track v/y incl. residuals (m)' %s.hit.layer %s.hit.residv %d -0.5 %g 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nlayers, nlayers - 0.5 ) << endl;

  odef_file << histdef.Format( "th2d h%s_residu_vs_module ';module; Track u/x incl. residuals (m)' %s.hit.module %s.hit.residu %d -0.5 %g 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nmodules, nmodules - 0.5 ) << endl;
  odef_file << histdef.Format( "th2d h%s_residv_vs_module ';module; Track v/y incl. residuals (m)' %s.hit.module %s.hit.residv %d -0.5 %g 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nmodules, nmodules - 0.5 ) << endl;


  //exclusive tracking residuals by layer and module:
  odef_file << histdef.Format( "th2d h%s_eresidu_vs_layer ';layer; Track u/x excl. residuals (m)' %s.hit.layer %s.hit.eresidu %d -0.5 %g 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nlayers, nlayers - 0.5 ) << endl;
  odef_file << histdef.Format( "th2d h%s_eresidv_vs_layer ';layer; Track v/y excl. residuals (m)' %s.hit.layer %s.hit.eresidv %d -0.5 %g 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nlayers, nlayers - 0.5 ) << endl;

  odef_file << histdef.Format( "th2d h%s_eresidu_vs_module ';module; Track u/x excl. residuals (m)' %s.hit.module %s.hit.eresidu %d -0.5 %g 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nmodules, nmodules - 0.5 ) << endl;
  odef_file << histdef.Format( "th2d h%s_eresidv_vs_module ';module; Track v/y excl. residuals (m)' %s.hit.module %s.hit.eresidv %d -0.5 %g 250 -0.0025 0.0025 singletrack",
			       detname_nodots.Data(), detname.Data(), detname.Data(), nmodules, nmodules - 0.5 ) << endl;

 
  // //Next up: hit maps by layer/module and strip heat maps/ADC distributions by module:
  // odef_file << histdef.Format( "th2d h%s_hit_xglobal_vs_layer ';layer; Hit x global (m)' %s.hit.layer %s.hit.xglobal %d -0.5 %g 250 %g %g",
  // 			       detname_nodots.Data(), detname.Data(), detname.Data(), nlayers, nlayers-0.5, -maxlayersizeX/2.0, maxlayersizeX/2.0 ) << endl;
  // odef_file << histdef.Format( "th2d h%s_hit_yglobal_vs_layer ';layer; Hit y global (m)' %s.hit.layer %s.hit.yglobal %d -0.5 %g 250 %g %g",
  // 			       detname_nodots.Data(), detname.Data(), detname.Data(), nlayers, nlayers-0.5, -maxlayersizeY/2.0, maxlayersizeY/2.0 ) << endl << endl;


  odef_file << endl << "# Module-specific histograms: " << endl;
    
  
  //All module-specific histograms defined in one loop:
  for( int i=0; i<nmodules; i++ ){
    
    odef_file << "# Module " << i << " strip histograms, all strips: " << endl;
    //Strip counts, require track cut:
    odef_file << histdef.Format( "th1d h%s_nstripstot_good '%s; Num. fired strips (U/X) + (V/Y);' %s.strip.nstripsfired %d -0.5 %g %s.ontrack",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), mod_nstripu[i]+mod_nstripv[i]+1, mod_nstripu[i]+mod_nstripv[i]+0.5, modname[i].Data() ) << endl;

    odef_file << histdef.Format( "th1d h%s_nclustU_good '%s; num. U clusters;' %s.clust.nclustu %d -0.5 %g %s.ontrack",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), maxclust+1, maxclust+0.5, modname[i].Data() ) << endl;
    
    odef_file << histdef.Format( "th1d h%s_nclustV_good '%s; num. V clusters; ' %s.clust.nclustv %d -0.5 %g %s.ontrack",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), maxclust+1, maxclust+0.5, modname[i].Data() ) << endl;
    
    
    odef_file << histdef.Format( "th1d h%s_nstripstot_all '%s; Num. fired strips (U/X) + (V/Y);' %s.strip.nstripsfired %d -0.5 %g",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), mod_nstripu[i]+mod_nstripv[i]+1, mod_nstripu[i]+mod_nstripv[i]+0.5 ) << endl;
    
    odef_file << histdef.Format( "th1d h%s_nclustU_all '%s; num. U clusters;' %s.clust.nclustu %d -0.5 %g",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), maxclust+1, maxclust+0.5 ) << endl;

    odef_file << histdef.Format( "th1d h%s_nclustV_all '%s; num. V clusters;' %s.clust.nclustv %d -0.5 %g",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), maxclust+1, maxclust+0.5 ) << endl;
    
    //No cuts for strip heat map:
    odef_file << histdef.Format( "th1d h%s_stripU_all '%s; U/X strip index; ' %s.strip.istrip[I] %d -0.5 %g %s.strip.IsU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), mod_nstripu[i], mod_nstripu[i]-0.5, modname[i].Data() ) << endl;
    //No cuts for strip heat map:
    odef_file << histdef.Format( "th1d h%s_stripV_all '%s; V/Y strip index; ' %s.strip.istrip[I] %d -0.5 %g %s.strip.IsV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), mod_nstripv[i], mod_nstripv[i]-0.5, modname[i].Data() ) << endl;

    

    //1D histograms for strip T values and ADC values:
    odef_file << histdef.Format( "th1d h%s_ADCsumU_all '%s; Strip ADC sum, U/X strips;' %s.strip.ADCsum[I] 1000 0 15000 %s.strip.IsU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_ADCsumV_all '%s; Strip ADC sum, V/Y strips;' %s.strip.ADCsum[I] 1000 0 15000 %s.strip.IsV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_ADCmaxU_all '%s; Strip ADC max, U/X strips;' %s.strip.ADCmax[I] 500 0 4000 %s.strip.IsU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_ADCmaxV_all '%s; Strip ADC max, V/Y strips; ' %s.strip.ADCmax[I] 500 0 4000 %s.strip.IsV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_TimeU_all '%s; Strip Time, U/X strips;' %s.strip.Tmean[I] 150 0 150 %s.strip.IsU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_TimeV_all '%s; Strip Time, V/Y strips;' %s.strip.Tmean[I] 150 0 150 %s.strip.IsV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_iSampMaxU_all '%s; Peak time sample, U/X strips;' %s.strip.isampmax[I] 6 -0.5 5.5 %s.strip.IsU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_iSampMaxV_all '%s; Peak time sample, V/Y strips;' %s.strip.isampmax[I] 6 -0.5 5.5 %s.strip.IsV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;

    //ADC sum dist. vs strip:
    odef_file << histdef.Format( "#th2d h%s_ADCsum_vs_Ustrip_all '%s; U strip ADC sum vs strip;' %s.strip.istrip[I] %s.strip.ADCsum[I] %d -0.5 %g 250 0 12000 %s.strip.IsU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), mod_nstripu[i], mod_nstripu[i]-0.5, modname[i].Data() ) << endl;
    odef_file << histdef.Format( "#th2d h%s_ADCsum_vs_Vstrip_all '%s; V strip ADC sum vs strip;' %s.strip.istrip[I] %s.strip.ADCsum[I] %d -0.5 %g 250 0 12000 %s.strip.IsV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), mod_nstripv[i], mod_nstripv[i]-0.5, modname[i].Data() ) << endl;

    //ADC max dist. vs strip:
    odef_file << histdef.Format( "#th2d h%s_ADCmax_vs_Ustrip_all '%s; U strip ADC max vs strip;' %s.strip.istrip[I] %s.strip.ADCmax[I] %d -0.5 %g 250 0 4000 %s.strip.IsU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), mod_nstripu[i], mod_nstripu[i]-0.5, modname[i].Data() ) << endl;
    odef_file << histdef.Format( "#th2d h%s_ADCmax_vs_Vstrip_all '%s; V strip ADC max vs strip;' %s.strip.istrip[I] %s.strip.ADCmax[I] %d -0.5 %g 250 0 4000 %s.strip.IsV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), mod_nstripv[i], mod_nstripv[i]-0.5, modname[i].Data() ) << endl;
    

    odef_file << endl << "# Module " << i << " strip histograms, strips on tracks: " << endl;
    //Strips on tracks:
    odef_file << histdef.Format( "th1d h%s_stripU_good '%s (strips on tracks); U/X strip index;' %s.strip.istrip[I] %d -0.5 %g %s.strip.ontrackU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), mod_nstripu[i], mod_nstripu[i]-0.5, modname[i].Data() ) << endl;
    
    odef_file << histdef.Format( "th1d h%s_stripV_good '%s (strips on tracks); V/Y strip index;' %s.strip.istrip[I] %d -0.5 %g %s.strip.ontrackV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), mod_nstripv[i], mod_nstripv[i]-0.5, modname[i].Data() ) << endl;

    //1D histograms for strip T values and ADC values:
    odef_file << histdef.Format( "th1d h%s_ADCsumU_good '%s; Strip ADC sum, U/X strips;' %s.strip.ADCsum[I] 1000 0 15000 %s.strip.ontrackU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_ADCsumV_good '%s; Strip ADC sum, V/Y strips;' %s.strip.ADCsum[I] 1000 0 15000 %s.strip.ontrackV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_ADCmaxU_good '%s; Strip ADC max, U/X strips;' %s.strip.ADCmax[I] 500 0 4000 %s.strip.ontrackU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_ADCmaxV_good '%s; Strip ADC max, V/Y strips;' %s.strip.ADCmax[I] 500 0 4000 %s.strip.ontrackV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_TimeU_good '%s; Strip Time, U/X strips;' %s.strip.Tmean[I] 150 0 150 %s.strip.ontrackU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_TimeV_good '%s; Strip Time, V/Y strips;' %s.strip.Tmean[I] 150 0 150 %s.strip.ontrackV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_iSampMaxU_good '%s; Peak time sample, U/X strips;' %s.strip.isampmax[I] 6 -0.5 5.5 %s.strip.ontrackU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_iSampMaxV_good '%s; Peak time sample, V/Y strips;' %s.strip.isampmax[I] 6 -0.5 5.5 %s.strip.ontrackV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;

    //ADC sum dist. vs strip:
    odef_file << histdef.Format( "#th2d h%s_ADCsum_vs_Ustrip_good '%s; U strip ADC sum vs strip;' %s.strip.istrip[I] %s.strip.ADCsum[I] %d -0.5 %g 250 0 12000 %s.strip.ontrackU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), mod_nstripu[i], mod_nstripu[i]-0.5, modname[i].Data() ) << endl;
    odef_file << histdef.Format( "#th2d h%s_ADCsum_vs_Vstrip_good '%s; V strip ADC sum vs strip;' %s.strip.istrip[I] %s.strip.ADCsum[I] %d -0.5 %g 250 0 12000 %s.strip.ontrackV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), mod_nstripv[i], mod_nstripv[i]-0.5, modname[i].Data() ) << endl;

    //ADC max dist. vs strip:
    odef_file << histdef.Format( "#th2d h%s_ADCmax_vs_Ustrip_good '%s; U strip ADC max vs strip;' %s.strip.istrip[I] %s.strip.ADCmax[I] %d -0.5 %g 250 0 4000 %s.strip.ontrackU[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), mod_nstripu[i], mod_nstripu[i]-0.5, modname[i].Data() ) << endl;
    odef_file << histdef.Format( "#th2d h%s_ADCmax_vs_Vstrip_good '%s; V strip ADC max vs strip;' %s.strip.istrip[I] %s.strip.ADCmax[I] %d -0.5 %g 250 0 4000 %s.strip.ontrackV[I]",
				 modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), mod_nstripv[i], mod_nstripv[i]-0.5, modname[i].Data() ) << endl;
    
    
    odef_file << endl << "# Module " << i << " hit maps " << endl;

    
    odef_file << histdef.Format( "#th1d h%s_hit_xlocal 'Hit local X position (m), Module %d' %s.hit.hitx[I] 250 %g %g %s.hit.ontrack[I]",
				 modname_nodots[i].Data(), i, modname[i].Data(), -mod_xsize[i]/2.0, mod_xsize[i]/2.0, modname[i].Data() ) << endl;
    odef_file << histdef.Format( "#th1d h%s_hit_ylocal 'Hit local Y position (m), Module %d' %s.hit.hity[I] 250 %g %g %s.hit.ontrack[I]",
				 modname_nodots[i].Data(), i, modname[i].Data(), -mod_ysize[i]/2.0, mod_ysize[i]/2.0, modname[i].Data() ) << endl;
    odef_file << histdef.Format( "#th2d h%s_hit_xylocal 'Hit local X vs Y (m), Module %d' %s.hit.hity[I] %s.hit.hitx[I] 200 %g %g 200 %g %g %s.hit.ontrack[I]",
				 modname_nodots[i].Data(), i, modname[i].Data(), modname[i].Data(), -mod_ysize[i]/2.0, mod_ysize[i]/2.0, -mod_xsize[i]/2.0, mod_xsize[i]/2.0,
				 modname[i].Data() ) << endl;


    //Let's also make some histograms with CM_OR variables:
    odef_file << endl;
    odef_file << histdef.Format("th1d h%s_ADCmax_good_CM '%s; Strip ADC max; ' %s.strip.ADCmax[I] 500 0 4000 %s.strip.CM_GOOD[I]",
    				modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format("th1d h%s_ADCsum_good_CM '%s; Strip ADC sum; ' %s.strip.ADCsum[I] 1000 0 15000 %s.strip.CM_GOOD[I]",
    				modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format("th1d h%s_iSampMax_good_CM '%s; Strip max time sample;' %s.strip.isampmax[I] 6 -0.5 5.5 %s.strip.CM_GOOD[I]",
    				modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format("th1d h%s_StripTime_good_CM '%s; Strip Mean Time (ns);' %s.strip.Tmean[I] 150 0 150 %s.strip.CM_GOOD[I]",
    				modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format("th1d h%s_StripTsigma_good_CM '%s; Strip RMS Time (ns);' %s.strip.Tsigma[I] 150 0 150 %s.strip.CM_GOOD[I]",
    				modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    
    //New timing based histograms: 
    odef_file << endl;

    odef_file << histdef.Format( "th1d h%s_strip_Tdiff '%s; Tstrip - Tclust (ns);' %s.strip.Tdiff[I] 200 -40 40 %s.strip.ontrack[I]",
      modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th1d h%s_strip_CorrCoeff '%s; Strip. Corr. Coeff.;' %s.strip.CorrCoeff[I] 200 0.5 1 %s.strip.ontrack[I]", 
      modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th2d h%s_strip_CorrCoeff_vs_Tdiff '%s; Tstrip - Tclust (ns); Corr. Coeff.' %s.strip.Tdiff[I] %s.strip.CorrCoeff[I] 200 -40 40 200 0.5 1 %s.strip.ontrack[I]", 
      modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th2d h%s_strip_Tdiff_vs_ADC '%s; strip ADC sum ; Tstrip - Tclust (ns)' %s.strip.ADCsum[I] %s.strip.Tdiff[I] 200 0 1.5e4 200 -40 40 %s.strip.ontrack[I]", 
      modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    odef_file << histdef.Format( "th2d h%s_strip_CorrCoeff_vs_ADC '%s; strip ADC sum ; Corr. Coeff.' %s.strip.ADCsum[I] %s.strip.CorrCoeff[I] 200 0 1.5e4 200 0.5 1 %s.strip.ontrack[I]", 
      modname_nodots[i].Data(), moddesc[i].Data(), modname[i].Data(), modname[i].Data(), modname[i].Data() ) << endl;
    
    
    

    odef_file << endl;
    
    
    odef_file << endl;
  }
  
  cfg_file << "newpage 2 2" << endl;
  cfg_file << "title Layers with hits" << endl;
  TString histcfg;

  cfg_file << histcfg.Format( "h%s_Nlayers_hit -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_Nlayers_hitu -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_Nlayers_hitv -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_Nlayers_hituv -nostat", detname_nodots.Data() ) << endl << endl;

  cfg_file << "newpage 2 2" << endl;
  cfg_file << "title Strip and cluster multiplicities" << endl;
  cfg_file << histcfg.Format( "h%s_NstripsU_layer -logz -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_NstripsV_layer -logz -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_NclustU_layer -logz -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_NclustV_layer -logz -drawopt colz -nostat", detname_nodots.Data() ) << endl << endl;

  cfg_file << "newpage 4 3" << endl;
  cfg_file << "title Cluster size, timing, ADC correlations" << endl;
  cfg_file << histcfg.Format( "h%s_clustwidthU -title \"U/X cluster size\"", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_clustwidthV -title \"V/Y cluster size\"", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_clustwidthVvsU -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "macro overlay.C(\"h%s_clust_Utime\",\"h%s_clust_Vtime\",\"U time (ns)\",\"V time (ns)\")", detname_nodots.Data(), detname_nodots.Data() ) << endl;
  //cfg_file << histcfg.Format( "h%s_ADCavg -drawopt colz", detname_nodots.Data() ) << endl;
  
  cfg_file << histcfg.Format( "h%s_clust_UVtime -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "macro plotfitgaus.C(\"h%s_clust_deltat\",0.5)", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_iSampMaxU  -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_iSampMaxV  -nostat", detname_nodots.Data() ) << endl;

  cfg_file << histcfg.Format( "h%s_CorrCoeff_clust", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_CorrCoeff_maxstrip", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCasym", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCasym_vs_ADCavg -drawopt colz", detname_nodots.Data() ) << endl << endl;


  cfg_file << "newpage 4 3" << endl;
  cfg_file << "title Strip and cluster ADC distributions and correlations" << endl;
  cfg_file << histcfg.Format( "h%s_ADCU_maxsample", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCU_maxstrip", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCU_maxclustsample", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCU_clust", detname_nodots.Data() ) << endl;

  cfg_file << histcfg.Format( "h%s_ADCV_maxsample", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCV_maxstrip", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCV_maxclustsample", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCV_clust", detname_nodots.Data() ) << endl;

  cfg_file << histcfg.Format( "h%s_ADCVvsU_maxsample -drawopt colz", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCVvsU_maxstrip -drawopt colz", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCVvsU_maxclustsample -drawopt colz", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_ADCVvsU_clust -drawopt colz", detname_nodots.Data() ) << endl << endl;

  cfg_file << "newpage 3 3" << endl;
  cfg_file << "title Tracking statistics" << endl;
  cfg_file << histcfg.Format( "h%s_Ntracks -logy", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_TrackNhits", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_TrackChi2NDF -logy", detname_nodots.Data() ) << endl;

  cfg_file << histcfg.Format( "h%s_TrackX", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_TrackY", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_TrackXY -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_TrackXp", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_TrackYp", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_TrackXpYp -drawopt colz -nostat", detname_nodots.Data() ) << endl << endl;

  cfg_file << "newpage 3 2" << endl;
  cfg_file << "title Tracking residuals (inclusive)" << endl;
  cfg_file << histcfg.Format( "macro plotfitgaus.C(\"h%s_residu_allhits\")", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_residu_vs_layer -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_residu_vs_module -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "macro plotfitgaus.C(\"h%s_residv_allhits\")", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_residv_vs_layer -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_residv_vs_module -drawopt colz -nostat", detname_nodots.Data() ) << endl;

  cfg_file << "newpage 3 2" << endl;
  cfg_file << "title Tracking residuals (exclusive)" << endl;
  cfg_file << histcfg.Format( "macro plotfitgaus.C(\"h%s_eresidu_allhits\")", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_eresidu_vs_layer -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_eresidu_vs_module -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "macro plotfitgaus.C(\"h%s_eresidv_allhits\")", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_eresidv_vs_layer -drawopt colz -nostat", detname_nodots.Data() ) << endl;
  cfg_file << histcfg.Format( "h%s_eresidv_vs_module -drawopt colz -nostat", detname_nodots.Data() ) << endl << endl;

  for( int i=0; i<nmodules; i++ ){
    cfg_file << "newpage 3 2" << endl;
    cfg_file << "title Module " << i << " (" << moddesc[i].Data() << ") efficiencies" << endl;

    cfg_file << histcfg.Format( "hefficiency_vs_x_%s", modname_nodots[i].Data() ) << endl;
    cfg_file << histcfg.Format( "hefficiency_vs_y_%s", modname_nodots[i].Data() ) << endl;
    cfg_file << histcfg.Format( "hefficiency_vs_xy_%s -drawopt colz -nostat", modname_nodots[i].Data() ) << endl;

    cfg_file << histcfg.Format("hdidhitxy_%s -drawopt colz -nostat", modname_nodots[i].Data() ) << endl;
    
    cfg_file << histcfg.Format( "macro efficiency.C(\"hdidhitx_%s\",\"hshouldhitx_%s\",%d);", 
				modname_nodots[i].Data(), modname_nodots[i].Data(), i ) << endl;
    cfg_file << histcfg.Format( "macro efficiency.C(\"hdidhity_%s\",\"hshouldhity_%s\",%d);", 
				modname_nodots[i].Data(), modname_nodots[i].Data(), i ) << endl << endl;
    
    

    
    
    

    // cfg_file << histcfg.Format( "hshouldhitx_%s", modname_nodots[i].Data() ) << endl;
    // cfg_file << histcfg.Format( "hshouldhity_%s", modname_nodots[i].Data() ) << endl;
    // cfg_file << histcfg.Format( "hshouldhitxy_%s -drawopt colz -nostat", modname_nodots[i].Data() ) << endl;
    // cfg_file << histcfg.Format( "hdidhitx_%s", modname_nodots[i].Data() ) << endl;
    // cfg_file << histcfg.Format( "hdidhity_%s", modname_nodots[i].Data() ) << endl;
    // cfg_file << histcfg.Format( "hdidhitxy_%s -drawopt colz -nostat", modname_nodots[i].Data() ) << endl;
    
    // cfg_file << histcfg.Format( "hefficiency_vs_xy_%s -drawopt colz -nostat", modname_nodots[i].Data() ) << endl << endl;
  }

  for( int i=0; i<nlayers; i++ ){
    cfg_file << "newpage 3 2" << endl;
    cfg_file << "title Layer " << i << " efficiencies" << endl;

    cfg_file << histcfg.Format( "hefficiency_x_%s_layer%d", detname_nodots.Data(), i ) << endl;
    cfg_file << histcfg.Format( "hefficiency_y_%s_layer%d", detname_nodots.Data(), i ) << endl;
    cfg_file << histcfg.Format( "hefficiency_xy_%s_layer%d -drawopt colz -nostat", detname_nodots.Data(), i ) << endl;


    cfg_file << histcfg.Format( "hdidhit_xy_%s_layer%d -drawopt colz -nostat", detname_nodots.Data(), i ) << endl;
    
    cfg_file << histcfg.Format("macro efficiency_layer.C(\"hdidhit_x_%s_layer%d\",\"hshouldhit_x_%s_layer%d\",%d);",
			       detname_nodots.Data(), i, detname_nodots.Data(), i, i ) << endl;
    cfg_file << histcfg.Format("macro efficiency_layer.C(\"hdidhit_y_%s_layer%d\",\"hshouldhit_y_%s_layer%d\",%d);",
			       detname_nodots.Data(), i, detname_nodots.Data(), i, i ) << endl << endl;
   

    // cfg_file << histcfg.Format( "hshouldhit_x_%s_layer%d", detname_nodots.Data(), i ) << endl;
    // cfg_file << histcfg.Format( "hshouldhit_y_%s_layer%d", detname_nodots.Data(), i ) << endl;
    // cfg_file << histcfg.Format( "hshouldhit_xy_%s_layer%d -drawopt colz -nostat", detname_nodots.Data(), i ) << endl;

    // cfg_file << histcfg.Format( "hdidhit_x_%s_layer%d", detname_nodots.Data(), i ) << endl;
    // cfg_file << histcfg.Format( "hdidhit_y_%s_layer%d", detname_nodots.Data(), i ) << endl;
   
    
  }

  int pagediv = int(sqrt(double(nmodules)));

  int testdiv=1;

  while( pagediv*testdiv < nmodules ){ testdiv++; }

  int ndivx = std::max(pagediv,testdiv);
  int ndivy = std::min(pagediv,testdiv);

  cfg_file << endl;
  cfg_file << "newpage " << ndivx << " " << ndivy << endl;
  cfg_file << "title Module average efficiencies" << endl;
  
  for( int i=0; i<nmodules; i++ ){
    cfg_file << histcfg.Format( "macro efficiency.C(\"hdidhitx_%s\",\"hshouldhitx_%s\",%d);",
				modname_nodots[i].Data(), modname_nodots[i].Data(), i ) << endl;
  }
  cfg_file << endl;
  
  pagediv = int(sqrt(double(nlayers)));
  testdiv=1; 
  while( pagediv*testdiv < nlayers ){ testdiv++; }
  
  ndivx = std::max(pagediv,testdiv);
  ndivy = std::min(pagediv,testdiv);

  cfg_file << endl;
  cfg_file << "newpage " << ndivx << " " << ndivy << endl;
  cfg_file << "title Layer hit maps on good tracks" << endl;
  for( int i=0; i<nlayers; i++ ){
    cfg_file << histcfg.Format("macro GEM_hit_map.C(%d);",i) << endl;
  }
  cfg_file << endl;
  
  
  
  			     
  for( int i=0; i<nmodules; i++ ){
    cfg_file_lowlevel << "newpage 6 3" << endl;
    cfg_file_lowlevel << "title " << moddesc[i] << " low-level (with track cuts)" << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_nstripstot_good", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_strip_Tdiff", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_strip_CorrCoeff", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_strip_CorrCoeff_vs_Tdiff -drawopt colz -nostat", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_strip_Tdiff_vs_ADC -drawopt colz -nostat", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_strip_CorrCoeff_vs_ADC -drawopt colz -nostat", modname_nodots[i].Data() ) << endl;

    //cfg_file_lowlevel << histcfg.Format("macro overlay.C(\"h%s_nclustU_good\",\"h%s_nclustV_good\",\"Num. U/X clusters\",\"Num. V/Y clusters\")", modname_nodots[i].Data(), modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_nclustU_good", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_nclustV_good", modname_nodots[i].Data() ) << endl;
    
    cfg_file_lowlevel << histcfg.Format("h%s_TimeU_good", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_TimeV_good", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_iSampMaxU_good", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_iSampMaxV_good", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_stripU_good", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_stripV_good", modname_nodots[i].Data() ) << endl;
    
    cfg_file_lowlevel << histcfg.Format("h%s_ADCmaxU_good -logy", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_ADCmaxV_good -logy", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_ADCsumU_good -logy", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_ADCsumV_good -logy", modname_nodots[i].Data() ) << endl << endl;
    
    // cfg_file_lowlevel << histcfg.Format("h%s_ADCmax_vs_Ustrip_good -drawopt colz -logz", modname_nodots[i].Data() ) << endl;
    // cfg_file_lowlevel << histcfg.Format("h%s_ADCmax_vs_Vstrip_good -drawopt colz -logz", modname_nodots[i].Data() ) << endl;
    // cfg_file_lowlevel << histcfg.Format("h%s_ADCsum_vs_Ustrip_good -drawopt colz -logz", modname_nodots[i].Data() ) << endl;
    // cfg_file_lowlevel << histcfg.Format("h%s_ADCsum_vs_Vstrip_good -drawopt colz -logz", modname_nodots[i].Data() ) << endl << endl;
  }


  for( int i=0; i<nmodules; i++ ){
    cfg_file_lowlevel << "newpage 4 3" << endl;
    cfg_file_lowlevel << "title " << moddesc[i] << " low-level (no track cuts)" << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_nstripstot_all -logy", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("macro overlay.C(\"h%s_nclustU_all\",\"h%s_nclustV_all\",\"Num. U/X clusters\",\"Num. V/Y clusters\")", modname_nodots[i].Data(), modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_TimeU_all", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_TimeV_all", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_iSampMaxU_all", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_iSampMaxV_all", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_stripU_all", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_stripV_all", modname_nodots[i].Data() ) << endl;

    cfg_file_lowlevel << histcfg.Format("h%s_ADCmaxU_all -logy", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_ADCmaxV_all -logy", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_ADCsumU_all -logy", modname_nodots[i].Data() ) << endl;
    cfg_file_lowlevel << histcfg.Format("h%s_ADCsumV_all -logy", modname_nodots[i].Data() ) << endl;

    // cfg_file_lowlevel << histcfg.Format("h%s_ADCmax_vs_Ustrip_all -drawopt colz -logz", modname_nodots[i].Data() ) << endl;
    // cfg_file_lowlevel << histcfg.Format("h%s_ADCmax_vs_Vstrip_all -drawopt colz -logz", modname_nodots[i].Data() ) << endl;
    // cfg_file_lowlevel << histcfg.Format("h%s_ADCsum_vs_Ustrip_all -drawopt colz -logz", modname_nodots[i].Data() ) << endl;
    // cfg_file_lowlevel << histcfg.Format("h%s_ADCsum_vs_Vstrip_all -drawopt colz -logz", modname_nodots[i].Data() ) << endl << endl;
  }

  cfg_file_lowlevel << endl;
  
  pagediv = int(sqrt(double(nmodules)));
  testdiv=1; 
  while( pagediv*testdiv < nmodules ){ testdiv++; }
  
  ndivx = std::max(pagediv,testdiv);
  ndivy = std::min(pagediv,testdiv);


  cfg_file_lowlevel << histcfg.Format("newpage %d %d", ndivx, ndivy) << endl;
  cfg_file_lowlevel << "title All modules strip ADC max" << endl;
  for( int i=0; i<nmodules; i++ ){
    cfg_file_lowlevel << histcfg.Format("h%s_ADCmax_good_CM -logy", modname_nodots[i].Data() ) << endl;
  }
  cfg_file_lowlevel << endl;

  cfg_file_lowlevel << histcfg.Format("newpage %d %d", ndivx, ndivy) << endl;
  cfg_file_lowlevel << "title All modules strip ADC sum" << endl;
  for( int i=0; i<nmodules; i++ ){
    cfg_file_lowlevel << histcfg.Format("h%s_ADCsum_good_CM -logy", modname_nodots[i].Data() ) << endl;
  }
  cfg_file_lowlevel << endl;

  cfg_file_lowlevel << histcfg.Format("newpage %d %d", ndivx, ndivy) << endl;
  cfg_file_lowlevel << "title All modules max time sample" << endl;
  for( int i=0; i<nmodules; i++ ){
    cfg_file_lowlevel << histcfg.Format("h%s_iSampMax_good_CM", modname_nodots[i].Data() ) << endl;
  }
  cfg_file_lowlevel << endl;

  cfg_file_lowlevel << histcfg.Format("newpage %d %d", ndivx, ndivy) << endl;
  cfg_file_lowlevel << "title All modules strip time" << endl;
  for( int i=0; i<nmodules; i++ ){
    cfg_file_lowlevel << histcfg.Format("h%s_StripTime_good_CM", modname_nodots[i].Data() ) << endl;
  }
  cfg_file_lowlevel << endl;

  cfg_file_lowlevel << histcfg.Format("newpage %d %d", ndivx, ndivy) << endl;
  cfg_file_lowlevel << "title All modules strip rms time" << endl;
  for( int i=0; i<nmodules; i++ ){
    cfg_file_lowlevel << histcfg.Format("h%s_StripTsigma_good_CM", modname_nodots[i].Data() ) << endl;
  }
  cfg_file_lowlevel << endl;

  cfg_file_lowlevel << histcfg.Format("newpage %d %d", ndivx, ndivy) << endl;
  cfg_file_lowlevel << "title All modules U strip ADC no zero sup" << endl;
  for( int i=0; i<nmodules; i++ ){
    cfg_file_lowlevel << histcfg.Format("hADCpedsubU_allstrips_%s -logy", modname_nodots[i].Data() ) << endl;
  }
  cfg_file_lowlevel << endl;
  
}
