#include <sys/time.h>
#include <sys/stat.h>
#include "Calocube.h"

//Constructor and Destructor

Calocube::Calocube(const char *conf_file){
	dixit(0, true, "%s : Creating Calocube class\n", __func__);
	configuration_file = conf_file;

	fQusb = NULL;
	ofs=NULL;

	//this->init();
}

Calocube::~Calocube(){
	dixit(0, true, "%s : Getting ready to exit\n", __func__);
	if(this->close()!=0)
		dixit(0, true, "%s : Detected some errors during exit\n", __func__);
	dixit(0, true, "%s : Calocube class deleted\n", __func__);
}

int Calocube::init() {
	dixit(0, true, "%s : Calocube initialization\n", __func__);

	if(analyze_parameters()!=0) {
		dixit(0, true, "%s : Failed parameters analysis: Force Exit\n", __func__);
		return -1;
	}

	if(!is_enabled()) return -2;

	if(init_qusb()!=0) {
		dixit(0, true, "%s : Failed QUSB intialization: Force Exit\n", __func__);
		return -3;
	}

	return 0;
}

int Calocube::analyze_parameters () {//GET ACQUISITION PARAMETERS
	if(access(configuration_file, F_OK ) == -1) {
		dixit(0, true, "%s : Configuration File %s not found : Force exit\n", __func__, configuration_file);
		return -1;
	}

	int enabled, mode, gain;
	char path_to_data[256];

	FILE *file=fopen(configuration_file,"r");

	char dummy[256];
	for(int iline=0; iline<8; ++iline)
		fgets (dummy , 256, file); //Skip comments
	fscanf(file, "%s %s   ", &dummy, &data_path);
	fscanf(file, "%s %d   ", &dummy, &enabled);
	fscanf(file, "%s %d   ", &dummy, &mode);
	fscanf(file, "%s %d   ", &dummy, &gain);
	fscanf(file, "%s %d %d %d   ", &dummy, &rsth, &rsts, &reset);
	fscanf(file, "%s %d %d %d   ", &dummy, &repeat, &delay_coarse, &delay_fine);

	fclose(file);

	if(enabled==0)
		enable_flag=false;
	else if(enabled==1)
		enable_flag=true;
	else {
		dixit(0, true, "%s : Enable option must be 0 or 1\n", __func__);
		return -2;
	}
	if(mode==0)
		alt_mode=false;
	else if(alt_mode==1)
		alt_mode=true;
	else {
		dixit(0, true, "%s : Acquisition mode must be 0 or 1\n", __func__);
		return -3;
	}
	if(gain==0)
		force_gain=false;
	else if(alt_mode==1)
		force_gain=true;
	else {
		dixit(0, true, "%s : Gain mode must be 0 or 1\n", __func__);
		return -4;
	}

	if(rsth<0 || rsth>15) {
		dixit(0, true, "%s : RSTH parameter must be 0 <= RSTH <= 1\n", __func__);
		return -5;
	}
	if(rsts<0 || rsts>15) {
		dixit(0, true, "%s : RSTS parameter must be 0 <= RSTH <= 1\n", __func__);
		return -6;
	}
	if(reset<0 || reset>15) {
		dixit(0, true, "%s : RESET parameter must be 0 <= RSTH <= 1\n", __func__);
		return -7;
	}
	if(repeat<0 || repeat>15) {
		dixit(0, true, "%s : REPEAT parameter must be 0 <= RSTH <= 1\n", __func__);
		return -8;
	}
	if(delay_coarse<0 || delay_coarse>15) {
		dixit(0, true, "%s : DELAY_COARSE parameter must be 0 <= RSTH <= 1\n", __func__);
		return -9;
	}
	if(delay_fine<0 || delay_fine>15) {
		dixit(0, true, "%s : DELAY_FINE parameter must be 0 <= RSTH <= 1\n", __func__);
		return -10;
	}

	if(rsth>0 || rsts>0 || reset>0)
		set_reset=true;
	else
		set_reset=false;
	if(repeat>0 || delay_coarse>0 || delay_fine>0)
		set_time=true;
	else
		set_time=false;

	if(is_enabled()) {
		dixit(0, true, "%s : Data taking will start in the following configuration : \n", __func__);
		dixit(0, false, "\t\t --- %s MODE\n", alt_mode? "ALTERNATIVE" : "NOMINAL");
		dixit(0, false, "\t\t --- %s GAIN\n", force_gain? "FORCED" : "FREE");
		dixit(0, false, "\t with the following acquisition parameters : \n");
		dixit(0, false, "\t\t --- RSTH %d\n", rsth);
		dixit(0, false, "\t\t --- RSTS %d\n", rsts);
		dixit(0, false, "\t\t --- RESET %d\n", reset);
		dixit(0, false, "\t\t --- REPEAT %d\n", repeat);
		dixit(0, false, "\t\t --- DELAY_COARSE %d\n", delay_coarse);
		dixit(0, false, "\t\t --- DELAY_FINE %d\n", delay_fine);
	} else {
		dixit(0, true, "%s : Calocube is not included in the data taking\n", __func__);
	}

	return 0;
}

int Calocube::init_qusb() {//INITALIZE QUSB DEVICE
	if(open_qusb()!=0) return -1;
	if(conf_qusb()!=0) return -1;
	if(read_info()!=0) return -1;

	return 0;
}

int Calocube::open_qusb() {//OPEN QUSB DEVICE
	char dlist[BUFFER_LENGTH];

	fQusb = new CQuickUsb("QUSB-0");

	if(fQusb->FindModules(dlist,128)>0)
		dixit(0, true, "%s : Found Qusb Device %s \n", __func__, dlist);
	else {
		dixit(0, true, "%s : No Qusb Device Found \n", __func__);
		return -1;
	}

	if(fQusb->Open()==0)
	{
		dixit(0, true, "%s : Open Qusb Device Error\n", __func__);
		return -2;
	}
	else
		dixit(0, true, "%s : Open Qusb Device OK\n", __func__);

	return 0;
}

int Calocube::conf_qusb() {//CONFIGURE QUSB DEVICE
	unsigned short int read_wordwide = 0;
	unsigned short int read_dataaddress = 0;
	unsigned short int read_cpuconfig = 0;
	unsigned short int read_spiconfig = 0;
	dixit(0, true, "%s : Read QUSB settings\n", __func__);
	sleep(1);
	if(read_qusb_settings(read_wordwide, read_dataaddress, read_cpuconfig, read_spiconfig)!=0)
		return -1;

	unsigned short int write_wordwide = read_wordwide;
	unsigned short int write_dataaddress = read_dataaddress;
	unsigned short int write_cpuconfig = read_cpuconfig;
	unsigned short int write_spiconfig = read_spiconfig;
	dixit(0, true, "%s : Write QUSB settings\n", __func__);
	sleep(1);
	if(write_qusb_settings(write_wordwide, write_dataaddress, write_cpuconfig, write_spiconfig)!=0)
		return -2;

	unsigned short int check_wordwide = 0;
	unsigned short int check_dataaddress = 0;
	unsigned short int check_cpuconfig = 0;
	unsigned short int check_spiconfig = 0;
	dixit(0, true, "%s : Check QUSB settings\n", __func__);
	sleep(1);
	if(read_qusb_settings(check_wordwide, check_dataaddress, check_cpuconfig, check_spiconfig)!=0)
		return -3;

	/*if(check_wordwide != write_wordwide) 
		cout << "1 " << check_wordwide-write_wordwide << " " << check_wordwide << " " << write_wordwide << endl;
	if(check_dataaddress != write_dataaddress) 
		cout << "2 " << check_dataaddress-write_dataaddress << " " << check_dataaddress << " " << write_dataaddress << endl;
	if(check_cpuconfig != write_cpuconfig) 
		cout << "3 " << check_cpuconfig-write_cpuconfig << " " << check_cpuconfig << " " << write_cpuconfig << endl;
	if(check_spiconfig != write_spiconfig) 
		cout << "4 " << check_spiconfig-write_spiconfig << " " << check_spiconfig << " " << write_spiconfig << endl;*/

	if((check_wordwide != write_wordwide) ||
			(check_dataaddress != write_dataaddress) ||
			(check_cpuconfig != write_cpuconfig) ||
			(check_spiconfig != write_spiconfig)) {
		dixit(0, true, "%s : Read value is inconsistent with written value : Force Exit...\n", __func__);
		return -4;
	}

	return 0;
}

int Calocube::read_info(){//GET BOARD INFO

	unsigned short int id;
	float version;
	if(read_board_id (&id)!=0) return -1;
	if(read_board_version (&version)!=0) return -1;

	return 0;
}


int Calocube::read_qusb_settings(
		unsigned short int &read_wordwide,
		unsigned short int &read_dataaddress,
		unsigned short int &read_cpuconfig,
		unsigned short int &read_spiconfig) {//READ QUSB SETTINGS

	int result;

	result = fQusb->ReadSetting(SETTING_WORDWIDE, &read_wordwide);
	if(result==0) {
		dixit(0, true, "%s : Error reading WORDWIDE\n", __func__);
		return -1;
	} else
		dixit(0, true, "%s : Read WORDWIDE = %x\n", __func__, read_wordwide);

	result = fQusb->ReadSetting(SETTING_DATAADDRESS, &read_dataaddress);
	if(result==0) {
		dixit(0, true, "%s : Error reading DATAADDRESS\n", __func__);
		return -2;
	} else
		dixit(0, true, "%s : Read DATAADDRESS = %x\n", __func__, read_dataaddress);

	result = fQusb->ReadSetting(SETTING_CPUCONFIG, &read_cpuconfig);
	if(result==0) {
		dixit(0, true, "%s : Error reading CPUCONFIG\n", __func__);
		return -3;
	} else
		dixit(0, true, "%s : Read CPUCONFIG = %x\n", __func__, read_cpuconfig);

	result = fQusb->ReadSetting(SETTING_SPICONFIG, &read_spiconfig);
	if(result==0) {
		dixit(0, true, "%s : Error reading SPICONFIG\n", __func__);
		return -4;
	} else
		dixit(0, true, "%s : Read SPICONFIG = %x\n", __func__, read_spiconfig);

	return 0;
}

int Calocube::write_qusb_settings(
		unsigned short int &write_wordwide,
		unsigned short int &write_dataaddress,
		unsigned short int &write_cpuconfig,
		unsigned short int &write_spiconfig) {//WRITE QUSB SETTINGS
	int result;
	unsigned long temp;

	temp=write_wordwide;
	bitset<16> manip_1 (temp);
	manip_1.set(0,1);
	temp=manip_1.to_ulong();
	write_wordwide=temp;
	result = fQusb->WriteSetting(SETTING_WORDWIDE, write_wordwide);
	if(result==0) {
		dixit(0, true, "%s : Error writing WORDWIDE\n", __func__);
		return -1;
	} else
		dixit(0, true, "%s : Write WORDWIDE = %x\n", __func__, write_wordwide);

	temp=write_dataaddress;
	bitset<16> manip_2 (temp);
	manip_2.set(15,1);
	manip_2.set(14,0);
	temp=manip_2.to_ulong();
	write_dataaddress=temp;
	result = fQusb->WriteSetting(SETTING_DATAADDRESS, write_dataaddress);
	if(result==0) {
		dixit(0, true, "%s : Error writing DATAADDRESS\n", __func__);
		return -2;
	} else
		dixit(0, true, "%s : Write DATAADDRESS = %x\n", __func__, write_dataaddress);

	temp=write_cpuconfig;
	bitset<16> manip_3 (temp);
	manip_3.set(15,1);
	manip_3.set(4,1);
	manip_3.set(3,0);
	manip_3.set(2,0);
	manip_3.set(1,0);
	temp=manip_3.to_ulong();
	write_cpuconfig=temp;
	result = fQusb->WriteSetting(SETTING_CPUCONFIG, write_cpuconfig);
	if(result==0) {
		dixit(0, true, "%s : Error writing CPUCONFIG\n", __func__);
		return -3;
	} else
		dixit(0, true, "%s : Write CPUCONFIG = %x\n", __func__, write_cpuconfig);

	temp=write_spiconfig;
	bitset<16> manip_4 (temp);
	manip_4.set(15,1);
	temp=manip_4.to_ulong();
	write_spiconfig=temp;
	result = fQusb->WriteSetting(SETTING_SPICONFIG, write_spiconfig);
	if(result==0) {
		dixit(0, true, "%s : Error writing SPICONFIG\n", __func__);
		return -4;
	} else
		dixit(0, true, "%s : Write SPICONFIG = %x\n", __func__, write_spiconfig);

	return 0;
}

int Calocube::read_board_id (unsigned short int *ID_Qusb) { //READ BOARD ID
	int result;

	unsigned short int address=0x00;
	unsigned short int length=2;
	unsigned char value[2];

	result=fQusb->ReadCommand(address,value,&length);

	unsigned char *c;

	c = (unsigned char *)ID_Qusb;

	*c=value[0];
	*(c+1)=value[1];

	if(result==0) {
		dixit(0, true, "%s : Error reading ROC ID\n", __func__);
		return -1;
	} else
		dixit(0, true, "%s : ROC ID = %u\n", __func__, *ID_Qusb);

	if(*ID_Qusb != ROC_ID) {
		dixit(0, true, "%s : ROC ID is different from %x (check power of ROC connector) : Force Exit...\n", __func__, ROC_ID);
		return -2;
	}

	return 0;
}

int Calocube::read_board_version (float *version_Qusb) { //READ FIRMWARE VERSION
	int result;

	unsigned short int address=0x84;
	unsigned short int length=2;
	unsigned char value[2];

	result=fQusb->ReadCommand(address,value,&length);

	unsigned char *c;
	unsigned short int i;

	i = 0;
	c = (unsigned char *)&i;

	*c=value[0];
	*(c+1)=value[1];

	*version_Qusb = ((float)((i & 0xF000) >> 12) * 10) + ((float)((i & 0x0F00) >> 8) * 1) + ((float)((i & 0x00F0) >> 4) * 0.1 ) + ((float)((i & 0x000F) >> 0) * 0.01);

	if(result==0) {
		dixit(0, true, "%s : Error reading ROC version\n", __func__);
		return -1;
	} else
		dixit(0, true, "%s : ROC Version = %.2f\n", __func__, *version_Qusb);

	if(*version_Qusb != ROC_version) {
		dixit(0, true, "%s : ROC version is different from %.2f (check power of ROC connector) : Force Exit...\n", __func__, ROC_version);
		return -2;
	}

	return 0;
}

int Calocube::conf_roc() {//CONFIGURE ROC
	unsigned short int in_cmd=0;
	unsigned short int out_cmd=0;

	sleep(1);

	in_cmd = 0x0000;
	dixit(0, true, "%s : Executing reset\n", __func__);
	if(write_roc_settings(in_cmd)!=0) return -1;
	if(read_roc_settings(out_cmd)!=0) return -1;
	if(in_cmd != out_cmd) {
		dixit(0, true, "%s : Reset Failed\n", __func__);
		return -1;
	} else
		dixit(0, true, "%s : Configuration register is = %x - %x\n", __func__, in_cmd, out_cmd);


	if (set_reset){
		sleep(1);

		in_cmd =0x0000 + (reset<<8) + (rsts<<4) + rsth;
		dixit(0, true, "%s : Setting Reset Time\n", __func__);
		if(write_roc_settings(in_cmd)!=0) return -1;
		if(read_roc_settings(out_cmd)!=0) return -1;
		if(in_cmd != out_cmd) {
			dixit(0, true, "%s : Set Reset Time Failed\n", __func__);
			return -2;
		} else
			dixit(0, true, "%s : Configuration register is = %x - %x\n", __func__, in_cmd, out_cmd);
	}

	if (set_time){
		sleep(1);

		in_cmd = 0x1000 + (delay_fine<<8) + (delay_coarse<<4) + repeat;
		dixit(0, true, "%s : Setting Integration Time\n", __func__);
		if(write_roc_settings(in_cmd)!=0) return -1;
		if(read_roc_settings(out_cmd)!=0) return -1;
		if(in_cmd != out_cmd) {
			dixit(0, true, "%s : Set Integration Time Failed\n", __func__);
			return -3;
		} else
			dixit(0, true, "%s : Configuration register is = %x - %x\n", __func__, in_cmd, out_cmd);
	}

	sleep(1);

	if(force_gain)
		if(alt_mode)
			in_cmd = 0xB000; //avvia l'acquisizione forzando il guadagno in modalità alternativa
		else
			in_cmd = 0xA000; //avvia l'acquisizione forzando il guadagno
	else
		if(alt_mode)
			in_cmd = 0x9000; //avvia l'acquisizione in modalità alternativa
		else
			in_cmd = 0x8000; //avvia l'acquisizione
	dixit(0, true, "%s : Start acquisition in %s mode with %s gain\n", __func__,
			alt_mode? "Alternative" : "Normal",
					force_gain? "Forced" : "Free");
	if(write_roc_settings(in_cmd)!=0) return -1;
	if(read_roc_settings(out_cmd)!=0) return -1;
	if(in_cmd != out_cmd) {
		dixit(0, true, "%s : Start acquisition Failed\n", __func__);
		return -4;
	} else
		dixit(0, true, "%s : Configuration register is = %x - %x\n", __func__, in_cmd, out_cmd);

	return 0;
}

int Calocube::read_roc_settings (unsigned short int &read_set_roc) {//READ ROC SETTINGS
	unsigned short int address=0x85;
	unsigned short int length=2;
	unsigned char value[2];

	int result=fQusb->ReadCommand(address,value,&length);

	unsigned char *c;

	unsigned short int *settings = &read_set_roc;
	c = (unsigned char *)settings;

	*c=value[0];
	*(c+1)=value[1];

	if(result==0) {
		dixit(0, true, "%s : Error reading ROC settings\n", __func__);
		return -1;
	}

	return 0;
}

int Calocube::write_roc_settings (unsigned short int write_set_roc) {//WRITE QUSB SETTINGS
	unsigned short int address=0x83;
	unsigned short int length=2;
	unsigned char value[2];

	unsigned char * c;

	unsigned short int *settings = &write_set_roc;
	c = (unsigned char *)settings;

	value[0] = *c;
	value[1] = *(c+1);

	int result=fQusb->WriteCommand(address,value,length);

	if(result==0) {
		dixit(0, true, "%s : Error writing ROC settings\n", __func__);
		return -1;
	}

	return roc_dummy();
}

int Calocube::roc_dummy()
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

	if(result==0) {
		dixit(0, true, "%s : Error dummying ROC settings\n", __func__);
		return -1;
	}

	return 0;
}

int Calocube::open_file (char *run_number_name){
	file_name = this->compose_name(run_number_name).c_str();

	if(access(file_name.c_str(), F_OK ) != -1) {
		dixit(0, true, "%s : File %s already exist : Force exit\n", __func__);
		return -1;
	}

	ofs = new ofstream(file_name.c_str(), ofstream::app);

	if(!ofs->is_open()) {
		dixit(0, true, "%s : File %s is not opened : Force exit\n", __func__);
		return -2;
	}

	return 0;
}

int Calocube::open_file (int run_number){
	file_name = this->compose_name(run_number).c_str();

	if(access(file_name.c_str(), F_OK ) != -1) {
		dixit(0, true, "%s : File %s already exist : Force exit\n", __func__);
		return -1;
	}

	ofs = new ofstream(file_name.c_str(), ofstream::app);

	if(!ofs->is_open()) {
		dixit(0, true, "%s : File %s is not opened : Force exit\n", __func__);
		return -2;
	}

	return 0;
}

string Calocube::compose_name (char *run_number_name){
	string name;
	char buffer[512];
	sprintf(buffer, "%s/CC%s.dat", data_path, run_number_name);
	name = buffer;
	return name;
}

string Calocube::compose_name (int run_number){
	string name;
	char buffer[512];
	sprintf(buffer, "%s/CC%d.dat", data_path, run_number);
	name = buffer;
	return name;
}

int Calocube::take_data (bool save_data) {//READ DATA
	bool ready=false;
	while(!ready) {
		if(check_data_ready(ready)!=0)
			return -1;

		if(ready) {
			if(read_data(save_data)!=0)
				return -2;
			if(enable_trigger()!=0)
				return -3;
		}
	}

	return 0;
}

int Calocube::take_data_with_ams (timeval time_zero, char *ams_header, int ams_header_size) {//READ DATA WITH AMS DATA
	timeval time_now;
	bool ready=false;
	int iterations=0;

	char log_iter[128][256];

	while(!ready) {
		if(check_data_ready(ready)!=0)
			return -1;

		gettimeofday(&time_now, NULL);
		int sdiff = (time_now.tv_sec - time_zero.tv_sec);
		int udiff = (time_now.tv_usec - time_zero.tv_usec);
		double elapsed = (((double)(sdiff))*1000.) + (((double)(udiff))/1000.); //ms
		dixit(-2, false, "%d iteration : %.2f ms elapsed\n", iterations, elapsed);

		sprintf(log_iter[iterations], "%d iteration : %.2f ms elapsed", iterations, elapsed);
		++iterations;

		if(ready) {
			ROC_Header roc_header;
			roc_header.nIter=(ushort)iterations;
			roc_header.Sec_MSB=(ushort)(time_now.tv_sec>>16&0xffff);
			roc_header.Sec_LSB=(ushort)(time_now.tv_sec&0xffff);
			roc_header.Mic_MSB=(ushort)(time_now.tv_usec>>16&0xffff);
			roc_header.Mic_LSB=(ushort)(time_now.tv_usec&0xffff);

			if(write_header((char *)&ams_header_size, sizeof(int))!=0) return -2;
			if(write_header(ams_header, ams_header_size)!=0) return -3;
			if(write_header((char *)&roc_header, sizeof(struct ROC_Header))!=0) return -4;
			if(read_data()!=0) return -5;
			//if(enable_trigger()!=0) return -6; //This has been moved outside this funvtion

			dixit(-1, true, "%s : Event acquired after %.2f ms from AMS (%d iterations)\n", __func__, elapsed, iterations);
			if(elapsed>diff_time_out || iterations>max_niter) { 
				for(int iter=0; iter<iterations; ++iter)
					dixit(0, true, "%s : %s\n", __func__, log_iter[iter]);
				printf("\n");
			}
		} else {
			if(elapsed>diff_time_out && iterations>max_niter) {
				dixit(0, true, "%s : There is no trigger after %.2f ms from AMS (%d iterations) : Force Exit\n",
						__func__, elapsed, iterations);
				return -6;
			}
		}

	}

	return 0;
}

int Calocube::check_data_ready (bool &ready){//CHECK IF DATA IS READY TO BE READ
	unsigned short int content;
	if(read_status_register(&content)!=0) return -1;

	if(content & 1)
		ready = true;
	else
		ready = false;

	dixit(-3, true, "%s : Status register : %x\n", __func__, content);

	return 0;
}

int Calocube::read_status_register (unsigned short int *status_register) {//READ STATUS REGISTER
	int result;

	unsigned short int address=0x81;
	unsigned short int length=2;
	unsigned char value[2];

	result=fQusb->ReadCommand(address,value,&length);

	unsigned char *c;
	c = (unsigned char *)status_register;

	*c=value[0];
	*(c+1)=value[1];

	if(result==0) {
		dixit(0, true, "%s : Error reading status register\n", __func__);
		return -1;
	}

	return 0;
}

int Calocube::write_header(char *header, int header_size) {//WRITE HEADER
	if (!ofs->is_open())	{
		dixit(0, true, "%s : Output file is not open\n", __func__);
		return -1;
	}

	ofs->write(header, header_size);

	return 0;
}


int Calocube::read_data (bool save_data) {//READ DATA
	int result;

	unsigned long int length=1735;
	unsigned long int n_word = 2*length;
	unsigned char buf[n_word];
	unsigned short int data[length];

	unsigned int i, j;

	unsigned char * c;
	c = (unsigned char *)data;

	result = fQusb->ReadData(buf, &n_word);

	if(result==0) {
		dixit(0, true, "%s : Error reading data\n", __func__);
		return -1;
	}
	if (!ofs->is_open())	{
		dixit(0, true, "%s : Output file is not open\n", __func__);
		return -2;
	}

	if(save_data)
		for(i=0; i<length; i++) {
			for(j=2*i; j<2*(i+1); j++) {
				*(c+j)=buf[j];
			}
			ofs->write((char*)&data[i], 2);
		}

	return 0;
}

int Calocube::enable_trigger () {//ENABLE TRIGGER
	int result;

	unsigned short int address=0x82;
	unsigned short int length=2;
	unsigned char value[2];

	value[0]='a';
	value[1]='z';

	result=fQusb->WriteCommand(address,value,length);

	if(result==0) {
		dixit(0, true, "%s : Error enabling trigger\n", __func__);
		return -1;
	}

	return 0;
}

int Calocube::close() {
	int result=0;

	if(fQusb) {
		if(fQusb->IsOpen()) {
			unsigned short int in_cmd=0x0000;
			unsigned short int out_cmd=0;
			dixit(0, true, "%s : Executing reset\n", __func__);
			sleep(1);
			if(write_roc_settings(in_cmd)!=0) result=-1;
			if(read_roc_settings(out_cmd)!=0) result=-1;
			if(in_cmd != out_cmd) {
				dixit(0, true, "%s : Reset Failed\n", __func__);
				result=-2;
			}
			else
				dixit(0, true, "%s : Configuration register is = %x - %x\n", __func__, in_cmd, out_cmd);

			if (fQusb->Close()==0) {
				dixit(0, true, "%s : Close Qusb Device Error\n", __func__);
				result=-3;
			}
			else
				dixit(0, true, "%s : Close Qusb Device OK\n", __func__);
		}
		delete fQusb;
		fQusb=NULL;
	}

	if(ofs) {
		if(ofs->is_open())
		{
			ofs->close();
			if(ofs->fail()) {
				dixit(0, true, "%s : Close Output File Error\n", __func__);
				result=-4;
			}
			else
				dixit(0, true, "%s : Close Output File OK\n", __func__);
		}
		delete ofs;
		ofs=NULL;
	}

	return result;
}

void Calocube::dixit(const int verbosity, const bool print_name, const char *text, ...) {
	if(verbosity>=verbose_level) {
		string full_text = print_name?
				((string)"Calocube") + ":" + ((string)text) : ((string)text);
		va_list args;
		va_start(args, text);
		vprintf(full_text.c_str(), args);
		va_end(args);
	}
}
