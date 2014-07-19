// $Id: tkinits.h,v 1.1 2008/01/21 21:58:33 haino Exp $
#ifndef __tkinits__
#define __tkinits__

//////////////////////////////////////////////////////////////////////////
///
///\file  tkinits.h
///\brief Routines to initialize parameters for tracker
/// 
/// Tracker-relapted parts extracted from job.C
///
///\date  2007/11/27 SH  First test version
///\date  2007/11/29 SH  First stable version
///$Date: 2008/01/21 21:58:33 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

namespace gbtrk {
  /// Initialize datacards, extracted from AMSJob::gethead()->data()
  void cardinit(void);

  /// Initialize parameters, extracted from AMSJob::gethead()->udata()
  void datainit(void);

  /// Initialize jobs, extracted from AMSJob::gethead()->init()
  void jobinit(void);
}

#endif
