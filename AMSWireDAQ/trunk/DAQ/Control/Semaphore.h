#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <iostream.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


class Semaphore {
 public:
  Semaphore(int value);
  ~Semaphore();
  
  void Lock(int sem);
  void Release(int sem);

 private:
  key_t key;
  int semid;
  int sem_num;
  struct sembuf semop;
  union  semun  semval;
};


#endif /*SEMAPHORE_H*/

