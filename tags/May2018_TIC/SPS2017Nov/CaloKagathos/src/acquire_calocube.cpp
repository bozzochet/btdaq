#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <bitset>
#include <signal.h>
#include <math.h>
#include "qusb_module.h"

// QuickUSB settings 
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

const char *acq_dir = "/home/testsys/CaloCube/SPS2017Nov/CaloKagathos/acquired_files/";

//GESTIONE INPUT 

int n_ev = 1000; 
int n_file = 1;
int rsth = 0; 
int rsts = 0; 
int reset = 0; 
int repeat = 0; 
int delay_coarse = 7; //NOTE: everytime we overwrite default settins to change this parameter from 4 to 7 us
int delay_fine = 0; 
char *file;
bool change_name = false;
bool force_gain = false;
bool alt_mode = false;
bool set_time = true; //NOTE: everytime we overwrite default settins to change this parameter from 4 to 7 us
bool set_reset = false;

void HandleInputPar(int argc, char **argv)  
{ 

	stringstream usage;
	usage.clear();
	usage << endl;
	usage << "Usage:" << endl << endl;
	usage << "-n\t\t\tnumber of events to be acquired in each file" << endl;
	usage << "-i\t\t\tnumber of files to be acquired in sequence" << endl;
	usage << "-g\t\t\tset gain: force gain" <<endl;
	usage << "-l\t\t\tset mode: alternative mode" <<endl;
	usage << "-a\t\t\tset rsth: 1 -> 40 ns, 15 -> 300 ns (default 5 -> 80 ns)" <<endl;
	usage << "-b\t\t\tset rsts: 1 -> 0 ns, 15 -> 500 ns (default 4 -> 50 ns)" <<endl;
	usage << "-c\t\t\tset reset: 1 -> 0 ns, 15 -> 400 ns (default 15 -> 400 ns)" <<endl;
	usage << "-t\t\t\tset period between reset: 1 -> 10 us, 15 -> 2 ms (default 3 -> 20 us)" <<endl;
	usage << "-d\t\t\tset gain hold delay coarse: 1 -> 1 us, 15 -> 100 us (default 4 -> 4 us)" <<endl;
	usage << "-y\t\t\tset gain hold delay fine: 1 -> 100 ns, 15 -> 1500 ns (default 2 -> 200 ns)" <<endl;
	usage << "-o\t\t\toutput file, default: %Y%m%d_%H%M%S.dat" << endl;
	usage << "-h\t\t\thelp" << endl;

	int c;

	while ((c = getopt(argc, argv, "n:i:glo:a:b:c:t:d:y:h")) != -1){
		switch (c) {
		case 'n':
			n_ev = atoi(optarg);
			if (n_ev <= 0) {
				cout << "Warning: bad number of events. Exit.\n";
				exit(1);
			}
			break;
		case 'i':
			n_file = atoi (optarg);
			if (n_file <= 0) {
				cout << "Warning: bad number of files. Exit.\n";
				exit(1);
			}
			break;
		case 'g':
			force_gain=true;
			break;
		case 'l':
			alt_mode=true;
			break;
		case 'a':
			rsth=atoi(optarg);
			if(rsth<1 || rsth>15 ) {
				cout << "Warning: bad rsth time: 1 -> 40 ns, 15 -> 300 ns" << endl;
				exit(1);
			}
			set_reset = true;
			break;
		case 'b':
			rsts=atoi(optarg);
			if(rsts<1 || rsts>15 ) {
				cout << "Warning: bad rsts time: 1 -> 40 ns, 15 -> 500 ns" << endl;
				exit(1);
			}
			set_reset = true;
			break;
		case 'c':
			reset=atoi(optarg);
			if(reset<1 || reset>15 ) {
				cout << "Warning: bad reset time: 1 -> 100 ns, 15 -> 1500 ns" << endl;
				exit(1);
			}
			set_reset = true;
			break;
		case 't':
			repeat=atoi(optarg);
			if(repeat<1 || repeat>15 ) {
				cout << "Warning: bad period between reset: 1 -> 10 us, 15 -> 2 ms" << endl;
				exit(1);
			}
			set_time = true;
			break;
		case 'd':
			delay_coarse=atoi(optarg);
			if(delay_coarse<1 || delay_coarse>15 ) {
				cout << "Warning: bad gain hold delay coarse: 1 -> 1 us, 15 -> 100 us" << endl;
				exit(1);
			}
			set_time = true;
			break;
		case 'y':
			delay_fine=atoi(optarg);
			if(delay_fine<1 || delay_fine>15 ) {
				cout << "Warning: bad gain hold delay fine: 1 -> 100 ns, 7 -> 1500 ns" << endl;
				exit(1);
			}
			set_time = true;
			break;
		case 'o':
			file=optarg;
			if(access(file, F_OK ) != -1) {
				cout << "Warning: output file already existing. Please specify a different name" << endl;
				exit(1);
			}
			change_name = true;
			break;
		case 'h':
			cout << usage.str().c_str() << endl;
			exit(0);
			break;
		default:
			cout << usage.str().c_str() << endl;
			exit(0);
			break;
		}
	}

} 

CQuickUsb *fQUSB;
ofstream *ofs;

void exit_safe(int ret_val) {
	sleep(1);

	cout << endl;
	cout << "Getting ready to exit..." << endl;
	cout << endl;

#ifdef DRIVER
	if(fQUSB->IsOpened())
	{
		if (fQUSB->Close()==0)
		{
			cout<<"Close Qusb Device Error"<<endl;
			//exit(EXIT_FAILURE);
		}
		else
		{
			cout<<"Close Qusb Device OK"<<endl;
		}
	}
#else
	if(fQUSB->IsOpen())
	{
		if (fQUSB->Close()==0)
		{
			cout<<"Close Qusb Device Error"<<endl;
			//exit(EXIT_FAILURE);
		}
		else
		{
			cout<<"Close Qusb Device OK"<<endl;
		}
	}
#endif

	if(ofs->is_open())
	{
		ofs->close();
		if(ofs->fail())
		{
			cout<<"Close Ofstream File Error"<<endl;
			//exit(EXIT_FAILURE);
		}
		else
		{
			cout<<"Close OfStream File OK"<<endl;
		}
	}

	cout << endl;
	cout << "Now Exiting." << endl;
	exit(ret_val);
}

void sighandler(int code){
	cout << endl;
	cout << "Catched signal " << code << endl;
	exit_safe(EXIT_SUCCESS);
}

//MAIN

int main(int argc, char **argv)
{

	signal(SIGINT, sighandler);
	signal(SIGQUIT, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGUSR1, sighandler);
	signal(SIGUSR2, sighandler);

	HandleInputPar(argc, argv);

	if(access(acq_dir, F_OK ) == -1) {
		cout << "Error: The acquired files directory is not existing, correct the path in executable and recompile!" << endl;
		exit(EXIT_FAILURE);
	}

	//----------------------------------------------------
	//--------------  CONFIGURE DEVICES  -----------------
	//----------------------------------------------------

	//OPEN and SET QUSB DEVICE

	char dlist[BUFFER_LENGTH];

	fQUSB = new CQuickUsb("QUSB-0");

	if(fQUSB->FindModules(dlist,128)>0)
	{
		cout<<"Qusb Device Found"<<endl;
		cout<<dlist<<endl;
	}

	if(fQUSB->Open()==0)
	{
		cout<<"Open Qusb Device Error"<<endl;
		exit_safe(EXIT_FAILURE);
	}
	else cout<<"Open Device OK"<<endl;

	sleep(1);

	int result;

	unsigned short int read_wordwide =0;
	unsigned short int read_dataaddress =0;
	unsigned short int read_cpuconfig =0;
	unsigned short int read_spiconfig =0;

	cout << "Read Settings:"<<endl;

	result = fQUSB->ReadSetting(SETTING_WORDWIDE, &read_wordwide);

	if(result==0)
	{
		cout << "Error reading WORDWIDE: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	}

	cout << "SETTING_WORDWIDE = " <<hex << read_wordwide <<endl;

	result = fQUSB->ReadSetting(SETTING_DATAADDRESS, &read_dataaddress);

	if (result==0)
	{
		cout << "Error reading DATAADDRESS: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	}

	cout << "SETTING_DATAADDRESS = " <<hex << read_dataaddress <<endl;

	result = fQUSB->ReadSetting(SETTING_CPUCONFIG, &read_cpuconfig);

	if (result==0)
	{
		cout << "Error reading CPUCONFIG: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	}

	cout << "SETTING_CPUCONFIG = " <<hex << read_cpuconfig <<endl;

	result = fQUSB->ReadSetting(SETTING_SPICONFIG, &read_spiconfig);

	if (result==0)
	{ 
		cout << "Error reading SPICONFIG: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	}

	cout << "SETTING_SPICONFIG = " <<hex << read_spiconfig <<endl;

	cout << endl;

	result = set_qusb(fQUSB);

	if (result==0)
	{ 
		cout << "Error setting QuickUSB: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	} 

	cout << "Setting QuickUSB"<<endl;

	cout << endl;

	cout << "Check Settings:"<<endl;

	result = fQUSB->ReadSetting(SETTING_WORDWIDE, &read_wordwide);

	if(result==0)
	{
		cout << "Error reading WORDWIDE: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	}

	cout << "SETTING_WORDWIDE = " <<hex << read_wordwide <<endl;

	result = fQUSB->ReadSetting(SETTING_DATAADDRESS, &read_dataaddress);

	if (result==0)
	{ 
		cout << "Error reading DATAADDRESS: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	} 

	cout << "SETTING_DATAADDRESS = " <<hex << read_dataaddress <<endl;

	result = fQUSB->ReadSetting(SETTING_CPUCONFIG, &read_cpuconfig);

	if (result==0)
	{ 
		cout << "Error reading CPUCONFIG: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	} 

	cout << "SETTING_CPUCONFIG = " <<hex << read_cpuconfig <<endl;

	result = fQUSB->ReadSetting(SETTING_SPICONFIG, &read_spiconfig);

	if (result==0)
	{ 
		cout << "Error reading SPICONFIG: Exit..." << endl;
		exit_safe(EXIT_FAILURE);
	} 

	cout << "SETTING_SPICONFIG = " <<hex << read_spiconfig <<endl;

	cout << endl;

	//-----------------------------------------------
	//------------  START ACQUISITION  --------------
	//-----------------------------------------------

	unsigned short int ROC_ID;
	float ROC_Version;

	cout << "read_ID result = " << readID_qusb(fQUSB, &ROC_ID)<< endl;

	cout << hex << "ID_ROC = " << ROC_ID << endl;

	cout << endl;

	cout << "read_Version result = " << readVersion_qusb(fQUSB, &ROC_Version)<< endl;

	cout << dec << "Version_ROC = " << ROC_Version << endl;

	cout << endl;

	int i = 0;

	while(i!=n_file)
	{
		unsigned short int write_set_roc;
		unsigned short int read_set_roc;
		unsigned short int sr_ROC;

		write_set_roc = 0x0000;
		result = write_ROC_settings(fQUSB, &write_set_roc); //configura l'avvio dell'acquisizione

		if(result==0)
		{
			cout << "Error writing ROC settings: Exit..." << endl;
			exit_safe(EXIT_FAILURE);
		}

		result = read_ROC_settings(fQUSB, &read_set_roc);

		if(result==0)
		{
			cout << "Error reading ROC settings: Exit..." << endl;
			exit_safe(EXIT_FAILURE);
		}

		cout << "Executing reset..." << endl;
		cout << hex << "ROC settings = " << read_set_roc << endl << endl;

		sleep(1);

		if (set_reset){
			int Time_settings = 0x0000 + (reset<<8) + (rsts<<4) + rsth;

			write_set_roc = Time_settings;
			result = write_ROC_settings(fQUSB, &write_set_roc);
			if(result==0) {
				cout << "Error writing ROC Time_repeat: Exit..." << endl;
				exit_safe(EXIT_FAILURE);
			}

			result = read_ROC_settings(fQUSB, &read_set_roc);

			if(result==0)
			{
				cout << "Error reading ROC settings: Exit..." << endl;
				exit_safe(EXIT_FAILURE);
			}

			cout << "Set Time_repeat: " << Time_settings << endl;
			cout << hex << "ROC settings = " << read_set_roc << endl << endl;

			sleep(1);

		}

		if (set_time){
			int Time_settings = 0x1000 + (delay_fine<<8) + (delay_coarse<<4) + repeat;

			write_set_roc = Time_settings;
			result = write_ROC_settings(fQUSB, &write_set_roc);
			if(result==0) {
				cout << "Error writing ROC Time_repeat: Exit..." << endl;
				exit_safe(EXIT_FAILURE);
			}

			result = read_ROC_settings(fQUSB, &read_set_roc);

			if(result==0)
			{
				cout << "Error reading ROC settings: Exit..." << endl;
				exit_safe(EXIT_FAILURE);
			}

			cout << "Set Time_repeat: " << Time_settings << endl;
			cout << hex << "ROC settings = " << read_set_roc << endl << endl;

			sleep(1);

		}

		if(force_gain)
			if(alt_mode)
				write_set_roc = 0xB000; //avvia l'acquisizione forzando il guadagno in modalità alternativa
			else
				write_set_roc = 0xA000; //avvia l'acquisizione forzando il guadagno
		else
			if(alt_mode)
				write_set_roc = 0x9000; //avvia l'acquisizione in modalità alternativa
			else
				write_set_roc = 0x8000; //avvia l'acquisizione

		result = write_ROC_settings(fQUSB, &write_set_roc);

		if(result==0)
		{
			cout << "Error writing ROC settings: Exit..." << endl;
			exit_safe(EXIT_FAILURE);
		}

		result = read_ROC_settings(fQUSB, &read_set_roc);

		if(result==0)
		{
			cout << "Error reading ROC settings: Exit..." << endl;
			exit_safe(EXIT_FAILURE);
		}

		cout << "Configuring acquisition..." << endl;
		cout << hex << "ROC settings = " << read_set_roc << endl << endl;

		sleep(1);

		time_t t_start = time(0);   // get time now
		struct tm * now = localtime( & t_start );

		if(!change_name) {
			file = new char[256];
			strftime(file,256,"%Y%m%d_%H%M%S.dat", now);
			char name[256];
			sprintf(name,"%s/%s", acq_dir, file);
			file = &name[0];
		}

		cout << "File n° " << i+1 << " : " << file << endl << endl;

		ofs = new ofstream(file, ofstream::app);

		result = write_trg_enable(fQUSB);

		if(result==0)
		{
			cout << "Error enabling trigger: Exit..." << endl;
			exit_safe(EXIT_FAILURE);
		}

		cout << "Starting acquisition: trigger enabled" << endl;

		int j=0;

		sleep(1);

		//ACQUISIZIONE di n_ev EVENTI

		while(j!=n_ev)
		{
			result = read_status_register(fQUSB, &sr_ROC);

			if(result==0)
			{
				cout << "Error reading status register: Exit..." << endl;
				exit_safe(EXIT_FAILURE);
			}
			else
			{
				if(sr_ROC & 1)
				{
					cout << "\rEvent n° " << dec << j+1 << flush;

					result = read_data(fQUSB, ofs); //READ DATA

					if(result==0)
					{
						cout << "Error reading data: Exit..." << endl;
						exit_safe(EXIT_FAILURE);
					}

					if(j==n_ev-1)
						break;

					result = write_trg_enable(fQUSB); //ENABLE TRIGGER

					if(result==0)
					{
						cout << "Error enabling trigger: Exit..." << endl;
						exit_safe(EXIT_FAILURE);
					}

					++j;
				}
			}
		}

		ofs->close();
		if(ofs->fail())
		{
			cout<<"Close Ofstream File Error"<<endl;
			//exit(EXIT_FAILURE);
		}
		else
		{
			cout<<"Close OfStream File OK"<<endl;
		}

		time_t t_end = time(0);   // get time now
		double seconds = difftime(t_end, t_start);
		cout << endl;
		cout << "End of acquisition n°" << i+1 << " after " << seconds << " s from start" << endl;

		cout << endl;
		i++;
	}

	//-----------------------------------------------
	//--------------  CLOSE DEVICES  ----------------
	//-----------------------------------------------

	//CLOSE QUSB DEVICE

	if (fQUSB->Close()==0)
	{
		cout<<"Close Qusb Device Error"<<endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		cout<<"Close Qusb Device OK"<<endl;
	}
	
	exit(EXIT_SUCCESS);

}


