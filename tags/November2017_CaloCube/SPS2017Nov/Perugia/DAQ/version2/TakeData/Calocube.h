#ifndef CALOCUBE_H
#define CALOCUBE_H
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
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
#include "CQuickUsb.h"

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

//#define DRIVER

/* C++98 <-> C++11 portability */
#if __cplusplus > 199711
#define STATIC_CONST static constexpr
#else
#define STATIC_CONST static const
#endif

using namespace std;

struct ROC_Header {
	unsigned short int nIter;
	unsigned short int Sec_MSB;
	unsigned short int Sec_LSB;
	unsigned short int Mic_MSB;
	unsigned short int Mic_LSB;
};

class Calocube {

public:
	Calocube(const char* conf_file);
	~Calocube();

	int init();
	int analyze_parameters ();
	int init_qusb();
	int open_qusb();
	int conf_qusb();
	int read_info();
	int read_qusb_settings(
			unsigned short int &read_wordwide,
			unsigned short int &read_dataaddress,
			unsigned short int &read_cpuconfig,
			unsigned short int &read_spiconfig);
	int write_qusb_settings(
			unsigned short int &write_wordwide,
			unsigned short int &write_dataaddress,
			unsigned short int &write_cpuconfig,
			unsigned short int &write_spiconfig);
	int read_board_id (unsigned short int *ID_Qusb);
	int read_board_version (float *version_Qusb);
	int conf_roc();
	int read_roc_settings (unsigned short int &read_set_roc);
	int write_roc_settings (unsigned short int write_set_roc);
	int roc_dummy();
	int open_file (char * run_number_name);
	int open_file (int run_number);
	int take_data (bool save_data=true);
	int take_data_with_ams (timeval time_zero, char *header, int header_size);
	int check_data_ready (bool &ready);
	int read_status_register (unsigned short int *status_register);
	int write_header(char *ams_header, int ams_header_size);
	int read_data (bool save_data=true);
	int enable_trigger ();
	int close();

	void dixit(const int verbosity, const bool print_name, const char *text, ...);

	string compose_name (char * run_number_name);
	string compose_name (int run_number);

	bool is_enabled(){
		return enable_flag;
	}
	string get_file_name (){
		return file_name;
	}

	STATIC_CONST int max_niter = 3;
	STATIC_CONST double init_time_out = 3.;//s
	STATIC_CONST double loop_time_out = 1000.;//s
	STATIC_CONST double diff_time_out = 10.;//ms

private:
	STATIC_CONST int verbose_level=0;

	STATIC_CONST unsigned short int ROC_ID=4;
	STATIC_CONST float ROC_version=4.22;

	CQuickUsb *fQusb;
	ofstream *ofs;

	const char *configuration_file;
	string file_name;

	char data_path[256];

	bool enable_flag;

	bool alt_mode;
	bool force_gain;
	bool set_time;
	bool set_reset;

	int rsth, rsts, reset;
	int repeat, delay_coarse, delay_fine;
};
#endif
