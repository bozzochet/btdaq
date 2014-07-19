 /********************************************************************
 *
 * Filename:     test_gassys.c
 *
 * Author(s):    Francesca Spada
 *
 * Date:         oct 2006
 *
 * Changes       29-NOV-2006  Get ConfigFileName from CommandLine
 *               29-NOV-2006  Implemented GOTO in function jump(char*)
 *               30-NOV-2006  Implemented OPTION for branch after read
 *               30-NOV-2006  Implemented Counters 
 *               30-NOV-2006  Implemented Averaging for GP50 and Kulite
 *                            - no "Reading sensor" output from fslib
 *               30-NOV-2006  Set Line-Buffering for LogFile
 *               08-MAR-2007  FS: added spiro readout
 *               11-feb-2008  FS: added enable and disable
 * -------------------------------------------------------------------
 *
 *      Read config file and execute operations on gas sys prototype.
 *
 *********************************************************************/

/* #include "xformslib.h" */
#include "template.h"
#include "fslib.h"

char *date   = {"07-Jan-08"};
char *author = {"F.R.Spada"};
char *title  = {"Test of TRD Gas System"};

#ifdef USE_CAN
#include "uscmlib.h"
int JMDC_ID = 0x01;                                // global variable
int USCM_ID = 0x4E;                                // global variable

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int P = 0;                                         // global variable

bool use_CAN = TRUE;                               // global variable
bool use_MEM = FALSE;                              // global variable
bool use_UDP = FALSE;                              // global variable
bool use_TCP = FALSE;                              // global variable
bool use_Peter_TCP = FALSE;                        // global variable
bool use_PCIAMSW = FALSE;                          // global variable
#endif

char *pch, *buffer, *buffer0;
char *configfilename = "GasSys.config";
char  logfile_name[80];
int   start_time;
char  J1[80];
char  J2[80];
char  J3[80];

int8  bus;           // FS
int   clk     = 0;   // FS
int   dat     = 0;   // FS
FILE  *lFile;
int   port    = 0;   // SPIRO


// DALLAS Stuff
    int   IF_DTS = 0;
    int8  ndts[9]; 
    bool  ok3[64];
    float t8[64];
    int64 ID[9][64];
    int64 ID8_QM[25] = {
     0x69000800AAD53110ULL,   // 01 Plate fin
     0x8D000800AAF0BE10ULL,   // 02 CO2 1
     0x54000800AAD8DA10ULL,   // 03 CO2 Outlet
     0x54000800AAB46B10ULL,   // 04 CO2 2
     0x27000800AAF34710ULL,   // 05 Preheater 1
     0xD6000800AADA5F10ULL,   // 06 Preheater 2
     0x3E000800AACD0A10ULL,   // 07 Valve tower
     0x55000800AAF2AE10ULL,   // 08 CO2 feedline 1
     0x49000800AACE7210ULL,   // 09 CO2 feedline 2
     0xA5000800C5EBEE10ULL,   // 10 Four valve
     0xF0000800AAFF3510ULL,   // 11 2Valve filter
     0x91000800AB05D110ULL,   // 12 Xe feedline
     0xEA000800AAC7B810ULL,   // 13 Vent valve
     0xBE000800AB0DCC10ULL,   // 14 2Valve GP50
     0x76000800AB0F4410ULL,   // 15 Xe outlet
     0x1B000800AB014E10ULL,   // 16 Xe 1
     0x41000800AAD9F810ULL,   // 17 late
     0x7300080016802610ULL,   // 18 box C canister 1
     0xF000080045493510ULL,   // 19 box C canister 2
     0xA2000800AAA0F410ULL,   // 20 D tank
     0x710008008E8C3110ULL,   // 21 Xe 2
     0x7C000800C6016A10ULL,   // 22 Xe test 1
     0x50000800C5B62010ULL,   // 23 Xe test 2
     0x44000800C5A53C10ULL,   // 24 CO2 test 1
     0x17000800C5E57610ULL    // 25 CO2 test 2
    };

//  correct table:
//    No    Location               -------DTS-A--------    -------DTS-B--------    
//    in    on                     Rd                      Rd        
//  Chain   TRD-GAS               Seq       ID            Seq       ID
//  
//    01    Plate Middle Fin        3  180008008B5D0810    3 820008008B4BB410
//    02    CO2 #1                  8  400008008B487A10    6 9B0008008B4AFC10
//    03    CO2 Outlet Boss        14  D70008008B431510    4 870008008B3CCC10
//    04    CO2 #2                  6  490008008B359210   10 B40008008B690910
//    05    PreHeater #1           17  670008008B5D1710    7 680008008B3DF610
//    06    PreHeater #2           16  0B0008008B3B4D10   16 590008008B517310
//    07    Valve Tower            15  A50008008B527510   14 1C0008008B475D10
//    08    CO2 FeedLine #1         4  170008008B6F4410    5 7B0008008B399C10
//    09    CO2 FeedLine #2        12  D70008008B5E2910   20 6E0008008B66BF10
//    10    Four Valve              7  D00008008B5DB210   15 F80008008B482310
//    11    Two Valve Filter        0  60000800AAA9C010   12 F3000800AAE4C910
//    12    Xe FeedLine            18  AB000800AAA20F10    0 16000800AACA2010
//    13    Vent Valve              1  9A000800AAD5E010   11 60000800AAFF0910
//    14    Two Valve GP50          5  AF000800AAB09210   13 BC000800AA9A6D10
//    15    Xe Outlet Boss         10  6E000800AB064110   19 BA000800AA9D1710
//    16    Xe #1                  20  45000800AAA4FF10    2 B8000800AADB4410
//    17    Plate close to USS     13  D2000800AAF95910    8 B7000800AAB88E10
//    18    Box-C Canister         19  A9000800AADD0F10    9 FF000800AAD42110
//    19    Box-C Monitor Tube     11  08000800AAC20910   18 F4000800AAA00B10
//    20    Mixing Vessel           2  FE000800AAC15010    1 CD000800AB0CA010
//    21    Xe #2                   9  05000800AAF05610   17 98000800AAD5F310

//  wrong table:
    int64 ID8_FMB[23] = {
     0x1020caaa00080016ULL,   // 01 Plate Fin
     0x10a00cab000800cdULL,   // 02 CO2 1
     0x1044dbaa000800b8ULL,   // 03 CO2 Outlet Boss
     0x10b44b8b00080082ULL,   // 04 CO2 2
     0x10cc3c8b00080087ULL,   // 05 Preheater 1
     0x109c398b0008007bULL,   // 06 Preheater 2
     0x10fc4a8b0008009bULL,   // 07 Tower Valve
     0x10f63d8b00080068ULL,   // 08 CO2 Feedline 1
     0x108eb8aa000800b7ULL,   // 09 CO2 Feedline 2
     0x1021d4aa000800ffULL,   // 10 Four Valve
     0x1009698b000800b4ULL,   // 11 2 Valve Filter
     0x1009ffaa00080060ULL,   // 12 Xe Feedline
     0x10c9e4aa000800f3ULL,   // 13 Vent Valve
     0x106d9aaa000800bcULL,   // 14 2 Valve GP50
     0x105d478b0008001cULL,   // 15 Xe Outlet Boss
     0x1023488b000800f8ULL,   // 16 Xe 1
     0x1073518b00080059ULL,   // 17 Plate
     0x10f3d5aa00080098ULL,   // 18 Box C Monitor Tube
     0x100ba0aa000800f4ULL,   // 19 Box C Canister Ext
     0x10a6428b00080044ULL,   // 20 Box C Spirometer 1 (Mail Alexei)
     0x10ef368b000800d5ULL,   // 21 Box C Spirometer 2 (Mail Alexei)
     0x10179daa000800baULL,   // 22 D Tank
     0x10bf668b0008006eULL    // 23 Xe 2
    };

//  wrong table:
    int64 ID8_FMA[23] = {
      0x10c0a9aa00080060ULL,   // 01 Plate Fin
      0x10e0d5aa0008009aULL,   // 02 CO2 
      0x1050c1aa000800feULL,   // 03 CO2 Outlet Boss
      0x10085d8b00080018ULL,   // 04 CO2 2
      0x10446f8b00080017ULL,   // 05 Preheater 
      0x1092b0aa000800afULL,   // 06 Preheater 2
      0x1092358b00080049ULL,   // 07 Tower Valve
      0x10b25d8b000800d0ULL,   // 08 CO2 Feedline 1
      0x107a488b00080040ULL,   // 09 CO2 Feedline 2
      0x1056f0aa00080005ULL,   // 10 Four Valve
      0x104106ab0008006eULL,   // 11 2 Valve Filter
      0x1009c2aa00080008ULL,   // 12 Xe Feedline
      0x10295e8b000800d7ULL,   // 13 Vent Valve
      0x1059f9aa000800d2ULL,   // 14 2 Valve GP50
      0x1015438b000800d7ULL,   // 15 Xe Outlet Boss
      0x1075528b000800a5ULL,   // 16 Xe 1
      0x104d3b8b0008000bULL,   // 17 Plate
      0x10175d8b00080067ULL,   // 18 Box C Monitor Tube
      0x100fa2aa000800abULL,   // 19 Box C Canister Ext
      0x10564e8b00080017ULL,   // 20 Box C Spirometer 1 (Mail Alexei)
      0x1037468b000800ddULL,   // 21 Box C Spirometer 2 (Mail Alexei) 
      0x100fddaa000800a9ULL,   // 22 D Tank
      0x10ffa4aa00080045ULL,   // 23 Xe 2

};


void test_gassys(void);


/*********************************************************************
 *
 * Function:      main()
 * 
 *********************************************************************/
int main(int argc, char **argv)
{

  printf("\033[2J\n");
  printf("\033[1;0H\n ************************************************************************\n");
  printf(" * test.gassys  * %s * %s\n", author, date);
  printf(" ************************************************************************\n");
  printf(" **********              Operate TRD Gas System               ***********\n");
  printf(" ************************************************************************\n");
  printf("\n Usage: test_gassys [ Config_file Debug_Level ]\n");

  P = 0;
  if (argc > 1) configfilename = argv[1];
  printf("\n ConfigFilename = %s\n\n\n", configfilename);

  if (argc > 2) P = atoi(argv[2]);
  printf("\n Debug_Level = %d\n\n\n", P);

  start_time = (int)time(NULL);
  test_gassys();

  printf("\n Test ended succesfully");
  printf(" * see logfile %s\n", logfile_name);
  printf(" ************************************************************************\n\n");

  return 0;
}

void jump(char*);
void catch_error(int);
bool initialize_crate(void);
bool is_Marotta(char*);
bool is_flipper(char*);
bool is_pump(char*);
bool is_gp50(char*);
bool is_kulite(char*);
bool is_heater(char*);
bool is_channel(char*);
bool is_serial(char*);
bool is_tension(char*);
bool is_spirometer(char*);
bool is_mux(char*);
bool find_Marotta_box(char*, char*);
bool find_tension_box(char*, char*);
int  which_counter(char*);
int  which_dallas(char*);
int  which_dP(char*);
float  read_dallas(int);
void read_spirometer(int16*);
void read_gp50_ave(   char*, int16*, bool*, float*, int);
void read_kulite_ave( char*, int16*, bool*, float*, int);
float read_dP(int);
void read_ADC(int16*, int16*);

/*********************************************************************
 *
 * Function:      test_gasys()
 * Description:   read configuration file and execute operations
 *
 *********************************************************************/
void test_gassys(void) 
{ 

  int16    loc_sta = 1;
  bool     loc_chk = FALSE;
  float    t1;
  char      Label[80];
  char      Cid[80];
  FILE     *pFile;
  long     lSize;
  char     b[80]         = "start";
  char     boxID[80]     = "Bxx";
  char     valveID[80]   = "Vxx";
  float    valveTIME     = 0.;
  float    waitTIME      = 0.;
  char     enaID[80]     = "Exx";
  char     disID[80]     = "Dxx";
  float    heatTIME      = 0.;
  char     startID[80]   = "Sxx";
  char     stopID[80]    = "sxx";
  char     pumpSPEED[80] = "xx";
  char     readID[80]    = "Rxx";
  char     moduleID[80]  = "Mxx";
  float    pressMIN      = 0;
  float    pressMAX      = 0;
  float    tensionGp50   = 0.;
  float    sigKulite     = 0.;
  int16    sigHoney[256];
  char     HOTCOLD[80]   = "X";
  onechar  vstatus;
  //char     vRstatus[80]  = "X";
  onechar  tstatus;
  //char     tRstatus[80]  = "X";
  twochar  pstatus;
  //twochar  pRstatus;
  float    Counter[10] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
  //float    pre; 
  int16    spErr;                               /// SPIRO


  if (P > 2) printf(" *** invoking test_gassys\n");

  /* Read config file */
  pFile = fopen(configfilename, "rb");
  if (pFile == NULL) catch_error(1);
  fseek(pFile, 0, SEEK_END);
  lSize = ftell(pFile);
  rewind(pFile);
  buffer  = (char*) malloc(lSize);
  if (buffer == NULL) catch_error(2);
  buffer0 = (char*) malloc(lSize);
  if (buffer0 == NULL) catch_error(2);
  fread(buffer, 1, lSize, pFile);
  fclose(pFile);


  /* Create log file */
  sprintf(logfile_name, "data/test_gassys_log-%d.dat", start_time);
  lFile = fopen(logfile_name, "w");
  setlinebuf(lFile);            // get output line-by-line

  /* Begin test */
  printf(" %s \n", mytime());
  printf("\n Reading config file %s \n", configfilename);
  printf("\n------------------ \n\n");
  printf("%s\n", buffer);
  printf("------------------ \n");

  strcpy( buffer0, buffer);
  strcpy(J1, "-:");strcpy(J2, "-:");strcpy(J3, "-:");
  

  if (P > 1) printf("\n Splitting buffer in tokens:\n");
  pch  = strtok(buffer, "\n ");
  while (pch != NULL)
  {
    if (P > 1) printf(" -+-+- %s\n", pch);

    /* Initialize USCM */
    /* --------------- */
    if (strcasecmp (pch, "ADDR") == 0) {
      pch = strtok(NULL, "\n ");
      USCM_ID = (int)(atof(pch));
      if (P > 1) printf(" -+-+-+- USCM_ID = %s, 0x%X\n", pch, USCM_ID);
      fprintf(lFile,"%8d %s USCM boot  0x%X\n", (int)time(NULL)-start_time, mytime(), USCM_ID);
      if (!initialize_crate()) catch_error(3);
    }

    /* hot-cold */
    /* -------- */
    if (strcasecmp (pch, "slot") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(HOTCOLD, "%s", pch);
      if      (strcasecmp (pch, "H") == 0) {  clk = 0 ; dat = 0; }
      else if (strcasecmp (pch, "C") == 0) {  clk = 2 ; dat = 2; }
      else if (strcasecmp (pch, "FVH") == 0) {  clk = 1 ; dat = 1; }
      else if (strcasecmp (pch, "FVC") == 0) {  clk = 3 ; dat = 3; }
      fprintf(lFile,"%8d %s Slot %s\n", (int)time(NULL)-start_time, mytime(), HOTCOLD);
      if (P > 1) printf(" -+-+-+- clk = %d, dat = %d\n", clk, dat);
    }


    /* DTS Init */
    if (strcasecmp (pch, "DTS") == 0) {
      pch = strtok(NULL, "\n ");
      IF_DTS = (int)(atof(pch));
      if (P > 1) printf(" IF_DTS = %s, 0x%X\n", pch, IF_DTS);
      fprintf(lFile,"%8d %s IF_DTS = 0x%X\n", (int)time(NULL)-start_time, mytime(), IF_DTS);
    }


    /* Enable */
    /* ------ */
    else if (strcasecmp (pch, "enable") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(enaID, "%s", pch);
      if (P > 1) printf(" -+-+-+- enaID = %s  %s\n", pch, enaID);
      /* Marotta */
      if (is_Marotta(enaID)) {
	enable_Marotta_valve(enaID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  enable\n", (int)time(NULL)-start_time, mytime(), enaID);
      }
      /* Tensions */
      else if (is_tension(enaID)) {
	enable_tension(enaID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  enable\n", (int)time(NULL)-start_time, mytime(), enaID);
      }
      /* Heaters */
      else if (is_heater(enaID)) {
	enable_heater(enaID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  enable\n", (int)time(NULL)-start_time, mytime(), enaID);
      }
      /* Serials */
      else if (is_serial(enaID)) {
	enable_serial(enaID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  enable\n", (int)time(NULL)-start_time, mytime(), enaID);
      }
      /* Channels */
      else if (is_channel(enaID)) {
	enable_channel(enaID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  enable\n", (int)time(NULL)-start_time, mytime(), enaID);
      }
    }



    /* Disable */
    /* ------ */
    else if (strcasecmp (pch, "disable") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(disID, "%s", pch);
      if (P > 1) printf(" -+-+-+- disID = %s  %s\n", pch, disID);

      /* Marotta */
      if (is_Marotta(disID)) {
	find_Marotta_box(disID, boxID);
	disable_Marotta_valves(boxID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s VALVES %s disable\n", (int)time(NULL)-start_time, mytime(), boxID);
      }
      /* Tensions */
      else if (is_tension(disID)) {
	find_tension_box(disID, boxID);
	disable_tensions(boxID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s TENSIONS %s disable\n", (int)time(NULL)-start_time, mytime(), boxID);
      }
      /* Heaters */
      else if (is_heater(disID)) {
	disable_heaters(&loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s  HEATERS disable\n", (int)time(NULL)-start_time, mytime());
      }
      /* Serials */
      else if (is_serial(disID)) {
	disable_serials(&loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s  SERIALS disable\n", (int)time(NULL)-start_time, mytime());
      }
      /* Channels */
      else if (is_channel(disID)) {
	disable_channels(&loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s  CHANNELS disable\n", (int)time(NULL)-start_time, mytime());
      }
    }


    /* Open */ 
    /* ---- */
    else if (strcasecmp (pch, "open") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(valveID, "%s", pch);
      if (P > 1) printf(" -+-+-+- valveID = %s  %s\n", pch, valveID);

      /* Marotta */
      if (is_Marotta(valveID)) {
	pch = strtok(NULL, "\n ");
	valveTIME = atof(pch);
	if (P > 1) printf("  -+-+-+-  valveTIME = %s %f\n", pch, valveTIME);
	sleep(1);
	open_Marotta_valve(valveID, valveTIME, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  open  %5.2f\n", (int)time(NULL)-start_time, mytime(), valveID, valveTIME);
      }
      /* Flipper */
      else if (is_flipper(valveID)) {
	pch = strtok(NULL, "\n ");
	sprintf(moduleID, "%s", pch);
	open_flipper(valveID, moduleID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s %s open\n", (int)time(NULL)-start_time, mytime(), valveID, moduleID);
      }
    }

    /* Close */
    /* ----- */
    else if (strcasecmp (pch, "close") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(valveID, "%s", pch);
      if (P > 1) printf(" -+-+-+- valveID = %s  %s\n", pch, valveID);
      
      /* Flipper */
      if (is_flipper(valveID)) {
	pch = strtok(NULL, "\n ");
	sprintf(moduleID, "%s", pch);
	close_flipper(valveID, moduleID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s %s close\n", (int)time(NULL)-start_time, mytime(), valveID, moduleID);
      }

      /* Marotta */
      /* (open for a short time to overwrite the "open" command) */
      if (is_Marotta(valveID)) {
	valveTIME = 0.001;
	//enable_Marotta_valve(valveID, &loc_sta, &loc_chk);
	sleep(1);
	open_Marotta_valve(valveID, valveTIME, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  close\n", (int)time(NULL)-start_time, mytime(), valveID);
      }
    }

        
    /* Start */
    /* ----- */
    else if (strcasecmp (pch, "start") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(startID, "%s", pch);
      if (P > 1) printf(" -+-+-+- startID = %s  %s\n", pch, startID);

      /* Circulation pumps */
      if (is_pump(startID)) {
	pch = strtok(NULL, "\n ");
	sprintf(pumpSPEED, "%s", pch);
	if (P > 1) printf("  -+-+-+-  pumpSPEED = %s  %s\n", pch, pumpSPEED);
	turn_on_pump(startID, pumpSPEED, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  start %s\n", (int)time(NULL)-start_time, mytime(), startID, pumpSPEED);
      }
      /* Heaters */
      else if (is_heater(startID)) {
	pch = strtok(NULL, "\n ");
	heatTIME = atof(pch);
	if (P > 1) printf("  -+-+-+-  heatTIME = %s %f\n", pch, heatTIME);
	start_heater(startID, heatTIME, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s   start %5.2f\n", (int)time(NULL)-start_time, mytime(), startID, heatTIME);
      }
    }


    /* Stop */
    /* ---- */
    else if (strcasecmp (pch, "stop") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(stopID, "%s", pch);
      if (P > 1) printf(" -+-+-+- stopID = %s  %s\n", pch, stopID);
      /* Circulation pumps */
      if (is_pump(stopID)) {
	turn_off_pump(stopID, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s  stop\n", (int)time(NULL)-start_time, mytime(), stopID);
      }
      /* Heaters */
      /* (start for the shortest time possible to overwrite the "start" command) */
      else if (is_heater(stopID)) {
	heatTIME = 4.;
	start_heater(stopID, heatTIME, &loc_sta, &loc_chk);
	fprintf(lFile,"%8d %s %s   stop\n", (int)time(NULL)-start_time, mytime(), stopID);
      }
    }
   
 
    /* Readout */
    /* ------- */
    else if (strcasecmp (pch, "read") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(readID, "%s", pch);
      if (P > 1) printf(" -+-+-+- readID = %s  %s\n", pch, readID);

      /* Pressure sensors */
      if ( is_gp50(readID) || 
           is_kulite(readID) || 
           is_mux(readID)|| 
           which_counter(readID)>=0 ||
           which_dallas(readID)>=0  
	   ) {
	pch = strtok(NULL, "\n ");
	pressMIN = atof(pch);
	if (P > 1) printf("  -+-+-+-  pressMIN = %s %f\n", pch, pressMIN);
	pch = strtok(NULL, "\n ");
	pressMAX = atof(pch);
	if (P > 1) printf("  -+-+-+-  pressMAX = %s %f\n", pch, pressMAX);
	
	/* gp50 */
	if (is_gp50(readID)) {
	  convert_read_gp50(readID, &loc_sta, &loc_chk, &tensionGp50);
	  if (P==0) printf("  -*-*-*-  tensionGp50 =  %f %d %d\n", tensionGp50, loc_sta, loc_chk);
	  fprintf(lFile,"%8d %s %4s  read %6.1f sta %6d chk %6d\n", (int)time(NULL)-start_time, mytime(), readID, tensionGp50,loc_sta, loc_chk );
	  if (      tensionGp50 <  pressMIN ) { if ( strcmp(J1,"-:") != 0 ) jump(J1); else catch_error(11); }
	  else if ( tensionGp50 <= pressMAX ) { if ( strcmp(J2,"-:") != 0 ) jump(J2); }
	  else                                     { if ( strcmp(J3,"-:") != 0 ) jump(J3); else catch_error(12); }
	} 

	/* Kulite */
	else if (is_kulite(readID)) {
	  convert_read_kulite(readID, &loc_sta, &loc_chk, &sigKulite);
	  if (P > 1) printf(" sigKulite = %f \n", sigKulite);
	  fprintf(lFile,"%8d %s %4s  read  %6.1f\n", (int)time(NULL)-start_time, mytime(), readID, sigKulite);
	}

	/* Honeywell */
	else if (is_mux(readID)) {
	    loc_sta = 777;
	//read_honeywell(readID, &loc_sta, &loc_chk, &sigHoney);
	  read_honeywell(readID, &loc_sta, &loc_chk,  sigHoney);
          printf(" read_honeywell %s  sta:%d chk:%d\n",readID,loc_sta,loc_chk);
          fprintf(lFile,"read_honeywell %s  sta:%d chk:%d\n",readID,loc_sta,loc_chk);
	  if      (clk == 1 && dat == 1) {
	    printf("read %s: %6d %6d %6d %6d %6d [Volt]\n", 
			      readID, sigHoney[0], sigHoney[1], sigHoney[2], sigHoney[3], sigHoney[4]);
	    fprintf(lFile,"%8d %s %s read %6d %6d %6d %6d %6d \n", (int)time(NULL)-start_time, mytime(), readID, 
		    sigHoney[0], sigHoney[1], sigHoney[2], sigHoney[3], sigHoney[4]);
	  }
	  else if (clk == 3 && dat == 3) {
	    printf("read %s: %6d %6d %6d %6d %6d [Volt]\n", 
			      readID, sigHoney[8], sigHoney[9], sigHoney[10], sigHoney[11], sigHoney[12]);
	    fprintf(lFile,"%8d %s %s read %6d %6d %6d %6d %6d \n", (int)time(NULL)-start_time, mytime(), readID, 
		    sigHoney[8], sigHoney[9], sigHoney[10], sigHoney[11], sigHoney[12]);
	  }
	  if (      (float)loc_chk <  pressMIN ) { if ( strcmp(J1,"-:") != 0 ) jump(J1); else catch_error(11); }
	  else if ( (float)loc_chk <= pressMAX ) { if ( strcmp(J2,"-:") != 0 ) jump(J2); }
	  else                                   { if ( strcmp(J3,"-:") != 0 ) jump(J3); else catch_error(12); }
	}

	/* Counter */
	else if (which_counter(readID)>=0) {
          int i = which_counter(readID);
          float Ci = Counter[i];
	  if (P > 0) printf(" counter[%d] = %6.0f \n", i , Ci);
	  fprintf(lFile,"%8d %s %4s  read  %6.0f\n", (int)time(NULL)-start_time, mytime(), readID, Ci);
	  if (      Ci <  pressMIN ) { if ( strcmp(J1,"-:") != 0 ) jump(J1); else catch_error(11); }
	  else if ( Ci <= pressMAX ) { if ( strcmp(J2,"-:") != 0 ) jump(J2); }
	  else                       { if ( strcmp(J3,"-:") != 0 ) jump(J3); else catch_error(12); }
	}

	/* DALLAS  */
	else if (which_dallas(readID)>=0) {
          int i = which_dallas(readID);
          float Tdts = read_dallas(i);
          if(i>8){ // just output single local DTS
	    if (P > 1) printf(" T[%02d] = %8.2f \n", i-8 , Tdts);
     	    fprintf(lFile,"%8d %s %4s  read  %6.2f\n", (int)time(NULL)-start_time, mytime(), readID, Tdts);
	    if (      Tdts <  pressMIN ) { if ( strcmp(J1,"-:") != 0 ) jump(J1); else catch_error(11); }
	    else if ( Tdts <= pressMAX ) { if ( strcmp(J2,"-:") != 0 ) jump(J2); }
	    else                         { if ( strcmp(J3,"-:") != 0 ) jump(J3); else catch_error(12); }
          }
	}
        /* dP */
	else if (which_dP(readID)>=0) {
          int imux = which_dP(readID);
          float stat = read_dP(imux);
	    if (      stat <  pressMIN ) { if ( strcmp(J1,"-:") != 0 ) jump(J1); else catch_error(11); }
	    else if ( stat <= pressMAX ) { if ( strcmp(J2,"-:") != 0 ) jump(J2); }
	    else                         { if ( strcmp(J3,"-:") != 0 ) jump(J3); else catch_error(12); }
	}
      }

      /* spirometer */
      else if (is_spirometer(readID)) {
        pch = strtok(NULL, "\n ");
        port = (int)(atof(pch));
	read_spirometer(&spErr);
	if (spErr) catch_error(13);
      }


      /* Circulation pumps */
      else if (is_pump(readID)) {
	pstatus = read_pump_status(readID, &loc_sta, &loc_chk ); 
	if (P > 1) printf(" stato della pompa = %s %s \n", pstatus.x_, pstatus.y_);
	fprintf(lFile,"%8d %s %s  read  %s %s\n", (int)time(NULL)-start_time, mytime(), readID, pstatus.x_, pstatus.y_);
/* 	pch = strtok(NULL, "\n "); */
/* 	sprintf(pRstatus.x_, "%s", pch); */
/* 	if (P > 1) printf(" (stato richiesto = %s)\n", pRstatus.x_ ); */
/* 	if ( strcasecmp( pRstatus.x_, pstatus.x_ ) != 0 )   catch_error(6); */
/* 	else if ( strcasecmp( pRstatus.x_, "A") == 0 &&  strcasecmp( pRstatus.x_, pstatus.x_ ) == 0 ) { */
/* 	  pch = strtok(NULL, "\n "); */
/* 	  sprintf(pRstatus.y_, "%s", pch); */
/* 	  if (P > 1) printf(" velocita' della pompa = %s\n", pstatus.y_); */
/* 	  if (P > 1) printf(" (velocita' richiesta = %s  %s)\n", pch, pRstatus.y_ ); */
/* 	  if ( strcasecmp( pRstatus.y_, pstatus.y_ ) != 0 ) catch_error(7); */
/* 	} */
      }

      /* Marotta valve */
      else if (is_Marotta(readID)) {
	vstatus = read_Marotta_valve_status(readID, &loc_sta, &loc_chk );
	if (P > 1) printf(" stato della valvola = %s \n", vstatus.x_);
	fprintf(lFile,"%8d %s %s  read  %s\n", (int)time(NULL)-start_time, mytime(), readID, vstatus.x_);
/* 	pch = strtok(NULL, "\n "); */
/* 	sprintf(vRstatus, "%s", pch); */
/* 	if ( strcasecmp( vRstatus, vstatus.x_) != 0 )   catch_error(5); */
      }

      /* Tension */
      else if (is_tension(readID)) {
	tstatus = read_tensions_status(readID, &loc_sta, &loc_chk );
	if (P > 1) printf(" stato della tensione = %s \n", tstatus.x_);
	fprintf(lFile,"%8d %s %s  read  %s\n", (int)time(NULL)-start_time, mytime(), readID, tstatus.x_);
/* 	pch = strtok(NULL, "\n "); */
/* 	sprintf(tRstatus, "%s", pch); */
/* 	if ( strcasecmp( tRstatus, tstatus.x_) != 0 )   catch_error(5); */
      }


      
      strcpy(J1, "-:");strcpy(J2, "-:");strcpy(J3, "-:");
    }
    
    /* Wait */
    /* ---- */
    else if (strcasecmp (pch, "wait") == 0) {
      pch = strtok(NULL, "\n ");
      waitTIME = atof(pch);
      if (P > 1) printf(" waitTIME = %s %f\n", pch, waitTIME);
      sleep(waitTIME);
    }

    /* Goto */
    /* ---- */
    else if (strcasecmp (pch, "goto") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(Label, "%s:", pch);
      fprintf(lFile,"%8d %s goto %s\n", (int)time(NULL)-start_time, mytime(), Label);
      //if (!initialize_crate()) catch_error(3);      ///  COMMENT THIS TO AVOID USCM BOOT
      //sleep(20);                                    ///  AT EVERY CALL TO GOTO
      jump(Label);
    }
    
    /* Option */
    /* ------ */
    else if (strcasecmp (pch, "option") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(J1, "%s:", pch);
      pch = strtok(NULL, "\n ");
      sprintf(J2, "%s:", pch);
      pch = strtok(NULL, "\n ");
      sprintf(J3, "%s:", pch);
      if (P > 1) printf(" Option   %s  %s  %s \n", J1,J2,J3);
    }

    /* C0 */
    /* -- */
    else if (strcasecmp (pch, "c0") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(Cid, "%s", pch);
      if (P > 1) printf(" C0   %s \n", Cid);
      int i = which_counter(Cid);
      if ( i>=0 ) Counter[i] = 0.;
      else catch_error(9);
   }

    /* C+ */
    /* -- */
   else if (strcasecmp (pch, "c+") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(Cid, "%s", pch);
      int i = which_counter(Cid);
      if ( i>=0 ) {
         Counter[i] += 1.;
         if (P >= 0) printf(" C+   %s %6.0f\n", Cid, Counter[i]);
      }
      else catch_error(9);
   }

    /* C- */
    /* -- */
    else if (strcasecmp (pch, "c-") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(Cid, "%s", pch);
      if (P > 1) printf(" C-   %s \n", Cid);
      int i = which_counter(Cid);
      if ( i>=0 ) Counter[i] -= 1.;
      else catch_error(9);
    }

    /* C= */
    /* -- */
    else if (strcasecmp (pch, "c=") == 0) {
      pch = strtok(NULL, "\n ");
      sprintf(Cid, "%s", pch);
      pch = strtok(NULL, "\n ");
      float Cval = atof(pch);
      if (P > 1) printf(" C=   %s %6.0f\n", Cid, Cval);
      int i = which_counter(Cid);
      if ( i>=0 ) Counter[i] = Cval; 
      else catch_error(9);
    }


    sprintf(b, "%s", pch);
    pch = strtok(NULL, "\n ");
  } 
  
  t1 = 3.;

  free(buffer);
  fprintf(lFile,"%8d %s EXIT OK\n", (int)time(NULL)-start_time, mytime());
  fclose(lFile);
}



/*********************************************************************
 *
 * Function:      jump(*char)
 * Description:   reset buffer, scan for Label, resume at next command
 *
 *********************************************************************/
void jump( char* Label)
{
      if (P > 1) printf(" Goto  %s \n", Label);
      strcpy(buffer, buffer0);
      pch = strstr(buffer, Label);
      if( pch == NULL ) catch_error(8);
      pch = strtok( pch, "\n ");
}

/*********************************************************************
 *
 * Function:      catch_error(int)
 * Description:   describe error when exiting
 *
 *********************************************************************/
void catch_error(int errcode)
{

  printf("\n Test ended with error ");
  printf(" * see logfile %s\n", logfile_name);
  printf(" ************************************************************************\n\n");

  switch ( errcode )
     {
        case 1:	 printf("\n No configuration file found. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(1);

        case 2:	 printf("\n Unable to allocate memory. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(2);

        case 3:  printf("\n Communication error with USCM. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(3);

        case 4:  printf("\n Pressure out of the required range. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(4);

        case 5:  printf("\n Valve in the wrong status. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(4);

        case 6:  printf("\n Pump in the wrong status. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(4);

        case 7:  printf("\n Pump at the wrong speed. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(4);

        case 8:  printf("\n Label not found. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(5);

        case 9:  printf("\n Counter not found. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(5);

        case 11: printf("\n Value below the required range. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(6);

        case 12: printf("\n Value above the required range. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(7);

        case 13: printf("\n Spirometer readout failed. Exit\n\n");
	         fprintf(lFile,"%s EXIT %2d\n", mytime(), errcode);
		 fclose(lFile);
	         exit(8);

        default: printf("\n There was something wrong. Exit\n\n");
	         exit(9);

     }

}



/*********************************************************************
 *
 * Function:      initialize_crate()
 * Description:   boot USCM and check if communication is OK
 *
 *********************************************************************/
bool initialize_crate(void) 
{ 

  int16 err;
  int from = 2; //FLASH
  int i;
  char errmsg[80];
  int iflag;
  int8 ena, rst, man, mdw, par; 

  boot_USCM(from, &err);
  
  for (i=0; i<3; i++) {
    ping_USCM(0, NULL, NULL, NULL, &err);
    if (!err) break;
  }

  // err = 0;   // **************************************** !!!  DEBUG !!!

  if (err) {
    return FALSE;
  } else {
    printf("\n USCM 0x%X booted\n", USCM_ID);

    if( IF_DTS>0 ){
     // Find Dallas Sensors
     ena = IF_DTS; 
     start_DS1820_scan(ena, &err); 
     if (err){sprintf(errmsg,"start_DS1820_scan FAILED err=0x%X\n", err); PANIC(errmsg);} 

     int8 nid[9]; 
     int8 bus = IF_DTS; 
     timer(1, START); 
     while (bus) { 
       sleep(1); 
       check_DS1820_scan(&bus, ndts, &err); 
       if (err) {sprintf(errmsg,"check_DS1820_scan FAILED err=0x%X\n",err); PANIC(errmsg);} 
       if (timer(1, EXPIRED, 15.0)) PANIC("check_DS1820_scan TIMED OUT (3)\n"); 
     } 

     // get DTS IDs 
     int ib;
     iflag = 1;
     for(ib=0;ib<9;ib++){
       if( (IF_DTS&iflag)>0 ){
         nid[ib]=0;
         if (ndts[ib]) read_DS1820_table(ib+1, &nid[ib], &ID[ib][0], NULL, &err); 
         if (err) {sprintf(errmsg,"read_DS1820_table FAILED err=0x%X\n",err); PANIC(errmsg);} 
         if (nid[ib] != ndts[ib]){sprintf(errmsg,"DTS Bus %d ID-ERROR expected %d got %d\n",ib,ndts[ib],nid[ib]);PANIC(errmsg);} 
       }
       iflag *=2;
     }    

   
     int i8;
     for(i8=0;i8<ndts[7]; i8++){
        printf("Bus8 i %2d  ID %llx\n",i8,ID[7][i8]);
     }

     // Start Dallas Sensors 

     //if( n1_8) ena = 0x80; else ena = 0x00; 

     ena = IF_DTS;  // enable all flagged ports
     rst = IF_DTS;  // reset  all flagged ports  ??
     man = 0x00; 
     mdw = 0x00; 
     par = 1; 
     setup_DS1820_readout(ena, rst, man, mdw, par, &err); 
     if (err) {sprintf(errmsg,"setup_DS1820_readout FAILED err=0x%X\n",err);PANIC(errmsg);} 
 
     iflag = 1;
     for(ib=0;ib<9;ib++){
       if( (IF_DTS&iflag)>0 ){
         printf("started DALLAS Sensors on Port%d: %4d\n",ib+1,ndts[ib]); 
       }
       iflag *=2;
     }
    }

    return TRUE;
  }
 
}

/*******************************************************************
 * READ DALLAS
 *******************************************************************/
float read_dallas(int i){

  int8  nr=0;

  int16 err;
  int idts,j;
  float Tdts = 999.9;
  // int64 reverse_byte_order(int64);

  if(i<9){ // dump DTS port i
    fprintf(lFile,"%8d %s  DTS-Port%d \n", (int)time(NULL)-start_time, mytime(), i+1);
    Tdts = 0.0;
    nr = ndts[i];
    read_DS1820_temp_long(i+1, &nr, &ok3[0], &t8[0], NULL, &err);
    for(idts=0;idts<nr;idts++){
        printf(       "DTS%1d  %2d  %llx  %6.2f\n",i+1,idts,ID[i][idts],t8[idts]);
        fprintf(lFile,"DTS%1d  %2d  %llx  %6.2f\n",i+1,idts,ID[i][idts],t8[idts]);
    }

  } else {  // get single DTS value from local TRD-GAS chain on port8

    if( ndts[7] ) {
      nr = ndts[7];
      read_DS1820_temp_long(8, &nr, &ok3[0], &t8[0], NULL, &err);
      if (err) printf("read_Dallas_sensors FAILED %4d %8X\n", nr, err);
      else {    
       for( j=0; j<ndts[7]; j++ ){
	 // if(P > 2) printf("%4d %016llX %8.2f \n",j, reverse_byte_order(ID[j]), t8[j]);
	 if(P > 0) printf("%4d %016llX %8.2f \n",j, ID[7][j], t8[j]);
         //if ( reverse_byte_order(ID[j])==ID8_FMB[i] ) {
         //printf("IDcheck %llx  i-8 %d  j %d\n",ID8_FMB[i-9],i-9,j);

	 if(USCM_ID==0x14b)
	 {
           if ( ID[7][j]==ID8_FMA[i-9] ) {
             if(  ok3[j] ) Tdts=t8[j];
	      //printf("IDmatch %llx  i-8 %d  j %d\n",ID[7][j],i-9,j);
	   }
	 } //USCM hot for local A dallas chain


	 /* if(USCM_ID==0x14b)                       
	 {
           if ( ID[7][j]==ID8_FMB[i-9] ) {
             if(  ok3[j] ) Tdts=t8[j];
	      //printf("IDmatch %llx  i-8 %d  j %d\n",ID[7][j],i-9,j);
	   }
	 }
	 */  //USCM cold for local B dallas chain


       } 
      }
    }
    printf("\n Read sensor  T%02d %8.2f\n", i-8, Tdts);
  }


  return Tdts ;

}


/*******************************************************************
 * READ SPIROMETER
 *******************************************************************/
void read_spirometer(int16* spErr){

                             
  int16    err, error = 0x0000;               /// SPIRO
  int8     datsp[256];                        /// SPIRO
  int8     datspir[512];                      /// SPIRO
  int      i, n, nn;                          /// SPIRO
  int8     start[4] = {0x02, 'M', '0', '1'};  /// SPIRO
  int8     stop[4]  = {0x02, 'M', '0', '0'};  /// SPIRO
  int      updown=0;
  char     trtime[5];
  int      timeus;

  error = 0x0000;
  trtime[4] = '\0';
  if (P > 1) printf("                  *** error =  0x%04X\n", error);
  //boot_from_EEROM(&err);
  if (P > 1) printf(" boot_from_EEROM  *** error =  0x%04X\n", error);
  for (i=0; i<3; i++) {
    get_short_status(datsp, &err);
    if (!err) break;
  }
  error |= err;
  if (P > 1) printf(" get_short_status *** error =  0x%04X\n", error);
  enable_serial_port(port, &err);
  error |= err;
  write_serial_port(port, 4, stop, &err);
  error |= err;
  flush_serial_port(port, &err);
  error |= err;
  fprintf(lFile,"%8d %s %8s  read  %3d\n", (int)time(NULL)-start_time, mytime(), "spirometer ", port);
  write_serial_port(port, 4, start, &err);
  error |= err;
  sleep(10);
  write_serial_port(port, 4, stop, &err);
  error |= err;
  nn = 0;
  for (i=0; i<256; i=i+n) {
    read_serial_port(port, 256, &n, &datspir[i], &err);
    error |= err;
    if (n == 0) break;
    nn = nn + n;
  }

  for (i=0; i<nn; i++) {
    if (!(i % 7)) printf("\n");
    printf("%02X ", datspir[i]);
  }
  printf("\n");

/*   for (i=0; i<nn; i++) { */
/*     if ((i % 7) == 0) fprintf(lFile,"\n"); */
/*     if ((i % 7) == 0) fprintf(lFile,"%02XH ", datspir[i]); */
/*     else              fprintf(lFile,"%c ", datspir[i]); */
/*   } */

  for (i=0; i<nn; i++) {
    if ((i % 7) == 0 && i!=0 ) {
      printf("\n");
      timeus = strtol(trtime,NULL,16); 
      printf(" transit time %s %s %f\n", (updown&1)?" up   ":" down ", trtime, (float)timeus/100.);
      fprintf(lFile,"\n");
      fprintf(lFile," transit time %s %6.2f\n", (updown&1)?" up   ":" down ", (float)timeus/100.);
    }
    if ((i % 7) == 0) printf("%02XH ", datspir[i]);
    if ((i % 7) == 1) {
      updown = (int)datspir[i]-48;
      printf(" counter: %c %d ", datspir[i], updown);
      printf((updown&1)?" up   ":" down "); 
    }
    if (((i % 7) > 1) && ((i % 7) < 6)) {
      trtime[(i%7)-2] = datspir[i];
      printf("%c ", datspir[i]);
    }
    else              printf("%c ", datspir[i]);
  }

  printf("\n");
  *spErr = error;






}


/*******************************************************************
 * UTILITIES
 *******************************************************************/

int64 reverse_byte_order(int64 i){
  int j;
  int64 ii=0, byte=0, mask=0x00000000000000FFULL;

 
  for(j=0;j<8;j++){
    if (j<=3) byte = (i&mask)<<(7-2*j)*8;
    else      byte = (i&mask)>>(2*j-7)*8;
    ii |= byte;
    mask = mask << 8;
  }
  return ii;
}


bool is_Marotta(char* name)
{
#define n_marotta 21

  struct {
    char* nam;
  } valve[n_marotta] = {
    // box C
    {"V6a"},
    {"V6b"},
    {"V18a"},
    {"V18b"},
    {"V6a&V18a"},
    {"V6b&V18b"},
    // box S
    {"V1a"},
    {"V2a"},
    {"V3a"},
    {"V4a"},
    {"Sp1"},
    {"V10a"},
    {"V20a"},
    {"V1b"},
    {"V2b"},
    {"V3b"},
    {"V4b"},
    {"V10b"},
    {"V20b"},
    {"Sp2"},
    {"V20a&V20b"}
  };

  int i;
  bool valve_found = FALSE;

  if (P > 2) printf(" === is_Marotta: %s\n", name);
  for (i=0; i<n_marotta; i++) {
    if (strcasecmp (valve[i].nam, name) == 0) valve_found = TRUE;
  }

  return ((valve_found) ? TRUE : FALSE);
}


/*******************************************************************/

bool is_tension(char* name)
{
#define n_tensions 9

  struct {
    char* nam;
  } tension[n_tensions] = {
    // box C
    {"25V"},
    {"12V"},
    {"MCA"},
    {"CO2"},
    // Manifolds
    {"12VPS"},
    {"12VFV"},
    {"21V"},
    {"MUX"},
    {"21VeMUX"},

  };

  int i;
  bool tension_found = FALSE;

  if (P > 2) printf(" === is_tension: %s\n", name);
  for (i=0; i<n_tensions; i++) {
    if (strcasecmp (tension[i].nam, name) == 0) tension_found = TRUE;
  }

  return ((tension_found) ? TRUE : FALSE);
}



/*******************************************************************/

bool is_flipper(char* name)
{
#define n_flippers 5

  struct {
    char* nam;
  } flipper[n_flippers] = {
    {"FV1"},
    {"FV2"},
    {"FV3"},
    {"FV4"},
    {"FV5"}
  };

  int i;
  bool valve_found = FALSE;

  if (P > 2) printf(" === is_flipper: %s\n", name);
  for (i=0; i<n_flippers; i++) {
    if (strcasecmp (flipper[i].nam, name) == 0) valve_found = TRUE;
  }

  return ((valve_found) ? TRUE : FALSE);
}

/*******************************************************************/

bool is_pump(char* name)
{
#define n_pumps 2

  struct {
    char* nam;
  } pump[n_pumps] = {
    {"CP1"},
    {"CP2"}
  };

  int i;
  bool pump_found = FALSE;

  if (P > 2) printf(" === is_pump: %s\n", name);
  for (i=0; i<n_pumps; i++) {
    if (strcasecmp (pump[i].nam, name) == 0) pump_found = TRUE;
  }

  return ((pump_found) ? TRUE : FALSE);
}


/*******************************************************************/

bool is_gp50(char* name)
{
#define n_sensors 6

  struct {
    char* nam;
  } gp50[n_sensors] = {
    // box S
    {"P1a"},
    {"P2a"},
    {"P1b"},
    {"P2b"},
    // box C
    {"P3"},
    {"P4"}
  };

  int i;
  bool gp50_found = FALSE;

  if (P > 2) printf(" === is_gp50: %s\n", name);
  for (i=0; i<n_sensors; i++) {
    if (strcasecmp (gp50[i].nam, name) == 0) gp50_found = TRUE;
  }

  return ((gp50_found) ? TRUE : FALSE); 
}


/*******************************************************************/

bool is_kulite(char* name)
{
#define n_ksensors 3

  struct {
    char* nam;
  } kulite[n_ksensors] = {
    {"Pk1c"},
    {"Pk2c"},
    {"Pk1d"}
  };

  int i;
  bool kulite_found = FALSE;

  if (P > 2) printf(" === is_kulite: %s\n", name);
  for (i=0; i<n_ksensors; i++) {
    if (strcasecmp (kulite[i].nam, name) == 0) kulite_found = TRUE;
  }

  return ((kulite_found) ? TRUE : FALSE);
}

/*******************************************************************/

bool is_heater(char* name)
{
#define n_heaters 5

  struct {
    char* nam;
  } heater[n_heaters] = {
    {"H1"},
    {"H2"},
    {"H3"},
    {"H4"},
    {"H5"}
  };

  int i;
  bool heater_found = FALSE;

  if (P > 2) printf(" === is_heater: %s\n", name);
  for (i=0; i<n_heaters; i++) {
    if (strcasecmp (heater[i].nam, name) == 0) heater_found = TRUE;
  }

  return ((heater_found) ? TRUE : FALSE);
}

/*******************************************************************/


/// SPIRO ///

bool is_spirometer(char* name)
{
#define n_spiros 3

  struct {
    char* nam;
  } spiro[n_spiros] = {
    {"spirometer"},
    {"spiro"},
    {"ndd"}
  };

  int i;
  bool spiro_found = FALSE;

  if (P > 2) printf(" === is_spirometer: %s\n", name);
  for (i=0; i<n_spiros; i++) {
    if (strcasecmp (spiro[i].nam, name) == 0) spiro_found = TRUE;
  }

  return ((spiro_found) ? TRUE : FALSE);
}


/*******************************************************************/

bool is_channel(char* name)
{
#define n_channels 4

  struct {
    char* nam;
  } channel[n_channels] = {
    {"Ch1"},
    {"Ch2"},
    {"Ch3"},
    {"Ch4"}
  };

  int i;
  bool channel_found = FALSE;

  if (P > 2) printf(" === is_channel: %s\n", name);
  for (i=0; i<n_channels; i++) {
    if (strcasecmp (channel[i].nam, name) == 0) channel_found = TRUE;
  }

  return ((channel_found) ? TRUE : FALSE);
}


/*******************************************************************/

bool is_serial(char* name)
{
#define n_serials 4

  struct {
    char* nam;
  } serial[n_serials] = {
    {"RS232-1"},
    {"RS232-2"},
    {"RS232-3"},
    {"RS232-4"}
  };

  int i;
  bool serial_found = FALSE;

  if (P > 2) printf(" === is_serial: %s\n", name);
  for (i=0; i<n_serials; i++) {
    if (strcasecmp (serial[i].nam, name) == 0) serial_found = TRUE;
  }

  return ((serial_found) ? TRUE : FALSE);
}



/*******************************************************************/

bool is_mux(char* name)
{
#define n_muxs 4

  struct {
    char* nam;
  } mux[n_muxs] = {
    {"MUX1"},
    {"MUX2"},
    {"MUX3"},
    {"MUX4"}
  };

  int i;
  bool mux_found = FALSE;

  if (P > 2) printf(" === is_mux: %s\n", name);
  for (i=0; i<n_muxs; i++) {
    if (strcasecmp (mux[i].nam, name) == 0) mux_found = TRUE;
  }

  return ((mux_found) ? TRUE : FALSE);
}

/*******************************************************************/

bool find_Marotta_box(char* name, char* box)
{
#define n_marotta 21

  struct {
    char* nam;
    char* thisbox;
  } valve[n_marotta] = {
    // box C
    {"V6a",       "box C"},
    {"V6b",       "box C"},
    {"V18a",      "box C"},
    {"V18b",      "box C"},
    {"V6a&V18a",  "box C"},
    {"V6b&V18b",  "box C"},
    // box S
    {"V1a",       "box S"},
    {"V2a",       "box S"},
    {"V3a",       "box S"},
    {"V4a",       "box S"},
    {"Sp1",       "box S"},
    {"V10a",      "box S"},
    {"V20a",      "box S"},
    {"V1b",       "box S"},
    {"V2b",       "box S"},
    {"V3b",       "box S"},
    {"V4b",       "box S"},
    {"V10b",      "box S"},
    {"V20b",      "box S"},
    {"Sp2",       "box S"},
    {"V20a&V20b", "box S"}
  };

  int i;
  bool valve_found = FALSE;

  if (P > 2) printf(" === find_Marotta_box: %s\n", name);
  for (i=0; i<n_marotta; i++) {
    if (strcasecmp (valve[i].nam, name) == 0) {
      valve_found = TRUE;
      sprintf(box,"%s",valve[i].thisbox);
    }
  }

  return ((valve_found) ? TRUE : FALSE);
}

/*******************************************************************/

bool find_tension_box(char* name, char* box)
{

#define n_tensions 9

  struct {
    char* nam;
    char* thisbox;
  } tension[n_tensions] = {
    // box C
    {"25V", "box C"},
    {"12V", "box C"},
    {"MCA", "box C"},
    {"CO2", "box C"},
    // Manifolds
    {"12VPS",  "Manifolds"},
    {"12VFV",  "Manifolds"},
    {"21V",    "Manifolds"},
    {"MUX",    "Manifolds"},
    {"21VeMUX","Manifolds"}
  };

  int i;
  bool tension_found = FALSE;

  if (P > 2) printf(" === find_tension_box: %s\n", name);
  for (i=0; i<n_tensions; i++) {
    if (strcasecmp (tension[i].nam, name) == 0) {
      tension_found = TRUE;
      sprintf(box,"%s",tension[i].thisbox);
    }
  }
  if (P > 2) printf(" === find_tension_box: box %s\n", box);

  return ((tension_found) ? TRUE : FALSE);
}


/*******************************************************************/
/*******************************************************************/
/*******************************************************************/


/**************************************************************************************
 * TS. nov 2006
 **************************************************************************************/
int which_counter(char* name)
{
#define n_counters 10

  struct {
    char* nam;
  } counter[n_counters] = {
    {"C0"},
    {"C1"},
    {"C2"},
    {"C3"},
    {"C4"},
    {"C5"},
    {"C6"},
    {"C7"},
    {"C8"},
    {"C9"},
   };

  int i;
  int counter_found = -1;

  for (i=0; i<n_counters; i++) {
    if (strcasecmp (counter[i].nam, name) == 0) counter_found = i;
  }
  if (P > 2) printf(" === which_counter: %s %d\n", name, counter_found);

  return (counter_found);
}



/**************************************************************************************
 * TS. nov 2006
 **************************************************************************************/
int which_dallas(char* name)
{
#define n_dts 32

  struct {
    char* nam;
  } dts[n_dts] = {
    {"DTSP1"},
    {"DTSP2"},
    {"DTSP3"},
    {"DTSP4"},
    {"DTSP5"},
    {"DTSP6"},
    {"DTSP7"},
    {"DTSP8"},
    {"DTSP9"},
    {"T01"},
    {"T02"},
    {"T03"},
    {"T04"},
    {"T05"},
    {"T06"},
    {"T07"},
    {"T08"},
    {"T09"},
    {"T10"},
    {"T11"},
    {"T12"},
    {"T13"},
    {"T14"},
    {"T15"},
    {"T16"},
    {"T17"},
    {"T18"},
    {"T19"},
    {"T20"},
    {"T21"},
    {"T22"},
    {"T23"}
   };

  int i;
  int dts_found = -1;

  for (i=0; i<n_dts; i++) {
    if (strcasecmp (dts[i].nam, name) == 0) dts_found = i;
  }
  if (P > 2) printf(" === which_dallas: %s %d\n", name, dts_found);

  return (dts_found);
}


/**************************************************************************************
 * TS. nov 2006
 **************************************************************************************/
int which_dP(char* name)
{
#define n_dP 8

  struct {
    char* nam;
  } dP[n_dP] = {
    {"dP1"},
    {"dP2"},
    {"dP3"},
    {"dP4"},
    {"dP5"},
    {"dP6"},
    {"dP7"},
    {"dP8"},
   };

  int i;
  int dP_found = -1;

  for (i=0; i<n_dP; i++) {
    if (strcasecmp (dP[i].nam, name) == 0) dP_found = i;
  }
  if (P > 2) printf(" === which_dP: %s %d\n", name, dP_found);

  return (dP_found);
}


/**************************************************************************************
 * TS. nov 2006
 * average gp50 pressure sensors
 **************************************************************************************/
void read_gp50_ave( char* sensor_nam, int16* sta, bool* chk, float* tens, int Nav )
{
  float tens_i, mean, rms, var;
  double sum1, sum2;
  int i;
  float val[100];
  float pressure;
  *tens = 0.0;	  
  sum1 = 0.0;
  sum2 = 0.0;
  for ( i=0; i<Nav; i++){ 
    read_gp50(sensor_nam, sta, chk, &tens_i);
    val[i] = tens_i;
    sum1 += (double)tens_i;
    sum2 += (double)tens_i*tens_i;
  }
  mean = sum1/Nav;
  var  = sum2/Nav - (sum1/Nav)*(sum1/Nav);
  rms  = 0.0;

  if( var>0.0 ) {
    rms = sqrt(var);
    sum1 = 0.0;
    sum2 = 0.0;
    for ( i=0; i<Nav; i++){ 
      if( abs(val[i]-mean)<=2.*rms){
        sum1 += val[i];
        sum2 += 1.0;
      } else {
        if( P > 0 ) printf("%4s %4d %8.1f %8.1f %8.4f\n", sensor_nam, i, val[i], mean, rms);
      }
    }
  } else sum2=(double)Nav;

  *tens = sum1 / sum2;
  if( strcasecmp(sensor_nam,"P1a")==0)
    pressure=(*tens-7.9)/4500.0*3000.0; 
  else if(strcasecmp(sensor_nam,"P1b")==0)
    pressure=(*tens-12.58)/4500.0*3000.0;
  else if(strcasecmp(sensor_nam,"P3")==0)
    pressure=(*tens+700.0)/2920.0*14.5;    // mv->bar->psi
  else if(strcasecmp(sensor_nam,"P4")==0)
    pressure=(*tens+665.0)/2920.0*14.5;    // mv->bar->psi
  else  pressure=*tens;      
  printf("\n Read sensor %4s %8.1f %8.1f %4d %8.2f\n", sensor_nam, pressure,  *tens, (int)sum2, rms);  

}


/**************************************************************************************
 * TS. nov 2006
 * average Kulite pressure sensors
 **************************************************************************************/
void read_kulite_ave( char* sensor_nam, int16* sta, bool* chk, float* sig, int Nav )
{
  float sig_i, means,rmss,vars;
  double sum1s, sum2s;
  int i;
  float vals[100];
  float pressure;
  *sig = 0.0;	  
  sum1s=0.; sum2s=0.;
  for ( i=0; i<Nav; i++){ 
    read_kulite(sensor_nam, sta, chk, &sig_i);
    vals[i] = sig_i; 
    sum1s  += sig_i; 
    sum2s  += sig_i*sig_i;
  }
  means = sum1s/Nav;
  vars  = sum2s/Nav - (sum1s/Nav)*(sum1s/Nav);
  rmss  = 0.0;

  if( vars>0.0 ) {
    rmss = sqrt(vars);
    sum1s = 0.0; sum2s = 0.0;
    for ( i=0; i<Nav; i++){ 
      if( abs(vals[i]-means)<=2.*rmss){
        sum1s += vals[i];
        sum2s += 1.0;
      } else {
        if( P > 0 ) printf("%4s sig %4d %8.1f %8.1f\n", sensor_nam, i, vals[i], means);
      }
    }
  } else sum2s=(double)Nav;
 
  *sig = sum1s/sum2s; 

  if( strcasecmp(sensor_nam,"PK1c")==0)
    pressure=(*sig+27.13)/5000.0*3000.0; 
  else if(strcasecmp(sensor_nam,"PK2a")==0)
    pressure=(*sig+245)/5000.0*300.0;    
  else if(strcasecmp(sensor_nam,"PK2c")==0)
    pressure=(*sig-41.7)/5000.0*300.0;    
  else  pressure=*sig; 
  
  printf("\n Read sensor %4s %8.2f  %4d %8.2f\n", sensor_nam, pressure, (int)sum2s, rmss);
 
}


/**************************************************************************************
 * TS. nov 2006
 **************************************************************************************/


float read_dP(int ind){

float stat = 0.0;

#define n_mux 8

struct {
  char *nam;
  int16 msk;
  struct {
    int   mon;
    int   sta;
  } opn;
  struct {
    int   mon;
    int   sta;
  } cls;
  struct {
    FL_OBJECT *WRI;
  } ena;
} mux[n_mux] = {
  {" 1", 0x0},
  {" 2", 0x4},
  {" 3", 0x2},
  {" 4", 0x6},
  {" 5", 0x1},
  {" 6", 0x5},
  {" 7", 0x3},
  {" 8", 0x7}
};

#define n_board 2

struct {
  char *nam;
  int16 cmd;
  int32 msk;
  struct {
    int   mon;
    int   sta;
  } opn;
  struct {
    int   mon;
    int   sta;
  } cls;
  struct {
    FL_OBJECT *WRI;
  } ena;
} board[n_board] = {
  {" AC", 0x12, 0x400000},
  {" BD", 0x12, 0x200000}
};

  int16 muxs_ena_wri_cod  = mux[ind].msk << 16;
        muxs_ena_wri_cod += mux[ind].msk << 13;
        muxs_ena_wri_cod += mux[ind].msk << 10;
        muxs_ena_wri_cod += mux[ind].msk <<  7;
        muxs_ena_wri_cod += mux[ind].msk <<  4;
        muxs_ena_wri_cod += mux[ind].msk <<  1;

  int32 rea, wri, p;
  bool chk = 1;
  int16 err1,err2;

  int8 bus = (clk<<4)|dat; 

  wri  = 0x80880000; 
  wri += board[0].msk;
  wri += muxs_ena_wri_cod;
  p = parity32(wri);
  wri += p << 30;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err1);


  wri  = 0x80880000; 
  wri += board[1].msk;
  wri += muxs_ena_wri_cod;
  p = parity32(wri);
  wri += p << 30;
  exec_LeCroy_cmd(1, &bus, &wri, &rea, &chk, &err2);

  if(P>=0) printf("read dP%1d  ERR %4d %4d \n",ind,err1,err2);
  stat = (float)err1;
  stat += 1000.0*(float)err2;

  /*  

  sleep(1);

  int16 adc[256];
  //int n, i;
  //read_ADC(&adc, &err);
  read_ADC(adc, &err);

  if(ind == 4){

   printf("AC%1d %6d %6d %6d %6d %6d %6d \n", ind+1, adc[ 0], adc[ 1], adc[ 2], adc[ 3],adc[ 4],adc[ 5]);
   printf("BD%1d %6d %6d %6d %6d %6d %6d \n", ind+1, adc[16], adc[17], adc[18], adc[19],adc[20],adc[21]);

  fprintf(lFile,"AC%1d %6d %6d %6d %6d %6d %6d \n", ind+1, adc[ 0], adc[ 1], adc[ 2], adc[ 3],adc[ 4],adc[ 5]);
  fprintf(lFile,"BD%1d %6d %6d %6d %6d %6d %6d \n", ind+1, adc[16], adc[17], adc[18], adc[19],adc[20],adc[21]);
  }
  else{
   printf("AC%1d %6d %6d %6d %6d %6d \n", ind+1, adc[ 0], adc[ 1], adc[ 2], adc[ 3],adc[ 4]);
   printf("BD%1d %6d %6d %6d %6d %6d \n", ind+1, adc[16], adc[17], adc[18], adc[19],adc[20]);

  fprintf(lFile,"AC%1d %6d %6d %6d %6d %6d \n", ind+1, adc[ 0], adc[ 1], adc[ 2], adc[ 3],adc[ 4]);
  fprintf(lFile,"BD%1d %6d %6d %6d %6d %6d \n", ind+1, adc[16], adc[17], adc[18], adc[19],adc[20]);
  }

  */

  return stat;

}
