#ifndef CommClass_hh
#define CommClass_hh
#include <unistd.h>
#include <cstdio>

class CommClass {
  
public:
  char address[80];
  int  port;
  
  
protected:
  int childpid;
  int childfilepid;
  int pri;
  char answer[255];
  int sock_id;

public:
  CommClass(char *add="localhost",int PORT=6666);
  virtual ~CommClass() {};
  
  void PrintOn()  {pri=1;}
  void PrintOff() {pri=0;}

  int  OpenConnection();
  int  TestConnection();
  //! Send a command, returns <0 on error and 1 on success  
  int  SendCommand(char* command,char *answer, float timeout,int no_close=0);
  int  SendCommandWithReply(char *command,char *goodanswer,char* badanswer,char* funame,float timeout);

  inline void CloseConnection(){if(pri)printf("Closing Connection \n"); if(sock_id>0) close(sock_id); sock_id=0;}

  int  GetLog(char* buffout) ;
  int  StopLog();

  int GetFile(char *filename,int fdout=1) ;
  int StopFile();


};

#endif
