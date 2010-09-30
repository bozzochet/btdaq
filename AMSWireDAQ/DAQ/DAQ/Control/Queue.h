#ifndef QUEUE_H 
#define QUEUE_H 
 
#include <iostream> 
#include <errno.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
 
#include <unistd.h>     
#include <signal.h>     
#include <string.h> 
 
 
struct my_msgbuf{ 
  long mtype; 
  int par1; 
  int par2; 
}; 
 
struct log_msgbuf{ 
  long mtype;               // priority =0 Warning,debug info 
  char msg[200];            //          =1 general info 
  int priority;             //          =2 error 
}; 
  
enum mCOMANDS{ 
  kWakeUp=1001, 
  kTimeOut=1002, 
  kReady=1003, 
  kResetHW=1004, 
  kSetStation=1005,
 
  kNewLadder=1011, 
  kSetReferenceTDR=1012,
  kSetOctant=1013,
  kTDRExit=1019, 
 
  kCheckLadder=1020, 
  kLadderExist=1021, 
  kLadderNotExist=1022, 
  kDefineLadderPlease=1023, 
 
  kCreateLadder=1024, 
  kSaveLadder=1025, 
  kDeleteLadder=1026, 
  kLadderCreated=1027, 
  kLadderSaved=1028, 
  kLadderDeleted=1029, 
 
  kCheckProcess=1031, 
  kProcessOk=1032, 
  kProcessNotOk=1033, 
 
  kSetLogStatus=1041, 
  kSetDataMode=1042, 
  kSetTrigger=1043, 
  kSetSource=1044, 
   
  kSetupCalibration=1051, 
  kNextCalStep=1052, 
  kFinishPassCal=1053, 
  kCalibrationFinished=1054, 
  kSaveCalibration=1055, 
  kWriteCalSummary=1056, 
  kShowCalSummary=1057, 
  kReadDefaultCalibration=1058, 
 
  kSetupGain=1061, 
  kNextGainStep=1062, 
  kFinishPassGain=1063, 
  kGainFinished=1064, 
  kComputeGain=1065,
  kSaveGain=1066, 
  kWriteGainSummary=1067, 
  kShowGainSummary=1068, 
 
  kOctantCalibration=1070,
  kFullCalibration=1071,
  kAMSTest=1072,
  kEndAMSTest=1073,
  kSourceTest=1074,
  kSourceAck2b=1075,
  kSourceAck2c=1076,
  kSourceAck2d=1077,
  kSource2bReady=1078,
  kSource2cReady=1079,
  kSource2dReady=1080,
  kEndSourceTest=1081,
  kDSPCalibration=1082,
  kDSPCalibDone=10820,
  kDSPCalibSaved=10821,
  kReadDSPCalib=10822,
  kSaveDSPCalib=1083, 
  kDSPCalibRead=1084,

  kStartRun=1085, 
  kStopRun=1086, 
  kReadTemp=1087,
  kShowTemp=1088,

  kRunStarted=1091, 
  kRunStoped=1092, 
  kRunNotStarted=1093, 
  kRunNotStoped=1094, 
   
  kReadEvent=1101, 
  kEventReaded=1102, 

  kTakeDSPCalib=1121,

  kRefresh=1201, 
  kRefreshTime=1202, 
  kRefreshLog=1203, 
  kRefreshDone=1204,

  kHardware_Problem=1301, 
  kTemperature_Problem=1302,
  kTemperature_Error=1303,

  kShowConnectedTdrs=1400,
  kUpdateToolTipsTdrs=1401,

  kInterrupt = 7777, 
 
}; 
 
enum mQUEUES{  
  kGUI=901, 
  kDSP=902, 
  kCTL=903, 
  kRUN=904, 
  kLOG=905, 
}; 
 
 
 
class Queue { 
 public: 
  Queue(const char *file, int id); 
  ~Queue(); 
 
  int NumberOfMessages(); 
  void Empty(); 
  void Send(long mtype,  int  par1, int par2); 
  void Send(long mtype, const char* msg,  int priority); 
 
  void Receive(long *mtype,  int *par1, int *par2); 
 
  void Wait(int mty, long *mtype,  int *par1, int *par2); 
  void Wait(int mty, long *mtype, char* msg,  int *priority); 
 
  void TimeOut(int mty,     int *par1, int *par2, int timeout); 
  void TimeOut(mQUEUES mty, int *par1, int *par2, int timeout); 
 
  void Delete(); 
 
 private: 
  key_t key; 
  int msgid; 
  int alarm_on; 
  struct my_msgbuf  *mybuf;
  struct log_msgbuf *logbuf;
  struct msqid_ds   mymsg_ds;
};



#endif /*QUEUE_H*/ 
       
