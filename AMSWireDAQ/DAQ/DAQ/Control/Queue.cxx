#include "Queue.h"
//#include <stdio.h>

static void catch_alarm(int sig_num){
  signal(SIGALRM,catch_alarm);
}

Queue::Queue(const char *file, int id){
  key=ftok(file,id);
  msgid = msgget(key,0660 | IPC_CREAT);
  signal(SIGALRM,catch_alarm);
  mybuf=new my_msgbuf;
  logbuf=new log_msgbuf;

  // let's init mybuf and logbuf. non-init was pointed out by valgrind
  mybuf->mtype=0;
  mybuf->par1=0;
  mybuf->par2=0;
  logbuf->mtype=0;
  logbuf->priority=0;
  memset(logbuf->msg, 0, sizeof(logbuf->msg));

  // for debugging
  //printf("msgid=%d, file=%s\n", msgid, file);
}

Queue::~Queue(){
}


int Queue::NumberOfMessages(){
  msgctl(msgid,IPC_STAT, &mymsg_ds);
  int messages = mymsg_ds.msg_qnum;
  return messages;
}

void Queue::Empty(){
  int messages = NumberOfMessages();
  if (messages > 0)
    for (int i=0;i<messages;i++)
      msgrcv(msgid, (struct msgbuf *)NULL,sizeof(NULL),0,IPC_NOWAIT);
}

void Queue::Send(long mtype, int par1, int par2){
  mybuf->mtype=mtype; 
  mybuf->par1=par1; 
  mybuf->par2=par2; 
  msgsnd(msgid, (struct msgbuf *)mybuf,sizeof(*mybuf) - sizeof(mybuf->mtype) ,0); // the size has to be the one of the msg itself, so we must remove the size of mtype.
}
void Queue::Send(long mtype, const char* msg, int priority){
  logbuf->mtype    = mtype; 
  strcpy(logbuf->msg,msg); 
  logbuf->priority = priority; 
  msgsnd(msgid, (struct msgbuf *)logbuf,sizeof(*logbuf) - sizeof(logbuf->mtype) ,0); // the size to indicate is the one of the MESSAGE itself, not the structure
}

void Queue::Receive(long *mtype, int *par1, int *par2){
  mybuf->mtype=0; 
  mybuf->par1=0; 
  mybuf->par2=0; 
  msgrcv(msgid, (struct msgbuf *)mybuf,sizeof(*mybuf),0,IPC_NOWAIT);
  *mtype=mybuf->mtype;     
  *par1=mybuf->par1;     
  *par2=mybuf->par2;     
}

void Queue::Wait(int mty, long *mtype, int *par1, int *par2){
  mybuf->mtype=0; 
  mybuf->par1=0; 
  mybuf->par2=0; 
  msgrcv(msgid, (struct msgbuf *)mybuf,sizeof(*mybuf) - sizeof(mybuf->mtype) ,mty,0);
  *mtype=mybuf->mtype;     
  *par1=mybuf->par1;     
  *par2=mybuf->par2;     
}
void Queue::Wait(int mty, long *mtype, char* msg, int *priority){
  logbuf->mtype=0; 
  strcpy(logbuf->msg,""); 
  logbuf->priority=-1; 
  msgrcv(msgid, (struct msgbuf *)logbuf,sizeof(*logbuf) - sizeof(logbuf->mtype),mty,0);
  *mtype=logbuf->mtype;     
  strcpy(msg,logbuf->msg);     
  *priority=logbuf->priority;
  //printf("msgid %d, priority=%d, mess: %s\n", msgid, logbuf->priority, msg);     
}


void Queue::TimeOut(mQUEUES mty, int *par1, int *par2, int timeout){
  TimeOut((int)mty,par1,par2,timeout);
}

void Queue::TimeOut(int mty, int *par1, int *par2, int timeout){
  alarm_on=0;
  mybuf->mtype=mty; 
 
  alarm(timeout); 
  timeout = msgrcv(msgid, (struct msgbuf *)mybuf,sizeof(*mybuf) - sizeof(mybuf->mtype),mty,0);
  alarm(0);
  if (timeout == -1) {
    *par1=kTimeOut;
    *par2=errno;
  }
  else {
    *par1=mybuf->par1;     
    *par2=mybuf->par2;     
  }
}


void Queue::Delete(){
  msgctl(msgid,IPC_RMID,0);
  delete mybuf;
  delete logbuf;
}
