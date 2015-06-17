#include <sys/time.h>
#include <sys/stat.h>
#include "dampe.h"
#include "PUtil.h"
c_dampe::c_dampe(char *name, char *conf_file, int address, AMSWcom *node_in):JinjSlave(name, conf_file, address, node_in){
	strcpy(myname, name);
	mask.Nmask = 0;
	selfaddress = address;
	if (selfaddress == 0xffff)
		broadcastadd = 0x4000;
	else
		broadcastadd = (0x4000<<16)|selfaddress;
	node = node_in;
	CPars = new ConfPars();
	CPars->ReadConfig(conf_file);
	/* initialize communication */
	printf("[>>> waiting for Dampe @ %s:%s]\n", CPars->dampeIP, CPars->dampePORT);
	if (client_connect(CPars->dampeIP, CPars->dampePORT) != d_dampe_socket_null) {
		printf("[>>> dampe is \x1b[32mONLINE\x1b[0m]\n");

		this->client_send("list\n");
		this->client_send("mask -m 01\n");
		this->client_send("stream -o /dev/null\n");
		this->client_send("trigger -off\n");
		this->client_send("write -x 040700\n"); /* put Dampe in acquisition mode because in IDLE mode it sends empty packets */
	} else
		printf("[>>> dampe is \x1b[31mOFFLINE\x1b[0m]\n");
}

//--------------------------------------------------------------
c_dampe::~c_dampe(){
	client_send("quit\n");
	if (client_socket != d_dampe_socket_null) {
		shutdown(client_socket, SHUT_RDWR);
		close(client_socket);
		client_socket = d_dampe_socket_null;
	}
	if(CPars)
		delete CPars;
}

int c_dampe::client_connect(const char *address, const char *port) {
	struct addrinfo hints = {AI_PASSIVE, AF_INET, SOCK_STREAM}, *response;
	int flags, connected = false;
	client_socket = d_dampe_socket_null;
	if (getaddrinfo(address, port, &hints, &response) == 0) {
		if ((client_socket = socket(response->ai_family, response->ai_socktype, response->ai_protocol)) != d_dampe_socket_null) {
			if (connect(client_socket, response->ai_addr, response->ai_addrlen) == 0)
				if ((flags = fcntl(client_socket, F_GETFL)) != d_dampe_socket_null) {
					fcntl(client_socket, F_SETFL, flags|O_NONBLOCK);
					connected = true;
				}
			if (!connected) {
				shutdown(client_socket, SHUT_RDWR);
				close(client_socket);
				client_socket = d_dampe_socket_null;
			}
		}
		freeaddrinfo(response);
	}
	return client_socket;
}

int c_dampe::client_send(const char *buffer) {
	int result = false;
	char backup[d_dampe_string_buffer];
	if ((client_socket != d_dampe_socket_null) && (buffer)) {
		memset(backup, 0, d_dampe_string_buffer);
		snprintf(backup, d_dampe_string_buffer, "%s", buffer);
		if ((write(client_socket, backup, strlen(backup)) > 0)) {
			usleep(250000);
			result = true;
		}
	}
	return true;
}

//--------------------------------------------------------------
int c_dampe::Init() {
	//client_send("trigger -off\n");
	//client_send("write -x 040700\n");
	printf("[>>> initializing dampe (reset everything)]\n");
	return 0;
}

int c_dampe::SetDelay(){
	printf("[>>> dampe SET delay (do nothing)]\n");
	return 0;
}

int c_dampe::SetMode(int modein) {
	char buffer[d_dampe_string_buffer];
	if (modein) {
		if (strlen(CPars->REMOTEPATH) > 0) {
			memset(buffer, 0, d_dampe_string_buffer);
			client_send("list\n");
			snprintf(buffer, d_dampe_string_buffer, "stream -o %s/DAMPE_%s_%d\n", CPars->REMOTEPATH, myaction, timestamp);
			client_send(buffer);
			client_send("trigger -ext\n");
		}
	} else {
		client_send("list\n");
		client_send("trigger -off\n");
		client_send("stream -o /dev/null\n");
	}
	printf("[>>> dampe ... setting mode %d]\n", modein);
	return 0;
}

int c_dampe::GetEventNumber() {
	//	printf("[>>> getting events]\n");
	return 0;
}

char* c_dampe::PrintAllEventNumber(int log,int JLV1num) {
	int ret=0;
	static char numbers[1023]="";
	// snprintf(numbers, 1023, "Dampe %02d: %6d", selfaddress, 0);
	// printf("[>>>>>] dampe: %s\n", numbers); 
	return numbers;
}

int c_dampe::EventReset() {
	// client_send("write -x 020400\n");
	// client_send("write -x 040700\n");
	// printf("[>>>>>] resetting events (reinitialize)\n");
	return 0;
}

