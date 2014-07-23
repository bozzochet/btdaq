#include <cstdio>
#include "NIDaqClass.hh"

void NIDaqClass::Print(){
  char ss[120];
  printf("%s",PrintString(ss));
  return;
}


char* NIDaqClass::PrintString(char *ss){
//    sprintf(ss,"DO port %d: 0x%X  DO port %d: 0x%X  DI port %d: 0x%X  Counts: %d\n",
    sprintf(ss,"%X %X %X %d\n", 
	    r_dataDO_0[0],
	    r_dataDO_1[0],
	    r_dataDI[0],
	    Counts);	    
  return ss;
}

NIDaqClass::NIDaqClass(){
  debug=1;

  taskHandleC = 0;

  taskHandleDI = 0;
  num_portsDI  = 1;

  taskHandleDO_0 = 0;
  num_portsDO_0  = 1;

  taskHandleDO_1 = 0;
  num_portsDO_1  = 1;

  sprintf(chan1,"Dev1/port0");
  sprintf(chan2,"Dev1/port1");
  sprintf(chan3,"Dev1/port2/line0:6");
}

NIDaqClass::~NIDaqClass(){
  Stop();
}


int NIDaqClass::Stop(){
  int ret=0;
  if (taskHandleC != 0){
    if(debug)printf("Stoppo counter ...\n");
    ret=DAQmxBaseStopTask (taskHandleC);
    ret=DAQmxBaseClearTask (taskHandleC);
    if(debug)printf("Fatto.\n");
  }


  if (taskHandleDI != 0){
    if(debug)printf("Stoppo  Digital Input ...\n");
    ret=DAQmxBaseStopTask (taskHandleDI);
    ret=DAQmxBaseClearTask (taskHandleDI);
    if(debug)printf("Fatto.\n");
  }

  if (taskHandleDO_0 != 0){
    if(debug)printf("Stoppo Digital Output 0...\n");
    ret=DAQmxBaseStopTask (taskHandleDO_0);
    ret=DAQmxBaseClearTask (taskHandleDO_0);
    if(debug)printf("Fatto.\n");
  }


  if (taskHandleDO_1 != 0){
    if(debug)printf("Stoppo Digital Output 1...\n");
    ret=DAQmxBaseStopTask (taskHandleDO_1);
    ret=DAQmxBaseClearTask (taskHandleDO_1);
    if(debug)printf("Fatto.\n");
  }
  return ret;
}


int NIDaqClass::Read(){
  int     ret;
  int32   read;
  //Read the DO_0 ports
  ret=DAQmxBaseReadDigitalU32(taskHandleDO_0,1,10.0,DAQmx_Val_GroupByChannel,r_dataDO_0,num_portsDO_0,&read,NULL);
  if(ret) ProcessError(ret);

  //Read the DO_1 ports
  ret=DAQmxBaseReadDigitalU32(taskHandleDO_1,1,10.0,DAQmx_Val_GroupByChannel,r_dataDO_1,num_portsDO_1,&read,NULL);
  if(ret) ProcessError(ret);

  //Read the DI ports
  ret=DAQmxBaseReadDigitalU32(taskHandleDI,1,10.0,DAQmx_Val_GroupByChannel,r_dataDI,num_portsDI,&read,NULL);
  if(ret) ProcessError(ret);

  ret=DAQmxBaseReadCounterScalarU32 (taskHandleC, 1., &Counts, NULL);
  if(ret) ProcessError(ret);

  return ret;
}

int NIDaqClass::Write(uInt32 a, uInt32 b){

  printf("NiDaqClass::Write %d %d\n", a, b);
  
  uInt32       w_data [2];
  int32        written;
  w_data[0]=a;
  w_data[1]=b;
  int ret=DAQmxBaseWriteDigitalU32(taskHandleDO_0,1,0,10.0,DAQmx_Val_GroupByChannel,w_data,&written,NULL);
  if(ret) ProcessError(ret);


  w_data[0]=b;
  w_data[1]=a;
  ret=DAQmxBaseWriteDigitalU32(taskHandleDO_1,1,0,10.0,DAQmx_Val_GroupByChannel,w_data,&written,NULL);
  if(ret) ProcessError(ret);


  return ret;
}


int NIDaqClass::Init(){
  int ret;
  if(debug)printf("Init Started\n");
    
  Counts=0;
  //Stop the tasks if they exists
  Stop();

  //Create the DO_0 Task
  ret=DAQmxBaseCreateTask ("", &taskHandleDO_0);
  if(ret) ProcessError(ret);

  ret=DAQmxBaseCreateDOChan(taskHandleDO_0,chan1,"",DAQmx_Val_ChanForAllLines);
  if(ret) ProcessError(ret);


  //Create the DO_1 Task
  ret=DAQmxBaseCreateTask ("", &taskHandleDO_1);
  if(ret) ProcessError(ret);
  
  ret=DAQmxBaseCreateDOChan(taskHandleDO_1,chan2,"",DAQmx_Val_ChanForAllLines);
  if(ret) ProcessError(ret);


  //Create the DI Task
  ret=DAQmxBaseCreateTask ("", &taskHandleDI);
  //  ret=DAQmxBaseLoadTask ("diginput", &taskHandleDI);
  if(ret) ProcessError(ret);
  
  ret=DAQmxBaseCreateDIChan(taskHandleDI,chan3,"",DAQmx_Val_ChanForAllLines);
  if(ret) ProcessError(ret);
  
  //Create the Counter Task
  ret=DAQmxBaseCreateTask ("", &taskHandleC);
  //  ret=DAQmxBaseLoadTask ("counter", &taskHandleC);
  if(ret) ProcessError(ret);
  ret=DAQmxBaseCreateCICountEdgesChan (taskHandleC, "Dev1/ctr0", 
				       NULL, DAQmx_Val_Falling,
 				       0, DAQmx_Val_CountUp);



  ret=DAQmxBaseStartTask (taskHandleDO_0);
  if(ret) ProcessError(ret);

  ret=DAQmxBaseStartTask (taskHandleDO_1);
  if(ret) ProcessError(ret);
  //Rset Oitput channels
  ret=Write(0,0);
  if(ret) ProcessError(ret);
  
  //Start the tasks
  ret=DAQmxBaseStartTask (taskHandleDI);
  if(ret) ProcessError(ret);
  
  
  ret=DAQmxBaseStartTask (taskHandleC);
  if(ret) ProcessError(ret);


  if(debug)printf("Init done\n");
  return 0;
}


int NIDaqClass::ResetCounter(){
  int ret =0;
  if(taskHandleC != 0){
    ret=DAQmxBaseStopTask (taskHandleC);
    if(ret) ProcessError(ret);
    ret=DAQmxBaseClearTask (taskHandleC);
    if(ret) ProcessError(ret);
  }
  ret=DAQmxBaseCreateTask ("", &taskHandleC);
  //  ret=DAQmxBaseLoadTask ("counter", &taskHandleC);
  if(ret) ProcessError(ret);
  ret=DAQmxBaseCreateCICountEdgesChan (taskHandleC, "Dev1/ctr0", 
				       NULL, DAQmx_Val_Falling,
				       0, DAQmx_Val_CountUp);
  if(ret) ProcessError(ret);
  ret=DAQmxBaseStartTask (taskHandleC);
  if(ret) ProcessError(ret);
  
  return ret;
}

void NIDaqClass::ProcessError(int error){
  char errBuff[2048];
  
  if (DAQmxFailed (error)){
    DAQmxBaseGetExtendedErrorInfo (errBuff, 2048);
    printf ("DAQmxBase Error %d: %s\n", error, errBuff);
  }
  return;
}
