#include "Share.h"

Share::Share(char *file, int id,  int size){
  key=ftok(file,id);
  shm_size=size;
  shmid=shmget(key,shm_size,IPC_CREAT|0660);
  shmaddr=(char *)shmat(shmid,0,0);
}

Share::~Share(){
}


void Share::Detach(){
  shmdt(shmaddr);
}
void Share::Delete(){
  shmctl(shmid,IPC_RMID,&shmdesc);
}

void Share::Empty() {
  memset(shmaddr,0,shm_size);
}






