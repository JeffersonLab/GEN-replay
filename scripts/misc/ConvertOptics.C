#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TString.h"

void ConvertOptics( const char *oldfilename, const char *newfilename ){
  ifstream infile( oldfilename );

  ofstream outfile( newfilename );
  std::string thisline;

  while( std::getline( infile, thisline ) ){
    std::istringstream is( thisline );

    double coeffs[4];
    int expon[5];
    for( int i=0; i<4; i++ ){
      is >> coeffs[i];
    }

    coeffs[3] = 0.0;
    
    for( int i=0; i<5; i++ ){
      is >> expon[i];
    }

    TString outline;
    outline.Form( "%15.9g %15.9g %15.9g %15.9g      %d %d %d %d %d", coeffs[0], coeffs[1], coeffs[2], coeffs[3],
		  expon[0], expon[1], expon[2], expon[3], expon[4] );

    outfile << outline << endl;
    
  }

}
