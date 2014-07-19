#ifndef SMALLNETSERVER_H
#define SMALLNETSERVER_H

#include "NIDaqClass.hh"

class SmallNetServer {
private:
  int ChildPid;
  static int stopvar;
  unsigned short int listenPort;

  void DenyConnection(int connectSocket);
  int ProcessCommand(int cSock, char *rr);
  void HandleConnection(int connectSocket);
  int OpenSocket();
  NIDaqClass* nidaq;

public:
  SmallNetServer(NIDaqClass* pp);
  ~SmallNetServer();


  int Start();
  
  static void Stop(int signal);

};

#endif

