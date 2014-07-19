//  $Id: gbatch.C,v 1.1 2008/01/21 21:58:35 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  gbatch.C
///\brief A ROOT macro which should work exactly the same as gbatch.exe
/// 
/// It is confirmed with sh/g3.test.job on SLC 4.5 with Gbatch v4.00/189/2
///
/// The file name of datacards can be set as an argument.\n
/// The datacards file can be gbatch job file.\n
/// Actual datacard contents are extracted from the job file.\n
///
///\date  2007/11/22 SH  First test version
///\date  2007/11/26 SH  First stable version
///$Date: 2008/01/21 21:58:35 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

Int_t gbatch(const char* fname = 0)
{
///\brief Main function
///
/// Datacard file can be set as an argument

  // Create a GbatchAMS instance
  GbatchAMS gb;

  // Input datacards
  if (datacards(gb, fname) < 0) return -1;

  // Start run
  return gb.Main();
}

Int_t datacards(GbatchAMS &gb, const char* fname = 0)
{
///\brief Input datacards
///
/// Default datacards are input with defcards if fname = 0

  // Input default datacards if fname is not set
  if (!fname) {
    defcards(gb);
    return 0;
  }

  // Open datacards file
  ifstream fin(fname);
  if (!fin) {
    cout << "Datacards file not found: " << fname << endl;
    return -1;
  }

  TString sbuf;
  Int_t sw = 0, ret = -1;

  // Look for the actual datacard contents in the file
  while (!sbuf.ReadLine(fin).eof()) {
    if (sw && sbuf.Contains("!")) break;
    if(!sw && sbuf.Contains("!") && sbuf.Contains("gbatch.exe")) sw = 1;
    if (sw) ret = gb.DataCard(sbuf.Data());
  }

  return (ret == 1) ? 0 : -1;
}

void defcards(GbatchAMS &gb)
{
///\brief Input default datacards
///
/// The contents are imported from examples/g3.test.job

  gb.DataCard("LIST");
  gb.DataCard("KINE 14");
  gb.DataCard("LOSS 1");
  gb.DataCard("HADR 1");
  gb.DataCard("MULS 1");
  gb.DataCard("CKOV 1");
  gb.DataCard("RAYL 1");
  gb.DataCard("CUTS 1=0.0005 2=0.0005 3=0.001 4=0.001 5=0.001");
  gb.DataCard("TRIG 100");
  gb.DataCard("DEBUG -1 10 10");

  gb.DataCard("MISC 4=1 5=0       ");
  gb.DataCard("MCGEN 1=-2. 2=-2. 3=140. 4=2. 5=2. 6=140.");
  gb.DataCard("7=0. 8=0. 9=-1. 10=0. 11=0. 12=-1.");
  gb.DataCard("C 12=-0.95 ");
  gb.DataCard("13=5. 14=5. 15=0 16=1. 19=101 20=5 21=0  ");
  gb.DataCard("25=01012008 27=01012010");

  gb.DataCard("IOPA 1=2 2='g3v1g3.hbk' 43=2");
  gb.DataCard("45='AMSmceventg   ' 126=10000 ");

  gb.DataCard("RNDM  870274471 1739888007");
             //      Simul Debug    CpuLim  Read Write Jobname
  gb.DataCard("AMSJOB 1=0   2=0     3=50.   4=0  5=0 ");

  gb.DataCard("TIME 3=10000000");
  gb.DataCard("TERM 1234567890");
  gb.DataCard("L3REC 11=0");

  gb.DataCard("END");
}
