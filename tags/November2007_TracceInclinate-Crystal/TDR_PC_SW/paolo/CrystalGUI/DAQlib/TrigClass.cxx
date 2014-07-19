#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>

#include "TrigClass.hh"

TrigClass::TrigClass(char* address,int port):CommClass(address,port){
//initializing...
	number1=0;
	number2=0;
	number3=0;
	counter=0;
}

int TrigClass::ReadCounter(){
	char command[255];
  char answer[255];
	sprintf(command,"READ");
	SendCommand(command,answer,1,1);
	sscanf(answer,"%X %X %X %d\n",&number1,&number2,&number3,&counter);
//	sscanf(answer,"DO port 0: %X  DO port 1: %X  DI port 2: %X  Counts:      %d)",&number1[255],&number2[255],number3,counter);
	return counter;
}

int TrigClass::ReadInput(){
	char command[255];
  char answer[255];
	sprintf(command,"READ");
	SendCommand(command,answer,1,1);
	sscanf(answer,"%X %X %X %d\n",&number1,&number2,&number3,&counter);
//	sscanf(answer,"DO port 0: %X  DO port 1: %X  DI port 2: %X  Counts:      %d)",&number1[255],&number2[255],number3,counter);
	return number3;
}

int TrigClass::Write(int bit2modify,int value2write){
	int ret=0;
  char command[255];
  char answer[255];
	int temp1=0;
	int temp2=0;
	int bit2modify1=0;
	int bit2modify2=0;
	int OR1=0;
	int OR2=0;
	int AND1=255;
	int AND2=255;
	int ANDvalue2write=255;
	int ORvalue2write=0;
	if (bit2modify<8) {
		bit2modify1=bit2modify;
		AND1=0;
		OR1=255;
		}
	else if (bit2modify<16) {
		bit2modify2=bit2modify-8;
		AND2=0;
		OR2=255;
	}
	else {
		ret=1;
		printf("Error: we have only 2 ports with each 8 channels to write (0-15)...");
		return ret;
	}
	if (value2write==0) {
		ORvalue2write=0;
		ANDvalue2write=255 - (1<<bit2modify);
	}
	else {//any passed number except 0 is took like 1....
		ORvalue2write=0 + (1<<bit2modify);
		ANDvalue2write=255;
	}
	sprintf(command,"READ");
	SendCommand(command,answer,1,1);
//	CloseConnection();
//	usleep(200000);
	sscanf(answer,"%X %X %X %d\n",&number1,&number2,&number3,&counter);
/*
	printf("number1:%X\nnumber1 & (ANDvalue2write|AND1) | (ORvalue2write&OR1):%d\nANDvalue2write:%d\nAND1:%d\nORvalue2write:%d\nOR1:%d\n",
					number1,
					number1 & (ANDvalue2write|AND1) | (ORvalue2write&OR1),
					ANDvalue2write,
					AND1,
					ORvalue2write,
					OR1
	);
*/
  sprintf(command,"WRITE %d %d",
	number1 & (ANDvalue2write|AND1) | (ORvalue2write&OR1),
	number2 & (ANDvalue2write|AND2) | (ORvalue2write&OR2)
	);
	SendCommand(command,answer,1,1);
//	CloseConnection();
//	usleep(200000);
// controlling if the rights number are really written...
	sprintf(command,"READ");
	SendCommand(command,answer,1,1);
	CloseConnection();
	usleep(300000);
	sscanf(answer,"%X %X %X %d\n",&temp1,&temp2,&number3,&counter);
	if ( !(
		(number1 & (ANDvalue2write|AND1) | (ORvalue2write&OR1)) == temp1
		&&
		(number2 & (ANDvalue2write|AND2) | (ORvalue2write&OR2)) == temp2
	) ) {
	printf("Error writing trigger settings on NI...\n");
	ret=1;
	}
	return ret;
}

int TrigClass::TriggerOn(){
	int ret=0;
	ret=Write(0,1);
	return ret;
}

int TrigClass::TriggerOff(){
	int ret=0;
	ret=Write(0,0);
	return ret;
}

int TrigClass::CalibTrig(){
	int ret=0;
	ret=Write(1,0);
	return ret;
}

int TrigClass::BeamTrig(){
	int ret=0;
	ret=Write(1,1);
	return ret;
}
