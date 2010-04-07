// $Id: tkgeom.h,v 1.1 2008/01/21 21:58:33 haino Exp $
#ifndef __tkgeom__
#define __tkgeom__

//////////////////////////////////////////////////////////////////////////
///
///\file  tkgeom.h
///\brief Definition for materials/mediums and geometry for tracker
/// 
/// Tracker-related parts are extracted from gmat.C and amsgeom.C
///
///\date  2007/11/27 SH  First test version
///\date  2007/11/29 SH  First stable version
///$Date: 2008/01/21 21:58:33 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

namespace gbtrk {
  void amsmat (void);  ///< Define materials
  void amstmed(void);  ///< Define tracking mediums
  void amsgeom(void);  ///< Define geometry
}

#endif
