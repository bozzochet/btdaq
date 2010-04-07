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

int stopvar;

void DenyConnection(int connectSocket);
int ProcessCommand(int cSock, char *rr);
void HandleConnection(int connectSocket);
int OpenSocket();


void Stop();

void Stop(){
  printf("SIGTERM received\n");
  stopvar=0;
  return;
}

int ChildPid;




int OpenSocket(){
  unsigned short int listenPort=1700;
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

int main(){
  signal(SIGTERM,Stop);
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
    
    // set the Sample timeout of 2.03 secs:
    tv.tv_sec = 2;
    tv.tv_usec = 30000;

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
      int clientAddressLength=sizeof(clientAddress);
      int connectSocket=accept(lsock,
			   (struct sockaddr *) &clientAddress,
			   &clientAddressLength);
      
      if(connectSocket<0) continue;
      OpenConnections++;

      //IF IS THE FIRST CONNECTION FORK A CHILS TO ANSWER
      if(OpenConnections==1){
	ChildPid=fork();
	if(ChildPid==0){ //Child
	  HandleConnection(connectSocket);
	  close(connectSocket);
	  exit(0);
	}
	else {//father
	  close(connectSocket);
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



void DenyConnection(int connectSocket){
  char ss[120];

  sprintf(ss,"Control Server for the NI-6105 Digital I/O box \n");
  send(connectSocket,ss,strlen(ss)+1,0);
  sprintf(ss,"Too many open connections  \n");
  send(connectSocket,ss,strlen(ss)+1,0);
  return;
}


void HandleConnection(int connectSocket){
  char ss[120];
  char rr[120];

  sprintf(ss,"Control Server for the NI-6105 Digital I/O box \n");
  send(connectSocket,ss,strlen(ss)+1,0);
  while(1){
    recv(connectSocket,rr,strlen(rr)+1,MSG_WAITALL);
    int opt=ProcessCommand(connectSocket, rr);
    if(opt==-1) break;
  }
  sprintf(ss,"Closing Connection \n");
  send(connectSocket,ss,strlen(ss)+1,0);
  
  return;
}


int ProcessCommand(int connectSocket, char *rr){
  char ss[120];
  //PING
  if(strstr(rr,"PING")){
    sprintf(ss,"PONG\n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return 1;
  }
  //EXIT
  else if(strstr(rr,"EXIT")){
    sprintf(ss,"BYE \n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return -1;
  }
  else{
    sprintf(ss,"UNKNOWN COMMAND \n");
    send(connectSocket,ss,strlen(ss)+1,0);
    return 0;
  }
  return -1;
}
