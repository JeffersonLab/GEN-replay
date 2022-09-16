#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "TObjArray.h"
#include "TObjString.h"

void CommonModeBias(const char *rootfilename, const char *configfilename, const char *outfilename="CMbias_temp.dat" ){

  // Note: because I'm lazy, I'm not making the module names configurable. I assume each module is named "m%d", where %d is the module number.
  // Someone less lazy will need to fix this.
  TFile *fin = new TFile(rootfilename,"READ");

  if( fin ){
    ofstream outfile( outfilename );

    ifstream configfile( configfilename );
    if( !configfile ){
      return;
    }

    TString prefix = "bb.gem";
    int nmodules=12;

    vector<TString> modulenames(nmodules);
    
    TString currentline;
    while( currentline.ReadLine(configfile) && !currentline.BeginsWith("endconfig") ){
      if( !currentline.BeginsWith("#") ){
	TObjArray *tokens = currentline.Tokenize(" ");
	int ntokens = tokens->GetEntries();

	if( ntokens >= 2 ){
	  TString skey = ( (TObjString*) (*tokens)[0] )->GetString();
	  if( skey == "nmodules" ){
	    TString sval = ( (TObjString*) (*tokens)[1] )->GetString();
	    nmodules = sval.Atoi();
	    modulenames.resize( nmodules );
	  }

	  if( skey == "modulenames" && ntokens >= nmodules + 1 ){
	    for( int i=0; i<nmodules; i++ ){
	      TString sval = ( (TObjString*) (*tokens)[i+1] )->GetString();

	      modulenames[i] = sval;
	    }
	  }
	  if( skey == "prefix" ){
	    TString sval = ( (TObjString*) (*tokens)[1] )->GetString();
	    prefix = sval;
	  }
	}
      }
    }

    TString prefix_nodots = prefix;
    
    prefix_nodots.ReplaceAll(".","_");
    
    for( int imod=0; imod<nmodules; imod++ ){
      TString histnameU,histnameV;
      histnameU.Form( "hcommonmodeU_bias_%s_%s", prefix_nodots.Data(), modulenames[imod].Data() );
      histnameV.Form( "hcommonmodeV_bias_%s_%s", prefix_nodots.Data(), modulenames[imod].Data() );

      TH2D *hbiasU_temp;
      TH2D *hbiasV_temp;

      fin->GetObject( histnameU, hbiasU_temp );
      fin->GetObject( histnameV, hbiasV_temp );

      int nAPVU = hbiasU_temp->GetNbinsX();
      int nAPVV = hbiasV_temp->GetNbinsX();

      TString sheader;
      sheader.Form( "%s.%s.CMbiasU = ", prefix.Data(), modulenames[imod].Data() );

      outfile << sheader << endl;

      vector<double> CMbiasU(nAPVU);
      
      for( int ibin=1; ibin<=nAPVU; ibin++ ){
	TString histname;
	TH1D *htemp = hbiasU_temp->ProjectionY( TString::Format( "%s_py_bin%d", hbiasU_temp->GetName(), ibin ), ibin, ibin );
	CMbiasU[ibin-1] = htemp->GetMean();

	outfile << TString::Format( " %12.5g ", CMbiasU[ibin-1] );
	if( ibin % 16 == 0 ) outfile << endl;
      }
      outfile << endl << endl;


      sheader.Form( "%s.%s.CMbiasV = ", prefix.Data(), modulenames[imod].Data() );

      outfile << sheader << endl;
      
      vector<double> CMbiasV(nAPVV);

      for( int ibin=1; ibin<=nAPVV; ibin++ ){
	TString histname;
	TH1D *htemp = hbiasV_temp->ProjectionY( TString::Format( "%s_py_bin%d", hbiasV_temp->GetName(), ibin ), ibin, ibin );
	CMbiasV[ibin-1] = htemp->GetMean();

	outfile << TString::Format( " %12.5g ", CMbiasV[ibin-1] );
	if( ibin % 16 == 0 ) outfile << endl;
      }
      outfile << endl << endl;
      
    }
    
    
    
  
    
  }
  
}
