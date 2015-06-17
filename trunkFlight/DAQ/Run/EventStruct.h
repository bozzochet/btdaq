#ifndef EVENTSTRUCT_H
#define EVENTSTRUCT_H
struct EventX5Header
{
	unsigned long RunNumber;	//
	char day;		//start date and time
	unsigned char month;
	unsigned char year;             //-2000
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned short EventVersion;	//version of the OneEvent struct used 
	unsigned long EventLength;	//length of the OneVent struct used
	unsigned long Status;		//used for shaping time conf. 
	char comment[300];		//null terminated string for comment
		
};

struct EventX5
{
	unsigned long EvtNumber;       //BIT 31: 1 in spill / 0 out of spill
	unsigned long TimeStamp;       //millisecond since starttime;
	
	unsigned char TelADC_1[768];
	unsigned char TelADC_2[768];
	unsigned char TelADC_3[768];
	unsigned char TelADC_4[768];

	unsigned short S_11[320];
	unsigned short S_12[320];
	unsigned short K_1[384];
	unsigned short S_21[320];
	unsigned short S_22[320];
	unsigned short K_2[384];
};	                              //so size is 7176 bytes

#endif  /* EVENTSTRUCT_H */
