//  $Id: LinkDef.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ global gAMSR_Root;
#pragma link C++ global debugger;
#pragma link C++ global gAMSR_Display;

#pragma link C++ class AMSR_Root-;
#pragma link C++ class AMSR_Maker-;

#pragma link C++ class AMSR_3DCluster;
#pragma link C++ class AMSR_ToFCluster;
#pragma link C++ class AMSR_ToFClusterReader;
#pragma link C++ class AMSR_TrdCluster;
#pragma link C++ class AMSR_TrdClusterReader;
#pragma link C++ class AMSR_AntiCluster;
#pragma link C++ class AMSR_AntiClusterReader;
#pragma link C++ class AMSR_TrMCCluster;
#pragma link C++ class AMSR_TrMCClusterReader;
#pragma link C++ class AMSR_TrdTrack;
#pragma link C++ class AMSR_TrdTrackReader;
#pragma link C++ class AMSR_Track;
#pragma link C++ class AMSR_TrackReader;
#pragma link C++ class AMSR_EcalShower;
#pragma link C++ class AMSR_EcalShowerReader;
#pragma link C++ class AMSR_SiHit;
#pragma link C++ class AMSR_SiHitReader;
//#pragma link C++ class AMSR_AntiCluster;
//#pragma link C++ class AMSR_AntiClusterReader;
#pragma link C++ class AMSR_Particle;
#pragma link C++ class AMSR_MCParticle;
#pragma link C++ class AMSR_ParticleReader;
#pragma link C++ class AMSR_MCParticleReader;
#pragma link C++ class AMSR_Ntuple;
#pragma link C++ class AMSR_HistBrowser;
#pragma link C++ class AMSR_VirtualDisplay;
#pragma link C++ class AMSR_Display;
#pragma link C++ class AMSR_GeometrySetter;
//#pragma link C++ class AMSR_KeyNode;
#pragma link C++ class AMSR_Canvas;
#pragma link C++ class AMSR_Axis;
#pragma link C++ class TRadioButton;
#pragma link C++ class TSwitch;
#pragma link C++ class TGRunEventDialog;
#pragma link C++ class EventRoot02+;
#pragma link C++ class AMSEventHeaderRoot+;
#pragma link C++ class TRDClusterRoot+;
#pragma link C++ class MCEventGRoot02+;
#pragma link C++ class TrTrackRoot02+;
#pragma link C++ class TRDTrackRoot+;

#pragma link C++ class Debugger;

#endif
