#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <limits.h>
#include <time.h>
#include <ctime>
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <bitset>
#include "CQuickUsb.cxx"

//QuickUSB settings
#define SETTING_EP26CONFIG                      0
#define SETTING_WORDWIDE                        1
#define SETTING_DATAADDRESS                     2
#define SETTING_FIFO_CONFIG                     3
#define SETTING_FPGATYPE                        4
#define SETTING_CPUCONFIG                       5
#define SETTING_SPICONFIG                       6
#define SETTING_SLAVEFIFOFLAGS                  7
#define SETTING_I2CTL                           8
#define SETTING_PORTA                           9
#define SETTING_PORTB                           10
#define SETTING_PORTC                           11
#define SETTING_PORTD                           12
#define SETTING_PORTE                           13
#define SETTING_PORTACCFG                       14
#define SETTING_PINFLAGS                        15
#define SETTING_VERSIONBUILD                    16
#define SETTING_VERSIONSPEED                    17

#define BUFFER_LENGTH 128


using namespace std;


//LETTURA ID SCHEDA

int readID_qusb (CQuickUsb * fQusb, unsigned short int * ID_Qusb) 
  {
    int result;

    unsigned short int indirizzo=0x00;
    unsigned short int lung=2;
    unsigned char valore_r[2];
 
    result=fQusb->ReadCommand(indirizzo,valore_r,&lung);

    unsigned char *c; 

    c = (unsigned char *)ID_Qusb;

    *c=valore_r[0];
    *(c+1)=valore_r[1];

    return result;
  }


//LETTURA VERSIONE SCHEDA

int readVersion_qusb (CQuickUsb * fQusb, float * version_Qusb) 
  {
    int result;

    unsigned short int indirizzo=0x84;
    unsigned short int lung=2;
    unsigned char valore_r[2];
 
    result=fQusb->ReadCommand(indirizzo,valore_r,&lung);

    unsigned char *c; 
    unsigned short int i;
	
    i = 0;	
    c = (unsigned char *)&i;

    *c=valore_r[0];
    *(c+1)=valore_r[1];

    *version_Qusb = ((float)((i & 0xF000) >> 12) * 10) + ((float)((i & 0x0F00) >> 8) * 1) + ((float)((i & 0x00F0) >> 4) * 0.1 ) + ((float)((i & 0x000F) >> 0) * 0.01);

    return result;
  }


//LETTURA STATUS REGISTER

int read_status_register (CQuickUsb * fQusb, unsigned short int * status_register) 
  {
    int result;

    unsigned short int indirizzo=0x81;
    unsigned short int lung=2;
    unsigned char valore_r[2];
 
    result=fQusb->ReadCommand(indirizzo,valore_r,&lung);

    unsigned char *c; 
    c = (unsigned char *)status_register;

    *c=valore_r[0];
    *(c+1)=valore_r[1];

    return result;
  }


//SCRITTURA TRG_ENABLE

int write_trg_enable (CQuickUsb * fQusb) 
  {

    int result;
    
    unsigned short int indirizzo=0x82;
    unsigned short int lung=2;
    unsigned char valore_w[2];
    
    valore_w[0]='a';
    valore_w[1]='z';

    result=fQusb->WriteCommand(indirizzo,valore_w,lung);
 
    return result;
  }


//IMPOSTAZIONI PARAMETRI FUNZIONAMENTO DEL MODULO QUICKUSB

int set_qusb (CQuickUsb * fQusb) 
  {

    int result_read;
    int result_write;

    unsigned short int read_wordwide;
    unsigned short int read_dataaddress;
    unsigned short int read_cpuconfig;
    unsigned short int read_spiconfig;
    
    unsigned short int write_wordwide;
    unsigned short int write_dataaddress;
    unsigned short int write_cpuconfig;
    unsigned short int write_spiconfig;
    
    unsigned long temp;
    
    result_read = fQusb->ReadSetting(SETTING_WORDWIDE, &read_wordwide);
    
    if (result_read==0) 
      {  
	return result_read;
      }
    else
      {
	temp=read_wordwide;
	bitset<16> manip_1 (temp);
	manip_1.set(0,1);
	temp=manip_1.to_ulong();
	write_wordwide=temp; 
	
	result_write = fQusb->WriteSetting(SETTING_WORDWIDE, write_wordwide);
	
	if(result_write==0)
	  {
	    return result_write;
	  } 
      }
    
    result_read = fQusb->ReadSetting(SETTING_DATAADDRESS, &read_dataaddress);
    
    if (result_read==0) 
      {  
	return result_read;
      }
    else
      {
	temp=read_dataaddress;
	bitset<16> manip_2 (temp);
	manip_2.set(15,1);
	manip_2.set(14,0);
	temp=manip_2.to_ulong();
	write_dataaddress=temp; 
	
	result_write = fQusb->WriteSetting(SETTING_DATAADDRESS, write_dataaddress);

	if(result_write==0)
	  {
	    return result_write;
	  } 
      } 

    result_read = fQusb->ReadSetting(SETTING_CPUCONFIG, &read_cpuconfig);
   
    if (result_read==0) 
      {  
	return result_read;
      }
    else
      {  
	temp=read_cpuconfig;
	bitset<16> manip_3 (temp);
	manip_3.set(15,1);
	manip_3.set(4,1);
	manip_3.set(3,0);
	manip_3.set(2,0);
	manip_3.set(1,0);
	temp=manip_3.to_ulong();
	write_cpuconfig=temp;
	
	result_write = fQusb->WriteSetting(SETTING_CPUCONFIG, write_cpuconfig);

	if(result_write==0)
	  {
	    return result_write;
	  }  
      }
    
    result_read = fQusb->ReadSetting(SETTING_SPICONFIG, &read_spiconfig);
    
    if (result_read==0) 
      {
	return result_read;
      }
    else
      {
	temp=read_spiconfig;
	bitset<16> manip_4 (temp);
	manip_4.set(15,1);
	temp=manip_4.to_ulong();
	write_spiconfig=temp;
	
	result_write = fQusb->WriteSetting(SETTING_SPICONFIG, write_spiconfig);  

	if(result_write==0)
	  {
	    return result_write;
	  } 
      }

    return 1;
  }

//SCRITTURA DUMMY

int ROC_dummy (CQuickUsb * fQusb)
  {
	unsigned short int settings=0x2046;

	int result;

    unsigned short int indirizzo=0x42;
    unsigned short int lung=2;
    unsigned char valore_w[2];

    unsigned char * c;

    c = (unsigned char *)&settings;

    valore_w[0] = *c;
    valore_w[1] = *(c+1);

    result=fQusb->WriteCommand(indirizzo,valore_w,lung);

    return result;
  }

//SCRITTURA NEL REGISTRO DI IMPOSTAZIONI DELLA ROC

int write_ROC_settings (CQuickUsb * fQusb, unsigned short int * settings) 
  {
    int result;
    
    unsigned short int indirizzo=0x83;
    unsigned short int lung=2;
    unsigned char valore_w[2];

    unsigned char * c;
    
    c = (unsigned char *)settings;

    valore_w[0] = *c;
    valore_w[1] = *(c+1);

    result=fQusb->WriteCommand(indirizzo,valore_w,lung);
 
    if(result==0)
    	return result;
    else
    	return ROC_dummy(fQusb);
  }


//LETTURA DEL REGISTRO IMPOSTAZIONI DELLA ROC (per debug)

int read_ROC_settings (CQuickUsb * fQusb, unsigned short int * ROC_settings) 
  {
    int result;

    unsigned short int indirizzo=0x85;
    unsigned short int lung=2;
    unsigned char valore_r[2];
 
    result=fQusb->ReadCommand(indirizzo,valore_r,&lung);

    unsigned char *c; 

    c = (unsigned char *)ROC_settings;

    *c=valore_r[0];
    *(c+1)=valore_r[1];

    return result;
  }

int read_data (CQuickUsb * fQusb, ofstream *ofs) 
  {
    int result;
   
#if defined(SPS2016) || defined (SPS2017Aug) || defined (SPS2017Nov)
    unsigned long int length=1735;
#else
    unsigned long int length=178;
#endif
    unsigned long int n_word = 2*length;
    unsigned char buf[n_word];
    unsigned short int data[length];

    unsigned int i, j;
    
    unsigned char * c;
    c = (unsigned char *)data;

    result = fQusb->ReadData(buf, &n_word);
    
    if(result!=0) 
      {   
	if (ofs->is_open())
	  {
	    for(i=0; i<length; i++)
	      {
		for(j=2*i; j<2*(i+1); j++)
		  {
		    *(c+j)=buf[j];          
		  }
		ofs->write((char*)&data[i], 2);
	      } 
	  }
      }
    return result;
  }
