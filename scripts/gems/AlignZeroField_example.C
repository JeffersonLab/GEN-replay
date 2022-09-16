#include "TVector3.h"
#include "TMinuit.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TRandom3.h"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCut.h"
#include "TCutG.h"
#include "TEventList.h"
#include "TMinuit.h"
#include <iostream>
#include <fstream>


const double PI = TMath::Pi();

//Example code to align the GEM stack relative to the spectrometer coordinate system and determine distance from target to sieve plate using
//known dimensions and hole spacings of sieve and known dimensions and internal alignments of GEMs:
//We are going to be lazy and use MINUIT even though this problem could easily be linearized:

const double sigma_pos = 0.0001; //0.1 mm (wild guess)
const double sigma_slope = sigma_pos/1.6; //0.1 mm/1.6 m ~= 6e-5 gives rough order of magnitude for slope uncertainty, of course multiple scattering might invalidate that.

int NTRACKS;

//These arrays will be filled by Holly's code to select tracks going through the individual sieve holes:
vector<double> XTRACK,YTRACK,XPTRACK,YPTRACK; //Measured track parameters in GEM internal coordinate system
vector<double> XSIEVE,YSIEVE; //known x and y center positions of the holes the tracks went through.
//We assume the single foil position defines the global origin and acts as a point source of straight-line rays coming through the sieve holes into the GEMS

void CHI2_FCN( int &npar, double *gin, double &f, double *par, int flag ){
  double chi2 = 0.0;

  //Parameters are:
  
  // zsieve: distance from target to sieve along spectrometer central ray
  // (x0,y0,z0): coordinates of the GEM origin relative to the target center in spectrometer coordinates
  // (theta,phi): polar and azimuthal angles of the Z axis of the GEM stack relative to the spectrometer central ray:
  TVector3 GEMPOS(par[0], par[1], par[2] );
  
  double ZSIEVE = par[3];
  double thetaGEM = par[4];
  double phiGEM = par[5]; 

  //Define unit vectors along the GEM internal axes with respect to the global system:
  TVector3 GEM_zaxis( sin(thetaGEM)*cos(phiGEM), sin(thetaGEM)*sin(phiGEM), cos(thetaGEM) );

  TVector3 Global_yaxis(0,1,0);
  //Not clear if we will need these:
  TVector3 Global_xaxis(1,0,0);
  TVector3 Global_zaxis(0,0,1); 
  
  TVector3 GEM_xaxis = (Global_yaxis.Cross( GEM_zaxis )).Unit();
  TVector3 GEM_yaxis = (GEM_zaxis.Cross( GEM_xaxis )).Unit();
  
  chi2 = 0.0;

  for( int i=0; i<NTRACKS; i++ ){
    //Track position at sieve plate: XSIEVE and YSIEVE have to be determined by Holly's code:
    TVector3 TrackSievePos( XSIEVE[i], YSIEVE[i], ZSIEVE );

    //Track direction 
    TVector3 TrackDirGlobal = TrackSievePos.Unit();

    //Now compute track intersection with plane of first GEM:
    //Since all tracks start at origin, 
    //Equation is: ( s * TrackDir - GEMPOS ) dot GEM_zaxis = 0
    // OR: s = (GEMPOS dot GEM_zaxis)/(TrackDir dot GEM_zaxis)
    
    double sintersect = GEMPOS.Dot( GEM_zaxis )/(TrackDirGlobal.Dot( GEM_zaxis ) );

    TVector3 TrackIntersect_FirstGEMplane = sintersect * TrackDirGlobal;

    double xfp_expect = (TrackIntersect_FirstGEMplane - GEMPOS).Dot( GEM_xaxis );
    double yfp_expect = (TrackIntersect_FirstGEMplane - GEMPOS).Dot( GEM_yaxis );
    double xpfp_expect = TrackDirGlobal.Dot( GEM_xaxis )/TrackDirGlobal.Dot( GEM_zaxis );
    double ypfp_expect = TrackDirGlobal.Dot( GEM_yaxis )/TrackDirGlobal.Dot( GEM_zaxis );

    chi2 += ( pow( xfp_expect - XTRACK[i], 2 ) + pow( yfp_expect - YTRACK[i], 2 ) ) / (sigma_pos*sigma_pos) +
      ( pow( xpfp_expect - XPTRACK[i], 2 ) + pow( ypfp_expect - YPTRACK[i], 2 ) ) / (sigma_slope*sigma_slope);
    
  }
  
  f = chi2;
}

void AlignZeroField_example( const char *configfilename ){

  TChain *C = new TChain("T");
  
  ifstream configfile(configfilename);

  TString prefix = "bb.gem";

  //These are in spectrometer transport coordinates for an assumed magnet distance of 1.85 m
  double GEMX0=-0.1637, GEMY0=0.0, GEMZ0=2.958;
  double ZSIEVE=1.85-13.37*2.54/100.0; // = 1.51 m
  double GEMtheta = 10.0*PI/180.0; //this will become the pitch angle
  double GEMphi = 180.0*PI/180.0; 

  TCut globalcut = "";
  
  if( configfile ){
    TString currentline;
    
    while( currentline.ReadLine(configfile) && !currentline.BeginsWith("endlist") ){
      if( !currentline.BeginsWith("#") ){
	C->Add(currentline.Data());
      }
    }

    while( currentline.ReadLine(configfile) && !currentline.BeginsWith("endconfig")){
      if( !currentline.BeginsWith("#") ){
	TObjArray *tokens = currentline.Tokenize(" ");
	
	int ntokens = tokens->GetEntries();

	if( ntokens >= 2 ){
	  TString skey = ( (TObjString*) (*tokens)[0] )->GetString();

	  if( skey == "prefix" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    prefix = stemp;
	  }

	  if( skey == "GEMX0" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMX0 = stemp.Atof();
	  }
	  if( skey == "GEMY0" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMY0 = stemp.Atof();
	  }

	  if( skey == "GEMZ0" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMZ0 = stemp.Atof();
	  }

	  if( skey == "ZSIEVE" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    ZSIEVE = stemp.Atof();
	  }

	  if( skey == "GEMtheta" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMtheta = stemp.Atof();
	  }

	  if( skey == "GEMphi" ){
	    TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	    GEMphi = stemp.Atof();
	  }
	  
	}
      }
    }

    while( currentline.ReadLine(configfile) && !currentline.BeginsWith("endcut") ){
      if( !currentline.BeginsWith("#") ){
	globalcut += currentline;
      }
    }
  } else {
    return;
  }

  TEventList *elist = new TEventList("elist");
  
  C->Draw(">>elist",globalcut);
  
  cout << "Number of events passing global cut = " << elist->GetN() << endl;

  //TODO: need to declare variables and set tree branch addresses

  UInt_t MAXNTRACKS = 10000;
  
  double ntracks, besttrack;

  //We'll use the GEM track variables directly rather than the BigBite ones to avoid confusion initially.
  //The GEM track variables are guaranteed to be given in the internal GEM coordinates:
  vector<double> tracknhits(MAXNTRACKS);
  vector<double> trackChi2NDF(MAXNTRACKS);
  vector<double> trackX(MAXNTRACKS);
  vector<double> trackY(MAXNTRACKS);
  vector<double> trackXp(MAXNTRACKS);
  vector<double> trackYp(MAXNTRACKS); 

  C->SetBranchStatus("*",0);

  TString branchname;
  
  C->SetBranchStatus( branchname.Format("%s.track.ntrack",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.chi2ndf",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.x",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.y",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.xp",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.yp",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.nhits",prefix.Data() ), 1 );
  C->SetBranchStatus( branchname.Format("%s.track.besttrack",prefix.Data() ), 1 );
  //main event loop here:
  C->SetBranchAddress( branchname.Format("%s.track.ntrack",prefix.Data() ), &ntracks );
  C->SetBranchAddress( branchname.Format("%s.track.chi2ndf",prefix.Data() ), &(trackChi2NDF[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.x",prefix.Data() ), &(trackX[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.y",prefix.Data() ), &(trackY[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.xp",prefix.Data() ), &(trackXp[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.yp",prefix.Data() ), &(trackYp[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.nhits",prefix.Data() ), &(tracknhits[0]) );
  C->SetBranchAddress( branchname.Format("%s.track.besttrack",prefix.Data() ), &besttrack );
  

  TH2D *hxyfp = new TH2D("hxyfp"," ; y_{fp} (m) ; x_{fp} (m)",100,-0.3,0.3,250,-1.1,1.1);
  TH2D *hxysieve = new TH2D("hxysieve", " ; x_{sieve} (m) ; y_{sieve} (m)", 200,-0.2,0.2,400,-0.4,0.4);
  
  long nevent=0;

  TVector3 Global_yaxis(0,1,0);
  //Not clear if we will need these:
  TVector3 Global_xaxis(1,0,0);
  TVector3 Global_zaxis(0,0,1);

  TVector3 GEMzaxis( sin(GEMtheta)*cos(GEMphi), sin(GEMtheta)*sin(GEMphi), cos(GEMtheta) );
  TVector3 GEMxaxis = (Global_yaxis.Cross(GEMzaxis)).Unit();
  TVector3 GEMyaxis = (GEMzaxis.Cross(GEMxaxis)).Unit();
  TVector3 GEMorigin( GEMX0, GEMY0, GEMZ0 );
  TVector3 SieveOrigin(0,0,ZSIEVE);
  
  while( C->GetEntry( elist->GetEntry( nevent++ ) ) ){
    
    //do stuff: grab track info from tree, apply cuts, fill the track arrays defined near the top of this macro (see XTRACK, YTRACK, XPTRACK, YPTRACK, XSIEVE, YSIEVE above)
    int itr = int(besttrack);
    if( besttrack >= 0 && ntracks <= MAXNTRACKS ){
      hxyfp->Fill( trackY[itr], trackX[itr] );

      TVector3 TrackPos_local( trackX[itr], trackY[itr], 0.0 );
      TVector3 TrackDir_local( trackXp[itr], trackYp[itr], 1.0 );
      TrackDir_local = TrackDir_local.Unit();
      
      TVector3 TrackDir_global = TrackDir_local.X() * GEMxaxis +
	TrackDir_local.Y() * GEMyaxis +
	TrackDir_local.Z() * GEMzaxis;

      TVector3 TrackPos_global = GEMorigin + TrackPos_local.X() * GEMxaxis + TrackPos_local.Y() * GEMyaxis + TrackPos_local.Z() * GEMzaxis; 
      
      //Now compute intersection of track ray in global coordinates with sieve slit:
      // (trackpos + s * trackdir - sieveorigin) dot globalzaxis = 0
      // --> s * trackdir dot globalzaxis = (sieveorigin - trackpos) dot globalzaxis
      double sintersect = (SieveOrigin - TrackPos_global).Dot( Global_zaxis ) /
	( TrackDir_global.Dot( Global_zaxis ) );
      TVector3 TrackSievePos = TrackPos_global + sintersect * TrackDir_global;

      hxysieve->Fill( TrackSievePos.Y(), TrackSievePos.X() );
    }

   
    //////// INSERT CODE TO SELECT HOLES AND ASSIGN XSIEVE VALUES HERE /////////////

    /////// DO WE NEED TWO LOOPS OVER THE TREE? ONE TO FILL HISTOGRAMS AND ANOTHER TO
    /////// DEFINE CUTS?
    bool goodtrack = true;
    
  }

  nevent = 0;
  while( C->GetEntry( elist->GetEntry( nevent++ ) ) ){
    //In this second loop, make explicit the association between sieve holes and tracks:
  }
  
  
  TMinuit *FitZeroField = new TMinuit( 6 );

  FitZeroField->SetFCN( CHI2_FCN );

  int ierflg = 0;

  FitZeroField->mnparm( 0, "GEMX0", GEMX0, 0.01,0,0,ierflg ); //guesstimate 1 cm as initial position accuracy
  FitZeroField->mnparm( 1, "GEMY0", GEMY0, 0.03,0,0,ierflg ); //start with 3-cm uncertainty for y position
  FitZeroField->mnparm( 2, "GEMZ0", GEMZ0, 0.01,0,0,ierflg );

  FitZeroField->mnparm( 3, "ZSIEVE", ZSIEVE, 0.01,0,0,ierflg );
  FitZeroField->mnparm( 4, "GEMtheta", GEMtheta, 0.3*PI/180.0, 0, 0, ierflg ); //guesstimate 0.3 degrees as initial angular accuracy
  FitZeroField->mnparm( 5, "GEMphi", GEMphi, 0.3*PI/180.0, 0, 0, ierflg );

  double arglist[10];
  arglist[0]=1;
  FitZeroField->mnexcm("SET ERR",arglist,1,ierflg);
  
  arglist[0] = 5000;
  arglist[1] = 1.;

  //Uncomment this line when we are actually set up to do a fit:
  
  //FitZeroField->mnexcm("MIGRAD",arglist,2,ierflg);

  //TODO: grab parameters, write them out to file. Profit.
  
}
