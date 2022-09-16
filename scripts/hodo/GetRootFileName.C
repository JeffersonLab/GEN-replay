// GetRootFileName.C
//
// Helper macro for OnlineGUI to determine ROOT Filename
//   from a given runnumber
//
// 
TString GetRootFileName(UInt_t runnumber)
{
  //  TString file_path = "/w/work0/home/rachel/HallA/BB_Hodo/rootfiles/";
  //TString expID = "replayed_simdig_gmn13.5_1kevts_NB";
  TString file_path = "/adaqfs/home/a-onl/sbs/Rootfiles/bbhodo_hist/";
  TString expID = "RawTDC_bbhodo_";
  
  TString filename;
  filename = file_path + expID;
  filename += runnumber;
  filename += "_";
  //filename += runnumber;
  filename += "-1.root";
  // filename += "-all.root";

  return filename;

}
