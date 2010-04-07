#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <math.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include "CommClass.hh"

CommClass::CommClass(char *add,int PORT){

  sprintf(address,"%s",add);
  port=PORT;
  sock_id=0;
  childpid=0;
  pri=1;
}



int CommClass::StopFile(){
  int *stat;
  int ret;
  ret=kill(childfilepid,SIGKILL);
  if(ret==0){
    waitpid(childfilepid,stat,0); 
    childpid=0; 
    return 0;
  } else{
    if(pri) printf("CommClass::StopFile, cannot kill process %d\n",childpid);
    return ret;
  }
}


int CommClass::StopLog(){
  int stat;
  int ret;
  ret=kill(childpid,SIGKILL);
  if(ret==0){
    waitpid(childpid,&stat,0); 
    childpid=0; 
    return 0;
  } else{
    if(pri) printf("CommClass::StopLog, cannot kill process %d\n",childpid);
    return ret;
  }
}



int CommClass::GetLog(char* outbuff) {  
  int ret;
  int retval;
  char command[80];
  char buff[81];
  char buff2[81];
 
  int pid=fork();
 
  if(!pid){
    FILE* fdout=fopen(outbuff,"w");
    ret=OpenConnection();

    if( ret<=0) return 1;
    
    sprintf(command,"GETLOG\n");
    int lenght=strlen(command);
    //sprintf(buff,"%s\n",command);
    write(ret, command,lenght);

    int kk;
    while (1) {
      retval = read (ret, buff, sizeof(buff)-1);
      //      printf("GETLOG in retval %d \n",retval);
      kk=retval/sizeof(buff);
      //printf("-------------> %d  %d\n",retval,kk);
      buff[retval]='\0';
      // printf("%s",buff);fflush(stdout);
      fprintf(fdout,"%s",buff);fflush(fdout);
    }
    return 0;
  }
  else {
    childpid=pid;
    return pid;
  }
}

int CommClass::GetFile(char* filename,int fdout) {  
  int ret;
  int retval;
  char command[80];
  char buff[255];
 
  int pid=fork();
 
  if(!pid){
    ret=OpenConnection();

    if( ret<=0) return 1;
    
    sprintf(command,"GETFILE %s",filename);
    int lenght=strlen(command);
    sprintf(buff,"%s\n",command);
    write(ret, buff,lenght+1);
    
    while (1) {
    
      retval = read (ret, buff, sizeof (buff));
      write(fdout,buff,retval);
    }
    return 0;
  }
  else {
    childpid=pid;
    return pid;
  }
}


int CommClass::TestConnection() {

  int ret;
  char ans[80];
  if(sock_id<=0) sock_id=OpenConnection();
  if(sock_id<=0) { printf("TestConnection: Cannot Open connection:");return -1;}

  ret=SendCommand("PING",ans,20);
  if(ret>0&&strstr(ans,"PONG")){
    if(pri) printf(" TestConnection: OK\n"); 
    return 1;
  } else {
    if(pri) printf(" TestConnection: No Connection\n"); 
    sock_id=0;
    return 0;
  }
  
}


int CommClass::OpenConnection() {
  /*
   It Open a connection to theselected port at the rquired adress
   Upon sucess it returns the socket file descriptor (>0)
   Otherwise it returns the following error codes:

   -1 Illegal host name or address 
   -2 Socket creation Failed 
   -3 Connect Failed
   -4 Deamon do not answer
  */
  struct sockaddr_in remote;
  struct hostent * host;
  int socket_id;
  int ret;



  /* CALL SOCKET */
  socket_id=socket(PF_INET, SOCK_STREAM, 0);
  if(socket_id==-1){
    perror("OpenConnection:Socket creation Failed");
    return -2;
  }
  /* CONVERT  address */
  host=gethostbyname(address);
  if(host==NULL){
    perror("OpenConnection: Illegal host name or address ");
    return -1;
  }
  
  remote.sin_addr=*((struct in_addr*) host->h_addr);
  remote.sin_family=AF_INET;
  remote.sin_port=htons(port);

  /* call connect */
  ret=connect(socket_id,(struct sockaddr*)&remote, sizeof(struct sockaddr_in));
  if(ret==-1){
    perror("OpenConnection:Connect  Failed");
    return -3;
  }
  if(pri) printf("OpenConnection: Up and running\n");
  sock_id=socket_id;
  return socket_id;
  
}

int CommClass::SendCommand(char* command,char *answer, float timeout,int no_close){

  int retsel;
  int ret;
  struct timeval seltimeout;
  fd_set read_set;

  char buff[255];

  if(sock_id<=0) sock_id=OpenConnection();
  if(sock_id<=0) { printf("SendCommand: Cannot Open connection:");return -1;}

  int lenght=strlen(command);
  sprintf(buff,"%s\n",command);
  write(sock_id, buff,lenght+1);

  FD_ZERO(&read_set);
  FD_SET(sock_id, &read_set);
  
  /* Timeout   secondi */
  seltimeout.tv_sec  = timeout;
  seltimeout.tv_usec = 0;
  //  seltimeout.tv_usec = (timeout-seltimeout.tv_sec)*10E6;

  retsel = select(sock_id + 1, &read_set, NULL, NULL, &seltimeout);
  
  if (retsel == -1) {
    perror("SendCommand: select Failed");
    close(sock_id);
    sock_id=0;
    return -2;
  } else if (retsel) {
    /* Ci sono dati disponibili, posso iniziare una lettura */
    ret = read (sock_id, buff, sizeof (buff));
    if (ret <= 0) {
      perror ("SendCommand: read error");
      return -3;
    }
    if(pri) write (1, buff, ret);
    sprintf(answer,"%s",buff);
    if(!no_close){
      close(sock_id);
      sock_id=0;
    }
    return 1;
  } else {
    perror("SendCommand: DAQ does not answer: check connection? \n");
    close(sock_id);
    sock_id=0;
    return -4;
    /* Timeout: Nessun dato disponibile entro 10.5 secondi. Il
     * programma agisce di conseguenza (eg. chiude il socket).
     */
  }
  
  perror("SendCommand: How in the Hell did you reached this point? \n");
 return -7;
}


int CommClass::SendCommandWithReply(char *command,
				    char *goodanswer,
				    char* badanswer,
				    char* funame,
				    float timeout){

  char answer[255];
  int ret;
  ret=SendCommand(command,answer,timeout);
  if(ret<0) {
    if(pri) printf(" %s problem sending the command\n",funame);
    return -2;
  }
  if(strstr(answer,goodanswer)) {
    return 1;
  }else if (strstr(answer,badanswer)){
    return 0;
  }else{
    if(pri) printf(" %s Unkown answer there is some problem\n",funame);
    return -1;
  }
  
}
