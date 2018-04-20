#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "SmallNetServer.hh"

int SmallNetServer::stopvar=0;

void SmallNetServer::Stop(int signal){
  printf("SIGTERM received\n");
  stopvar=0;
  return;
}


SmallNetServer::SmallNetServer(NIDaqClass * pp){

  ChildPid=0;
  listenPort=1700;
  stopvar=0;
  nidaq= pp;
  if(nidaq)nidaq->Init();

}

SmallNetServer::~SmallNetServer(){

  //  if(nidaq) delete nidaq;

}



int SmallNetServer::OpenSocket(){

  struct sockaddr_in  serverAddress;

  int listenSocket=socket(AF_INET, SOCK_STREAM,0);
  if(listenSocket<0){
    perror("Cannot create listen socket\n");
    return listenSocket;
  }

  serverAddress.sin_family=AF_INET;
  serverAddress.sin_addr.s_addr=htonl(INADDR_ANY);
  serverAddress.sin_port=htons(listenPort);

  int ret=bind(listenSocket,(struct sockaddr *) &serverAddress, sizeof(serverAddress));
  
  if(ret<0){  perror("Cannot bind socket\n"); return ret;}
  
  ret=listen(listenSocket, 5);

  if(ret<0){  perror("Cannot put the socket to listen state\n"); return ret;}

  printf("Waiting for TCP connection on port %d ...\n",listenPort);
  return listenSocket;

}



int SmallNetServer::Start(){

  signal(SIGTERM,SmallNetServer::Stop);
  stopvar=1;

  //Create the socket
  struct sockaddr_in clientAddress;
  int lsock=OpenSocket();
  if(lsock<0){perror("Fatal Error! \n"); exit(1);}
  
  
  int mx;                  /* Max fd + 1 */
  fd_set rx_set;             /* Read set */
  fd_set wk_set;          /* Working set */
  struct timeval tv;    /* Timeout value */

  FD_ZERO(&rx_set);   /* Init. */
  FD_SET(lsock,&rx_set);    /* + s */
  mx = lsock + 1;    /* max fd + 1 */
  int OpenConnections=0;

  while(stopvar){
    int status;
    if(waitpid(-1,&status,WNOHANG)>0)
      OpenConnections--;

    int z;
    //copy the read set to the working one
    FD_ZERO(&wk_set);
    for ( z=0; z<mx; ++z ) {
      if ( FD_ISSET(z,&rx_set) )
	FD_SET(z,&wk_set);
    }
    
    // set the Sample timeout of 0.2 secs:
    tv.tv_sec = 0;
    tv.tv_usec = 200000;

    //launch the SELECT
    int n = select(mx,&wk_set,NULL,NULL,&tv);
    //TEST THE SELECT RESULT
    if ( n == -1 ) {
      fprintf(stderr,"%s: select(2)\n",
	      strerror(errno));
      continue;
      //      exit(1);
    } else if ( !n ) {
      /* puts("Timeout."); */
      continue;
    }
    
    // ACCEPT THE CONNECTION
    if ( FD_ISSET(lsock,&wk_set) ) {
      socklen_t clientAddressLength=sizeof(clientAddress);
      int connectSocket=accept(lsock,
			   (struct sockaddr *) &clientAddress,
			   &clientAddressLength);
      printf("CONNECTED\n");
      if(connectSocket<0) continue;
      OpenConnections++;

      //IF IS THE FIRST CONNECTION FORK A CHILS TO ANSWER
      if(OpenConnections==1){
	ChildPid=fork();
	if(ChildPid==0){ //Child
	  HandleConnection(connectSocket);
	 fprintf(stdout, "closing client\n");
	 fflush(stdout);
	  close(connectSocket);
	  return 0;
	} else {//father
	  //close(connectSocket);
	  continue;
	}
      }//ELSE DENY CONNECTION !!!
     else if(OpenConnections>1){
	DenyConnection(connectSocket);
	close(connectSocket);
	OpenConnections--;
      }
    }
  }
  close(lsock);
  printf("Server is exiting\n");
  return 0;
}



void SmallNetServer::DenyConnection(int connectSocket){
  char ss[120];

  //sprintf(ss,"Control Server for the NI-6105 Digital I/O box \n");
  //send(connectSocket,ss,strlen(ss)+1,0);
  sprintf(ss,"Too many open connections  \n");
  send(connectSocket,ss,strlen(ss)+1,0);
  return;
}


void SmallNetServer::HandleConnection(int connectSocket){
  char ss[120];
  char rr[120];

  sprintf(ss,"Control Server for the NI-6105 Digital I/O box \n");
  //send(connectSocket,ss,strlen(ss)+1,0);
  while(1){
    	int ret=read(connectSocket,rr,120);
	if (ret > 0) {
    		fflush(stdout);
    		int opt=ProcessCommand(connectSocket, rr);
		if (opt==-1)
			break;
	} else if(ret <= 0) 
		break;
  }
  //sprintf(ss,"Closing Connection \n");
  //send(connectSocket,ss,strlen(ss)+1,0);
  return;
}


int SmallNetServer::ProcessCommand(int connectSocket, char *rr){
  char ss[120];
  //PING
  if(strstr(rr,"PING") || strstr(rr,"ping") || strstr(rr,"Ping")){
    sprintf(ss,"PONG\n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return 1;
  }  //READ
  else if(strstr(rr,"READ") || strstr(rr,"read") || strstr(rr,"Read")){
    int ret=nidaq->Read();
    if(ret>=0)
      nidaq->PrintString(ss);
    else sprintf(ss,"FAILED \n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return 2;
  }
  //WRITE
  else if(strstr(rr,"WRITE") || strstr(rr,"write") || strstr(rr,"Write")){
    char* ff;
    char **endptr;
    int aa[2];
    int kk=0;
    ff=strtok(rr," ");
    endptr=NULL;
    while(ff=strtok(NULL," ")){
      kk++;
      if(kk>2) break;
      aa[kk-1]=atoi(ff);
    }
    int ret=nidaq->Write(aa[0],aa[1]);
    if(ret>=0) sprintf(ss,"DONE \n");
    else sprintf(ss,"FAILED \n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return 2;
  }
  //INIT
  else if(strstr(rr,"INIT") || strstr(rr,"init") || strstr(rr,"Init")){
    int ret=nidaq->Init();
    if(ret>=0) sprintf(ss,"DONE \n");
    else sprintf(ss,"FAILED \n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return 2;
  }
  else if(strstr(rr,"EXIT") || strstr(rr,"exit") || strstr(rr,"Exit")){
    sprintf(ss,"BYE \n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return -1;
  }
  else if(strstr(rr,"RESETCOUNTER") || strstr(rr,"resetcounter") || strstr(rr,"Resetcounter")){
    int ret=nidaq->ResetCounter();
    if(ret>=0) sprintf(ss,"DONE \n");
    else sprintf(ss,"FAILED \n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return 2;
  }
  else{
    sprintf(ss,"UNKNOWN COMMAND \n");
    send(connectSocket,ss,strlen(ss)+1,0);
    sprintf(ss,"Allowed commands are: PING INIT READ WRITE RESETCOUNTER EXIT \n");
    send(connectSocket,ss,strlen(ss)+1,0);

    return 0;
  }
  return -1;
}
