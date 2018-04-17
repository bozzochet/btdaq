#ifndef AMS_HEADER
#define AMS_HEADER

struct AMS_Header {
	//---- Primary and secondary header ---------------------//
	unsigned short SIZE;
	unsigned short RRRWNODETYPE;
	unsigned short FBITAG;
	unsigned short TIMEMSB;
	unsigned short TIMELSB;
	//---- JMDC data block ----------------------------------//
	unsigned short JMDCSIZE;
	unsigned short JMDCRRRWNODETYPE;

	unsigned short RUNNUMMSB;
	unsigned short RUNNUMLSB;	
	unsigned short RUNTAGMSB;
	unsigned short RUNTAGLSB;
	unsigned short EVTNUMMSB;
	unsigned short EVTNUMLSB;
	unsigned short JMDCTIMEMSB;
	unsigned short JMDCTIMELSB;
	unsigned short JMDCTIMEFINEMSB;
	unsigned short JMDCTIMEFINELSB;
	unsigned short GReservedGroups;
	//---- DSP Slave Format ----------------------------------//
	unsigned short DSPSIZE;
	unsigned short DSPRRRWNODETYPE;
};

struct ROC_Header {
	unsigned short nIter;
	unsigned short Sec_MSB;
	unsigned short Sec_LSB;
	unsigned short Mic_MSB;
	unsigned short Mic_LSB;
};

struct glb_Header {
	AMS_Header ams_header;
	ROC_Header roc_header;
};

#endif
