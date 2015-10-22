#ifndef DAMPE_H
#define DAMPE_H
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "amswcom.h"
#include "JinjSlave.h"
#define d_dampe_socket_null -1
#define d_dampe_string_buffer 512
class c_dampe: public JinjSlave{
private:
	int client_socket;
public:
	c_dampe(char* name,char* conf_file,int address,AMSWcom* node);
	~c_dampe();
private:
	int client_connect(const char *address, const char *port);
	int client_send(const char *buffer);
public:
	int Init();
	int SetDelay();
	int SetMode(int modein);
	int GetEventNumber();
	char* PrintAllEventNumber(int log=1,int JLV1num=0);
	int EventReset();
};
#endif
