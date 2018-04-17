#include "IOfile.h"

struct A {
	static const int CASIS_ID[];
};

#if defined(SPS2016) || defined(SPS2017Aug) || defined(SPS2017Nov)
const int A::CASIS_ID[] = {0x0401, 0x0402, 0x0403,
		0x0404, 0x0405, 0x0406,
		0x0407, 0x0408, 0x0409,
		0x040a, 0x040b, 0x040c,
		0x040d, 0x040e, 0x040f,
		0x0410,
		0x0411, 0x0412, 0x0413,
		0x0414, 0x0415, 0x0416,
		0x0417, 0x0418, 0x0419,
		0x041a, 0x041b, 0x041c,
		0x041d, 0x041e, 0x041f,
		0x0420,
		0x0421, 0x0422, 0x0423,
		0x0424, 0x0425, 0x0426,
		0x0427, 0x0428, 0x0429,
		0x042a, 0x042b, 0x042c,
		0x042d, 0x042e, 0x042f,
		0x0430,
		0x0431, 0x0432, 0x0433,
		0x0434, 0x0435, 0x0436,
};
#else
const int A::CASIS_ID[] = {0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049};
#endif

const unsigned short int ZERO = 0;

IOfile::IOfile()
{
	cout << endl << "Created Object IOfile" << endl;
	Reset(casis);
}

IOfile::~IOfile()
{
	cout << endl << "Deleted Object IOfile" << endl;
	Reset(casis);
}

void IOfile::Reset (struct dataCASIS dat)
{
	dat.trg_type = 0;
	dat.ev = 0;
	for (int i = 0; i<NCASISCHIPS; i++)
	{
		for (int j = 0; j<NCASISCHANNELS; j++)
			dat.data[i][j] = 0;
		for (int k = 0; k<NGAINWORDS; k++)
			dat.gain[i][k] = 0;
	}
}

void IOfile::OpenFile (const char * file, const char * option)
{
	in.open(file, ios_base::binary);

	string file_addr (file);
	file_addr.append("addr");

	//in_addr.open(file_addr.c_str(), ios_base::binary);

	string opt(option);
	if (opt=="v") {verbose = 1;}
	else {verbose = 0;}

}

int IOfile::ReadRunHeader()
{
	if(in.is_open())
	{return 0;}
	else
	{return -1;}
}

int IOfile::ReadEventHeader()
{
#ifdef AMS
	char *bufPtr;

	int header_size;
	bufPtr = (char*) &header_size;
	in.read(bufPtr, sizeof(int));
	if(header_size != sizeof(struct AMS_Header))
		cout << "Warning : size of header is " <<  header_size <<
		" instead of expected size " << sizeof(struct glb_Header) << endl;
	if (verbose == 1)
	{
		cout << dec << endl;
		cout << "Header NUMBERofBYTES = " << header_size << endl;
	}

	//int header_size = sizeof(struct glb_Header);
	bufPtr = (char*) &header;
	in.read(bufPtr, sizeof(struct glb_Header));

	if (verbose == 1)
	{
		cout << hex << endl;
		cout << "Header SIZE = " << header.ams_header.SIZE << endl;
		cout << "Header RRRWNODETYPE = " << header.ams_header.RRRWNODETYPE << endl;
		cout << "Header FBITAG = " << header.ams_header.FBITAG << endl;
		cout << "Header TIMEMSB = " << header.ams_header.TIMEMSB << endl;
		cout << "Header TIMELSB = " << header.ams_header.TIMELSB << endl;
		cout << "Header JMDCSIZE = " << header.ams_header.JMDCSIZE << endl;
		cout << "Header JMDCRRRWNODETYPE = " << header.ams_header.JMDCRRRWNODETYPE << endl;
		cout << "Header RUNNUMMSB = " << header.ams_header.RUNNUMMSB << endl;
		cout << "Header RUNNUMLSB = " << header.ams_header.RUNNUMLSB << endl;
		cout << "Header RUNTAGMSB = " << header.ams_header.RUNTAGMSB << endl;
		cout << "Header RUNTAGLSB = " << header.ams_header.RUNTAGLSB << endl;
		cout << "Header EVTNUMMSB = " << header.ams_header.EVTNUMMSB << endl;
		cout << "Header EVTNUMLSB = " << header.ams_header.EVTNUMLSB << endl;
		cout << "Header JMDCTIMEMSB = " << header.ams_header.JMDCTIMEMSB << endl;
		cout << "Header JMDCTIMELSB = " << header.ams_header.JMDCTIMELSB << endl;
		cout << "Header JMDCTIMEFINEMSB = " << header.ams_header.JMDCTIMEFINEMSB << endl;
		cout << "Header JMDCTIMEFINELSB = " << header.ams_header.JMDCTIMEFINELSB << endl;
		int time_ams_sec = (int) (header.ams_header.JMDCTIMEMSB*pow(2, 16)+header.ams_header.JMDCTIMELSB);
		int time_ams_mic = (int) (header.ams_header.JMDCTIMEFINEMSB*pow(2, 16)+header.ams_header.JMDCTIMEFINELSB);
		time_t time_ams = time_ams_sec;
		cout << "Equivalent to \n\t" << dec << asctime(localtime(&time_ams)) <<
				"\t\t and " << time_ams_mic << " usec " << hex << endl;
		cout << "Header GReservedGroups = " << header.ams_header.GReservedGroups << endl;
		cout << "Header DSPSIZE = " << header.ams_header.DSPSIZE << endl;
		cout << "Header DSPRRRWNODETYPE = " << header.ams_header.DSPRRRWNODETYPE << endl;
		cout << "Header nIter = " << header.roc_header.nIter << dec <<
				" (iteration " << header.roc_header.nIter << ")" << hex << endl;
		cout << "Header Sec_MSB = " << header.roc_header.Sec_MSB << endl;
		cout << "Header Sec_LSB = " << header.roc_header.Sec_LSB << endl;
		cout << "Header Mic_MSB = " << header.roc_header.Mic_MSB << endl;
		cout << "Header Mic_LSB = " << header.roc_header.Mic_LSB << endl;
		int time_roc_sec = (int) (header.roc_header.Sec_MSB*pow(2, 16)+header.roc_header.Sec_LSB);
		int time_roc_mic = (int) (header.roc_header.Mic_MSB*pow(2, 16)+header.roc_header.Mic_LSB);
		time_t time_roc = time_roc_sec;
		cout << "Equivalent to \n\t" << dec << asctime(localtime(&time_roc)) <<
				"\t\t and " << time_roc_mic << " usec " << hex << endl;
		int diff_sec = time_roc_sec - time_ams_sec;
		int diff_mic = time_roc_mic - time_ams_mic;
		cout << "Equivalent to a time difference of\n\t" << dec <<
				diff_sec << " sec\t" << diff_mic << " usec\n\t" <<
				"i.e. " << (((double)diff_sec)*1000.)+(((double)diff_mic)/1000.) << " msec" << hex << endl;
		cout << dec << endl;
	}
#elif ADAMO
	char *bufPtr;
	bufPtr = (char*) &header;
	in.read(bufPtr, sizeof(struct VME_Header));

	if (verbose == 1)
	{
		cout << hex << endl;
		cout << "Header Flag = " << header.Flag << endl;
		cout << "Header EvNum = " << header.EvNum << endl;
		cout << "Header Sec = " << header.Sec << endl;
		cout << "Header nSec = " << header.nSec << endl;
		cout << dec << endl;
	}
#endif

	if(in.eof())
	{
		cout << endl << "End of file reached" << endl;
		return -1;
	}

	return 0;
}

int IOfile::ReadEvent()
{
	unsigned short int buf;
	char *bufPtr = (char*) &buf;
	unsigned short int ROC_header = 0;
	int ev_number = 0;
	char *buf_ev = (char*) &ev_number;

	unsigned short int CASIS_header[NCASISCHIPS];
	for(int i = 0; i<NCASISCHIPS; i++)
	{CASIS_header[i]=0;}

	unsigned short int CASIS_id[NCASISCHIPS];
	for(int i = 0; i<NCASISCHIPS; i++)
	{CASIS_id[i]=0;}

	int CRC_32 = 0;
	char *buf_CRC = (char*) &CRC_32;
	/*
  float temp = 0;
  char *bufTemp = (char*) &temp;
	 */
	dataCASIS aux;

	Reset(aux);

	in.read(bufPtr, 2); // 5A5A header
	ROC_header = buf;
	in.read(buf_ev, 4); // Event number
	aux.ev = ev_number;
	in.read(bufPtr, 2); // Trigger type
	aux.trg_type = buf;
	in.read(bufPtr, 2); // Trigger time
	aux.trg_time = 10 * (buf - 2);

	if (verbose == 1)
	{
		cout << endl;
		cout << hex << "ROC header = " <<ROC_header << endl;
		cout << dec << "ROC event number = " << ev_number << endl;
		cout << hex << "TRG_TYPE = " << aux.trg_type << endl;
		cout << hex << "TRG_TIME = " << aux.trg_time << endl;
	}

	// Read casis
	for (int iChip = 0; iChip < NCASISCHIPS; iChip++)
	{
		// Discard casis number and gain
		in.read(bufPtr, 2); // CASIS tag F0F0
		CASIS_header[iChip] = buf;
		in.read(bufPtr, 2); // ROC and CASIS id
		CASIS_id[iChip] = buf;
		for (int iWord = 0; iWord<NGAINWORDS; iWord++)
		{
			in.read(bufPtr, 2); // Gain
			aux.gain[iChip][NGAINWORDS-iWord-1] = buf;
		}

		if (verbose == 1)
		{
			cout << hex << "CASIS header [" << iChip << "] = " << CASIS_header[iChip] << endl;
			cout << hex << "CASIS ID [" << iChip << "] = " << CASIS_id[iChip] << endl;
			for (int iWord = 0; iWord<NGAINWORDS; iWord++)
			{
				cout << hex << "CASIS gain [" << iChip << "][" << iWord << "] = " << aux.gain[iChip][iWord] << endl;
			}
		}

		for (int iChannel = 0; iChannel < NCASISCHANNELS; iChannel++)
		{
			in.read(bufPtr, 2);
			aux.data[iChip][iChannel] = buf;
		}
	}

	in.read(buf_CRC, 4); // CRC32
	/*
  for (int i=0; i < NSENSORS; i ++)
    {
      in.read(bufTemp, 4); // DS18B20
      aux.tempC[i] = temp;
    }
	 */
	if (verbose == 1)
	{
		cout << hex << "CRC32 = " << CRC_32 << endl;
	}

	if(in.eof())
	{
		cout << endl << "End of file reached" << endl;
		return -1;
	}

	if(ROC_header != ROC_HEADER)
	{
		cout << "Wrong ROC HEADER!" << endl;
		return -2;
	}

	for (int iChip = 0; iChip < NCASISCHIPS; iChip++)
	{
		if (CASIS_header[iChip] != CASIS_HEADER)
		{
			cout << "Wrong CASIS HEADER [" << iChip << "]!" << endl;
			return -2;
		}
		if (CASIS_id[iChip] != A::CASIS_ID[iChip])
		{
			cout << "Wrong CASIS ID [" << iChip << "]!" << endl;
			return -2;
		}
#if defined(SPS2016) || defined(SPS2017Aug) || defined(SPS2017Nov)
		if (aux.gain[iChip][1] & 0XF000)
		{
			cout << "Wrong CASIS Gain [" << iChip << "]!" << endl;
			return -2;
		}
#endif
	}

	casis = aux;
	return 0;
}

/*
int IOfile::SensorMap()
{

  ifstream in_map;
  in_map.open("DS10B20_map", ios_base::in);

  if(!in_map.is_open())
    {
      return -1;
    }
  else 
    {

      unsigned long long int id[NSENSORS];

      unsigned long long int buf;
      char *bufPtr = (char*) &buf;

      unsigned long long int addr = 0;
      int connector = 0;

      string temp;

      for (int iSensor = 0; iSensor < NSENSORS; iSensor++)
	{
	  in_addr.read(bufPtr, 8);
	  id[iSensor]=buf;
	}

      for (int iSensor = 0; iSensor < NSENSORS; iSensor++)
	{
	  in_map >> temp;
	  in_map >> connector;

	  addr = strtoull(temp.c_str(), NULL, 16);
	  for (int jSensor = 0; jSensor < NSENSORS; jSensor++)
	    {
	      if(addr==id[jSensor])
		{
		  map[jSensor]=connector;
		}
	    }

	}
    }
  return 0;
}
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TriggerType for Standard Acquisition
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef AMS
glb_Header IOfile::GetEventHeader()
{
	return this->header;
}
#elif ADAMO
VME_Header IOfile::GetEventHeader()
{
	return this->header;
}
#endif

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TriggerType for Standard Acquisition
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned int IOfile::GetEventNumber()
{
	return this->casis.ev;
}
unsigned short int IOfile::Get00Trigger1()
{
	unsigned short int mask = 0xF000;

	if((mask & (this->casis.trg_type))!=ZERO)
	{
		cout << "MSByte of TRIGGER_TYPE is not consitent with data acquisition" << endl;
		cout << "Maybe data are from a calibration file or are just corrupted" << endl;
	}

	return (mask & (this->casis.trg_type));
}
unsigned short int IOfile::Get00Trigger0()
{
	unsigned short int mask = 0x0F00;

	if((mask & (this->casis.trg_type))!=ZERO)
	{
		cout << "MSByte of TRIGGER_TYPE is not consitent with data acquisition" << endl;
		cout << "Maybe data are from a calibration file or are just corrupted" << endl;
	}

	return (mask & (this->casis.trg_type));
}
unsigned short int IOfile::GetLACTrigger()
{
	unsigned short int mask = 0x00F0;

	return (mask & (this->casis.trg_type));
}
unsigned short int IOfile::GetCCTrigger()
{
	unsigned short int mask = 0x000F;

	return (mask & (this->casis.trg_type));
}

#if defined(SPS2015) || defined(SPS2016) || defined(SPS2017Aug) || defined(SPS2017Nov)
unsigned short int IOfile::GetCCTime()
{
	return this->casis.trg_time;
}
#else
unsigned short int IOfile::GetCasisTime()
{
	return this->casis.trg_time;
}
#endif


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//TriggerType for Standard Calibration
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned short int IOfile::GetCalTag()
{
	unsigned short int mask = 0xF000;

	return (mask & (this->casis.trg_type));
}
int IOfile::GetCalChip()
{
	unsigned short int mask = 0x0F00;

	return ((mask & (this->casis.trg_type))>>8);
}

int IOfile::GetCalChannel()
{
	unsigned short int mask = 0x00F0;

	return ((mask & (this->casis.trg_type))>>4);
}
unsigned short int IOfile::GetCalTrigger()
{
	unsigned short int mask = 0x000F;

	return (mask & (this->casis.trg_type));
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Get Data and Gain
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned short int IOfile::GetCCUBEdata(int iCasis, int iCasisChannel)
{
	return this->casis.data[iCasis][iCasisChannel];
}


unsigned short int IOfile::GetCCGain(int iCasis, int iCasisChannel)
{
	//cout << "GAIN " << dec << iCasis << " " << iCasisChannel << "(" << (1<<iCasisChannel) << ")" << " " << hex << (this->casis.gain[iCasis]) << " = " << dec << (unsigned short int)(((this->casis.gain[iCasis]) & (1<<iCasisChannel)) >> iCasisChannel) << endl;

	const int iWord = iCasisChannel >= 16 ? 1 : 0;
	const int iShift = iCasisChannel >= 16 ? iCasisChannel-16 : iCasisChannel;

	return((unsigned short int)(((this->casis.gain[iCasis][iWord]) & (1<<iShift)) >> iShift));
}

/*
  int IOfile::GetSensor(int iSensor)
  {
  return this->map[iSensor];
  }
 */

/*
  float IOfile::GetTemp(int iSensor)
  {
  return this->casis.tempC[iSensor];
  }
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//Get Data and Gain from Channel of Chip under Calibration
//NB: This Part must be updated for SPS2017 upgrade
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

unsigned short int IOfile::GetCalData(int iCasis, int iCasisChannel)
{
	return this->casis.data[iCasis][iCasisChannel];
}

unsigned short int IOfile::GetCalGain(int iCasis)
{
	return GetCCGain(iCasis, 0);
}

bool IOfile::CheckCalData(int iCasis, int iCasisChannel)
{
	unsigned short int maskData = 0xFFFF;
	unsigned short int maskGain = 0xFFFE;
	unsigned short int andData = 0;
	unsigned short int andGain = 0;

	for(int iChip=0; iChip < NCASISCHIPS; iChip++)
	{

		andGain = 0x0FFF & (this->casis.gain[iCasis][1]);

		if(andGain!=maskGain)
		{
			cout << "At least 1 bite (except MSB) of Gain of Chip " << iChip << " is not consitent with data calibration" << endl;
			cout << "Maybe data are from an acquisition file or are just corrupted" << endl;

			return false;
		}

		andGain = maskGain & (this->casis.gain[iCasis][0]);

		if(andGain!=maskGain)
		{
			cout << "At least 1 bite (except MSB) of Gain of Chip " << iChip << " is not consitent with data calibration" << endl;
			cout << "Maybe data are from an acquisition file or are just corrupted" << endl;

			return false;
		}

		for(int iChannel=0; iChannel < NCASISCHANNELS; iChannel++)
		{
			if((iChip==iCasis)or(iChip==iCasis+3)or(iChip==iCasis+6))
			{
				if(iChannel==iCasisChannel)
				{
					continue;
				}
			}

			andData = maskData & (this->casis.data[iChip][iChannel]);

			if(andData!=maskData)
			{
				cout << "At least 1 bite non-calibrated channel Data[" << iChip << "][" << iChannel << "] is not consitent with data calibration" << endl;
				cout << "Maybe data are from an acquisition file or are just corrupted" << endl;

				return false;
			}
		}
	}
	return true;
}

void IOfile::CloseFile()
{
	in.close();
	//in_addr.close();
}
