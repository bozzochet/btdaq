//  $Id: RTDisplay3D.h,v 1.3 2008/12/03 21:02:03 haino Exp $
#ifndef _TDISPLAY_3D_
#define _TDISPLAY_3D_

//////////////////////////////////////////////////////////////////////////
///
///
///\class RTDisplay3D
///\brief A class to manage 3D AMS Tracker/TOF display
///
/// This class is a standalone one - dependent only on 
/// ROOT (v5.18.00 or higher version recomended) and TrackerLib, 
/// so one can create a standalone display on ROOT CINT as: \n
///
/// root [0] gROOT->ProcessLine(".L RTDisplay3D.C+");\n
/// root [1] TkDBc::CreateTkDBc(); TkDBc::Head->init();\n
/// root [2] RTrackerDisplay3D::RunDisplay3D();\n
///
/// and then one can the draw 3D display with your event object with:\n
///
/// root [3] RTrackerDisplay3D::DrawOneEvent(event);\n
///
///\date  2008/11/15 SH  First stable version
///$Date: 2008/12/03 21:02:03 $
///
///$Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////////

#include "Event.h"

class TCanvas;
class TObjArray;
class TGeoVolume;
class TrProp;

class RTrackerDisplay3D {

public:
  /// Enums for draw option bits
  enum { TRKSUP = 0x0001,    /// Draw Tracker support plane
	 ALLTRK = 0x0002,    /// Draw all the Tracker ladders
	 NUMTRK = 0x0004,    /// Draw all the Ladder IDs
	 TRKHIT = 0x0008,    /// Draw Tracker clusters

	 ANYTOF = 0x0100,    /// Draw TOF or not
	 ALLTOF = 0x0200,    /// Draw all the TOF pads
	 NUMTOF = 0x0400,    /// Draw all the TOF pad numbers
	 TOFHIT = 0x0800     /// Draw TOF hit position
  };

protected:
  /// Draw Option indicated by OR-ed sum of option bits
  static int Option;

  /// Self pointer
  static RTrackerDisplay3D *fSelfPtr;

  /// Number of polyline data for numbers
  static int NumDataN[11];
  /// Index of polyline data for numbers
  static int NumDataI[11];
  /// Polyline data for numbers (x position)
  static int NumDataX[152];
  /// Polyline data for numbers (y position)
  static int NumDataY[152];

  /// Array holding PolyLine3D objects to be drawn
  TObjArray *LineArray;

  /// Dummy canvas
  TCanvas *fCanvas;

  /// Number of ladder edge points at each layer
  int    NpLadLine[8];
  /// Ladder edge points (X)
  double PxLadLine[512];
  /// Ladder edge points (Y)
  double PyLadLine[512];
  /// Ladder edge points (Z)
  double PzLadLine[512];
  
public:
  /// Test draw option
  static bool TestOption(int option) { return (Option & option); }
  /// Add draw option
  static void AddOption (int option) { Option |=  option; }
  /// Remove draw option
  static void DelOption (int option) { Option &= ~option; }
  /// Set draw option with a switch
  static void SetOption (int option, bool sw) { 
    if (sw) AddOption(option); else DelOption(option);
  }

  /// Initiate the 3D display and run GLViewer
  static void RunDisplay3D(const char *option = "rtogl");

  /// Draw one event
  static void DrawOneEvent(Event *event = 0);

  /// Draw a track with TrProp
  static void DrawOneTrack(TrProp *tpr);

  /// Print current event
  static void PrintEvent(const char *pname);

  static TCanvas *GetCanvas(void) { 
    return (fSelfPtr) ? fSelfPtr->fCanvas : 0; }

  /// Destructor
  virtual ~RTrackerDisplay3D();

protected:
  /// Constructor is protected it can be created only through RunDisplay3D
  RTrackerDisplay3D(void);

  /// Build a 3D detector geometry
  void BuildGeom(void);

  /// Build a 3D Tracker geometry on the mother volume, tmere
  void BuildTracker(TGeoVolume *tmere);

  /// Build a 3D TOF geometry on the mother volume, tmere
  void BuildTof(TGeoVolume *tmere);

  /// Build external TOF pad called by Build3D
  TGeoVolume *BuildExtTof(int ip, const char *vname);

  /// Draw an event
  void DrawEvent(Event *event);

  /// Draw a TrTrack
  void DrawTrack(TrTrackR *trk, int col, int wid = 2, int sty = 1);

  /// Draw a track with TrProp
  void DrawTrack(TrProp *trp, int col = 2, int wid = 2, int sty = 1);

  /// Draw 3D Tracker hits
  void DrawTrackerHits(Event *event);

  /// Draw 3D TOF hits
  void DrawTofHits(Event *event);

  /// Draw 3D polyline
  void DrawLine(int n, double *x, double *y, double *z,	int col,
		int wid = 1, int sty = 1);
  /// Draw 3D polyline
  void DrawLine(int n, AMSPoint *plist, int col,
		int wid = 1, int sty = 1);
  /// Lines stored in LineArray are actually drawn here
  void DrawLines(void);

  /// Draw number [0-9] with 3D polyline
  void DrawNum(double x0, double y0, double z0, int num, 
	       int mode = 1, double size = 1.5, int col = 1, int wid = 2);
  /// Draw digits [0-9],'-' with 3D polyline
  void DrawDigit(double x0, double y0, double z0, int digit, 
		 int mode = 1, double size = 1.5, int col = 1, int wid = 2);
};

#endif
