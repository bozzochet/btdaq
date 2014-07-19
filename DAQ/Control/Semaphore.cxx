#include "Semaphore.h"

Semaphore::Semaphore(char *file, int id, int numsem, int *init){
  key=ftok(file,id);
  sem_num=numsem;
  semid = semget(key, sem_num, IPC_CREAT | 0666);
  
  semval.val=

}

Semaphore::~Semaphore(){
  semctl(semid,0,IPC_RMID,semval);
}


void Semaphore::Lock(int sem){
  semop.sem_num = sem;
  semop.sem_op  = -1;
  semop.sem_flg = 0;
  semop(semid, &semop, 1);
}


void Semaphore::Release(int sem){
  semop.sem_num = sem;
  semop.sem_op  = 1;
  semop.sem_flg = 0;
  semop(semid, &semop, 1);
}
