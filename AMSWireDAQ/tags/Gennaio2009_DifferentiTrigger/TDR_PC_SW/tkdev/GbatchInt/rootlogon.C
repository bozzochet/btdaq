//  $Id: rootlogon.C,v 1.2 2008/01/22 23:39:29 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  rootlogon.C
///\brief A ROOT macro automatically executed when you run ROOT CINT
///
/// In this macro shared library is loaded and include path is set.\n 
/// The include path should be written at config/incdirs file when 
/// the codes are compiled. 
///
///\date  21/Nov/2007 SH  First test version
///\date  26/Nov/2007 SH  First stable version
///$Date: 2008/01/22 23:39:29 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

void rootlogon(void)
{
  // Load shared library 
  gSystem->Load("lib/libGbInt.so");

  // Set include path for ACLiC
  std::ifstream fin("config/incdirs");
  if (!fin) {
    std::cerr << "config/incdires does not exist" << std::endl;
    return;
  }

  TString str;
  str.ReadLine(fin);
  str += " -I$ROOTSYS/include";
  gSystem->SetIncludePath(str.Data());
}
