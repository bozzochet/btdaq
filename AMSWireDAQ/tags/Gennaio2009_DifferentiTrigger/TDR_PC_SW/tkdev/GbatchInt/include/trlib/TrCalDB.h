// $Id: TrCalDB.h,v 1.12 2008/10/23 10:12:39 zuccon Exp $

#ifndef __TrCalDB__
#define __TrCalDB__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrCalDB.h
///\brief Header file of TrCalDB class
///
///\class TrCalDB
///\brief The new tracker caibration data base
///\ingroup tkdbc
///
/// author: P.Zuccon -- INFN Perugia 20/11/2007
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  File name changed, some utils are added
///\date  2008/01/23 SH  Some comments are added
///$Date: 2008/10/23 10:12:39 $
///
///$Revision: 1.12 $
///
//////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "TObject.h"
#include "typedefs.h"
#include "TkDBc.h"
#include "TrLadCal.h"
#include "TFile.h"

//! The AMS Tracker Calibration database class

class TrCalDB :public TObject{

public:
  //! map for fast binary search based on Tkhwid
  map<int,TrLadCal*> trcal_hwidmap;

private:


public:
  TrCalDB(){}
  TrCalDB(char * filename);
  ~TrCalDB();
  void init();
  static void Load(char * filename);
  static TrCalDB* Head;
  static float* linear;
  static int  GetLinearSize(){return (1+192*TrLadCal::GetSize())*sizeof(float);}

  //! The run number
  int run;

  //! Returns the run number;
  int GetRun(){return run;}

  int GetEntries(){return trcal_hwidmap.size();}
  TrLadCal* GetEntry(int ii);
  
  //! write the content of the DB to an ascii file
  int write(char* filename){return 0;}

  //! read the  content of the DB from an ascii file
  int read(char* filename){ return 0;}

  //! Returns the pointer to the ladder Calibration object with the required Assembly id. In case of failure returns a NULL pointer
  TrLadCal* FindCal_TkAssemblyId( int tkassemblyid);
  
  //! Returns the pointer to the ladder Calibration object with the required tkid. In case of failure returns a NULL pointer
  TrLadCal* FindCal_TkId( int tkid);

  //! Returns the pointer to the ladder Calibration object with the required tkid. In case of failure returns a NULL pointer
  TrLadCal* FindCal_JMDC( int JMDCid);
  
  //! Returns the pointer to the ladder Calibration object with the required HwId. In case of failure returns a NULL pointer
  TrLadCal* FindCal_HwId( int hwid);
  
  //! Returns the pointer to the ladder Calibration object with the required ladder name. In case of failure returns a NULL pointer
  TrLadCal* FindCal_LadName(string& name);

  //! Interface to linear storage
  void Lin2CalDB();
  //! Interface to linear storage
  void CalDB2Lin();

  //! Setup the linear space for the DB
  static void CreateLinear(){
    linear= new float[GetLinearSize()/4];
  }


  //! Reset to zero all the calibration in the lib
  void Clear();

  ClassDef(TrCalDB,2);
};

typedef map<int,TrLadCal*>::const_iterator trcalIT;

void SLin2CalDB();

#endif

