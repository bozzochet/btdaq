
#ifdef __cplusplus
extern "C" { 
#endif

int send_AMSW_CMD(int port, int cmdlen, unsigned short* cmd, unsigned short* Event, int* usize) ;
void Print_Level(int level) ;

//~============================================================================
//#define MAXPARS 300
//#define ANSW_LIM 10000
#define MAXPARS 20*1024
#define ANSW_LIM 20*1024

#ifdef __cplusplus
}
#endif
