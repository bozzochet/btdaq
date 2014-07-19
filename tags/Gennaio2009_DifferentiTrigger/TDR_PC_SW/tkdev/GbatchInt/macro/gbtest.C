//  $Id: gbtest.C,v 1.1 2008/01/21 21:58:35 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  gbtest.C
///\brief A ROOT macro which runs Gbatch  based on tracker only geometry 
///
/// You need to prepare module file which describe 
/// a class named GbatchTest inherited from GbatchTRK class.\n
/// In GbatchTest class you can implement your own actions: \n
///  PreRun, PostRun, PreEvent, and PostEvent \n
///
/// For an example of the GbatchTest module you can see RgtRest.C
///
///\date  2007/11/22 SH  First test version
///\date  2007/11/29 SH  Second test version including GbatchTest module
///$Date: 2008/01/21 21:58:35 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

Int_t gbtest(const char *module, Float_t momt = 10, 
	     Int_t ntrg = 10000, Int_t tkcode = 2, Int_t debug = 0)
{
///\brief Main function
///
/// Module file name must be set. 
/// In the module file a class named GbatchTest must be defined. 
/// Primary momentum and number of events can be also set.
/// Tracker code can be selected with tkcode (1: Gbatch 2: \ref tkrec). 
/// Debug message is shown if debug is set >= 1

  // Compile and load GbatchTest module
  gROOT->ProcessLine(Form(".L %s+", module));


  // Read magnetic field data
  GbatchTRK::InitBfield(gSystem->ExpandPathName(
			  "$AMSDataDir/DataBase/MagneticFieldMap/"
			  "978562800/MagneticFieldMap.0.1014715719"));

  // Create a GbatchTest instance
  GbatchTest gb;
  GbatchTRK::SetRecCodes(tkcode);
  GbatchTRK::SetRecDebug(debug);

  // Set parameters
  gb.SetVerbose(1);
  gb.SetMomentum(momt);

  // Input datacatds
  gb.DataCard("LIST");
  gb.DataCard("KINE 14");
  gb.DataCard("LOSS  1");
  gb.DataCard("HADR  0");
  gb.DataCard("MULS  1");
  gb.DataCard("DRAY  0");
  gb.DataCard(Form("TRIG %d", ntrg));
  gb.DataCard("END");

  // Start Run
  gb.Main();

  return 0;
}
