// 1.c
// test-bench_to_read_CAS_data

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

typedef struct { // polar coordinate system/
  double R; 
  double Teta; 
  double Phi;
} polar;

typedef struct { // Euler angles system
  double Roll;
  double Pitch;
  double Yaw;
} Euler;

#define integer8  unsigned char
#define integer16 unsigned short
#define integer32 unsigned int
#define integer64 unsigned long

#define foot_cm 30.48

char *block_directory = "/dats1/srdl/CAS/";
char *file_name[81];

FILE *file;
int nb_of_blocks_read;
int file_number;
struct {
  integer32 length;
  integer32 number;
  integer32 GSED_time[9];
  integer32 NASCOM_time[9];
  integer8  data[5202];
} CAS_message;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int read_CAS_message( void);
void get_CAS_time( int index, integer8 *status, time_t *value);
void get_CAS_data( int index, integer8 *status, double *value);

//-------------------------------------------------------------------

int read_CAS_message( void) {

  int DEBUG = 0;
  static FIRST = 1;
  static integer32 old_CAS_message_number;
  int i;
  
  if (fread( &CAS_message.length, sizeof( integer32), 1, file) == NULL) {
    if (DEBUG) {
      printf( "EOF after %d blocks.\n", nb_of_blocks_read);
    }
    return 0;
  }
  
  if (CAS_message.length != 5246) {
    printf( "length = %d\n", CAS_message.length);
  }
  
  if (DEBUG) printf( "CAS_message.length = %d\n", CAS_message.length);
  
  if (fread( &CAS_message.number, sizeof( integer32), 1, file) == NULL) {
    printf( "Format error 1 in block %d\n", nb_of_blocks_read);
    return 0;
  }
  
  if (!FIRST) {
    if (old_CAS_message_number + 9 != CAS_message.number) {
      printf( " %6.6d  %2d  old_CAS_message_number = %d,  new_CAS_message_number = %d\n",
            file_number, nb_of_blocks_read,
            old_CAS_message_number, CAS_message.number);
    }
  }
  FIRST = 0;
  old_CAS_message_number = CAS_message.number;
  
  if (DEBUG) printf( "CAS_message.number = %d\n", CAS_message.number);
  
  if (fread( CAS_message.GSED_time, sizeof( integer32), 9, file) == NULL) {
    printf( "Format error 2 in block %d\n", nb_of_blocks_read);
    return 0;
  }
  
  if (DEBUG) {
    printf( "CAS_message.GSED_time   = ");
    for (i=0; i<9; i++) printf("%8.8X ", CAS_message.GSED_time[i]);
    printf( "\n");
  }
  
  if (fread( CAS_message.NASCOM_time, sizeof( integer32), 9, file) == NULL) {
    printf( "Format error 3 in block %d\n", nb_of_blocks_read);
    return 0;
  }
  
  if (DEBUG) {
    printf( "CAS_message.NASCOM_time = ");
    for (i=0; i<9; i++) printf("%8.8X ", CAS_message.NASCOM_time[i]);
    printf( "\n");
  }
  
  if (fread( CAS_message.data, sizeof( integer8), CAS_message.length-76, file) == NULL) {
    printf( "Format error 4 in block %d\n", nb_of_blocks_read);
    return 0;
  }
  
  nb_of_blocks_read++;
  return 1;
}
//-------------------------------------------------------------------

void get_CAS_time( int index, integer8 *status, time_t *value) {

  int DEBUG = 0;
  int i;
  integer8 *p;
  integer64 temp;
  int day, hour, min, sec, msec;

  *status = CAS_message.data[index];
  for (i=6, p=(integer8*)&temp; i>0; i--) *p++ = CAS_message.data[index+i];

  day  = 10 * (10 * ((temp >> 46) & 0x3) + ((temp >> 42) & 0xF)) + ((temp >> 38) & 0xF);
  hour =       10 * ((temp >> 36) & 0x3) + ((temp >> 32) & 0xF);
  min  =       10 * ((temp >> 29) & 0x7) + ((temp >> 25) & 0xF);
  sec  =       10 * ((temp >> 22) & 0x7) + ((temp >> 18) & 0xF);
  msec = 10 * (10 * ((temp >> 11) & 0x3) + ((temp >>  7) & 0xF)) + ((temp >>  3) & 0xF);
  
  *value = 60 * (60 * (24 * (day - 1) + hour) + min) + sec + msec / 1000.0;

  if (DEBUG) {
    printf( "get_CAS_time: ");
    for (i=0; i<7; i++) printf( "%2.2X ", CAS_message.data[index+i]);
    printf( "   %16.16lX", temp);
    printf( "   %3d %2d %2d %2d %3d   ", day, hour, min, sec, msec);
    for (i=0x80; i>0x0; i>>=1) (*status & i) ? printf( "1") : printf( "0");
    printf( "   %8.8X\n", *value);
  }
}


//-------------------------------------------------------------------

void get_CAS_data( int index, integer8 *status, double *value) {

  int i;
  integer8 *p;
  
  *status = CAS_message.data[index];
  for (i=8, p=(integer8*)value; i>0; i--) *p++ = CAS_message.data[index+i];

}

//-------------------------------------------------------------------

void main( int argc, char *argv[]) {
  int DEBUG = 0;
  int TRACE_LEVEL = 0;
  int write_N_tuples = 0;
  int write_vectors = 1;
  
  int i, j;
  double check;
  
  integer8 status[4], time_status, coo_status, vel_status, q_status;
  int time_check, coo_check, vel_check, q_check;
  time_t time_value;
  double coo[3], vel[3], q[4];
  double altitude, velocity;
  
  double Greenw_Phi, Vel_angle;
  polar Geo, Geo_G, Geo_Vel, Geo_G_Vel, Solar;
  Euler Euler_LVLH;
  
  setbuf( stdout, NULL);	// set "no buffering" for stdout stream
  
  if (argc > 1) file_number = atoi( argv[1]);
  else          file_number = 4000;

  for ( ; file_number<18760; file_number++) {
//for (file_number=4000; file_number<18760; file_number++) { // the whole flight
//for (file_number=17459; file_number<17460; file_number++) {
    if (!(file_number % 100)) printf( "\r%6.6d\r", file_number);
    sprintf( file_name, "%s%6.6d", block_directory, file_number);
    if ((file = fopen( file_name, "rb")) == NULL) {
      printf( "Failed to open '%s'!\n", file_name);
      exit( 0);
    }
    nb_of_blocks_read = 0;
    
    while (read_CAS_message()) {
//--      
      get_CAS_time( 10, &time_status, &time_value);

      time_value = time_value + 883612800;

      if (0 || DEBUG || (time_status && (TRACE_LEVEL || 0))) {
        printf( "time_status = ");
        for (i=0x80; i>0x0; i>>=1) (time_status & i) ? printf( "1") : printf( "0");
        printf( "   time_value = %8.8X (NASCOM_time = %8.8X)\n", 
                time_value, CAS_message.NASCOM_time[0]);
      }
      
      if (time_status) continue;

//***********************************************************************
//**** this is to get CAS data to be compared with some of 'ntuples' ****
//
/*
{
static first = 1;
if (first) printf( "time_value = %d\n", time_value);
first = 0;
}
if (time_value < 897002926) continue;
printf( "time_value = %d\n", time_value);
if (time_value > 897003870) exit( 0);
*/
//
//***********************************************************************

      if (fabs( difftime( time_value, CAS_message.NASCOM_time[0])) < 10) time_check = 0;
      else                                                               time_check = 1;
      
      if (time_check) continue;
//--      
      get_CAS_data( 4490, &status[0], &coo[0]);
      get_CAS_data( 4508, &status[1], &coo[1]);
      get_CAS_data( 4526, &status[2], &coo[2]);

      coo_status = status[0] | status[1] | status[2];
      for (j=0; j<3; j++) coo[j] = coo[j] * foot_cm;

      if (DEBUG || (coo_status && (TRACE_LEVEL || 0))) {
        for (j=0; j<3; j++) {
          printf( "coo_status[%d] = ", j);
          for (i=0x80; i>0x0; i>>=1) (status[j] & i) ? printf( "1") : printf( "0");
          printf( "   coo_value[%d] = %13e\n", j, coo[j]);
        }
      }

      if (coo_status) continue;

      altitude = coo[0] * coo[0] + coo[1] * coo[1] + coo[2] * coo[2];
      altitude = sqrt( altitude) / 100000.0 - 6378.140;
      if (altitude > 200.0 && altitude < 400.0) coo_check = 0;
      else                                      coo_check = 1;
      
      if (coo_check) continue;
//--      
      get_CAS_data( 4687, &status[0], &vel[0]);
      get_CAS_data( 4701, &status[1], &vel[1]);
      get_CAS_data( 4715, &status[2], &vel[2]);

      vel_status = status[0] | status[1] | status[2];
      for (j=0; j<3; j++) vel[j] = vel[j] * foot_cm;

      if (0 || DEBUG || (vel_status && (TRACE_LEVEL || 1))) {
        for (j=0; j<3; j++) {
          printf( "vel_status[%d] = ", j);
          for (i=0x80; i>0x0; i>>=1) (status[j] & i) ? printf( "1") : printf( "0");
          printf( "   vel_value[%d] = %13e\n", j, vel[j]);
        }
      }

      if (vel_status) continue;

      velocity = vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2];
      velocity = sqrt( velocity) / 100000.0;
      if (velocity > 7.0 && velocity < 8.0) vel_check = 0;
      else                                  vel_check = 1;
      
      if (vel_check) continue;
//--      
      get_CAS_data( 3291, &status[0], &q[0]);
      get_CAS_data( 3305, &status[1], &q[1]);
      get_CAS_data( 3319, &status[2], &q[2]);
      get_CAS_data( 3333, &status[3], &q[3]);

      q_status = status[0] | status[1] | status[2] | status[3];

      if (DEBUG || (q_status && (TRACE_LEVEL || 0))) {
        for (j=0; j<4; j++) {
          printf( "q_status[%d] = ", j);
          for (i=0x80; i>0x0; i>>=1) (status[j] & i) ? printf( "1") : printf( "0");
          printf( "   q_value[%d] = %13e\n", j, q[j]);
        }
      }

      if (q_status) continue;

      check = q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3];
      if (fabs(1.0 - check) < 1.0e-3) q_check = 0;
      else                            q_check = 1;
      if (DEBUG || (q_check && (TRACE_LEVEL || 0))) {
        printf( "q = ");
        for (j=0; j<4; j++) {
          printf( "%13e ", q[j]);
        }
        printf( "   check = %13e (must be 1.0)\n", check);
      }

      if (q_check) continue;
//--      
      Coordinates( coo, vel, q, time_value,
                   &Geo, &Geo_G, &Vel_angle, &Euler_LVLH, 
                   &Solar, &Greenw_Phi, &Geo_Vel, &Geo_G_Vel);
//--      
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if (write_vectors) {
        struct tm *tm_ptr;
        float day, hour;
        static time_t old_time_value = 0;
        static FILE *file = NULL;
        if (file == NULL) file = fopen( "orbit", "w");
        if (file != NULL) {
///*
          if (difftime( time_value, old_time_value) > 9) {
            old_time_value = time_value;
            fprintf( file, " %d %13f %13f %13f %13f %13f %13f %13f %13f\n",
                   time_value-883612800-13390000,
                   altitude, 
                   velocity,
                   Geo_G.Teta * 57.296, 
                   Geo_G.Phi * 57.296,
                   Euler_LVLH.Roll * 57.296,
                   Euler_LVLH.Pitch * 57.296,
                   Euler_LVLH.Yaw * 57.296);
          }
//*/
/*
          if (difftime( time_value, old_time_value) > 60) {
            old_time_value = time_value;
            tm_ptr = gmtime( &time_value);
            day = tm_ptr->tm_yday + 1;
            hour = (float)tm_ptr->tm_hour+((float)tm_ptr->tm_min+(float)tm_ptr->tm_sec/60.0)/60.0;
            fprintf( file, " %6.3f %13f %13f %13f %13f %13f %13f %13f %13f\n",
                   (day - 154) * 24 + hour, 
                   altitude, 
                   velocity,
                   Geo_G.Teta * 57.296, 
                   Geo_G.Phi * 57.296,
                   Euler_LVLH.Roll * 57.296,
                   Euler_LVLH.Pitch * 57.296,
                   Euler_LVLH.Yaw * 57.296);
          }
*/
        }
      }
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//--      
    }
    fclose( file);
  }
  printf( "\n");
}

//-------------------------------------------------------------------
//*******************************************************************
//
//  this was given to me by Ilya Usoskin on 20.09.98
//
//   (later I did some cosmetic editing (10.10.98))
//
//*******************************************************************

#define Degr_Pi  0.017453292 // Degrees ---> radians
#define AE       149597875.1 // astronomical unit, km
#define Pi       3.141592654

typedef double matr3[3];
typedef double matr3x3[3][3];

matr3x3    E2000_mean,    // Matrix for accounting for precession
           Emean_true,    // Matrix for accounting for nutation
           E2000_ITOD,    // J2000 ---> inertial true-of-date
           EITOD_GTOD,    // internal true-of-date (TOD) ---> Greenwich TOD
           Ebod_1950,     // M1950 -------> body
           Ebody_1950,    // body axis  ------> M1950
           Ebody_GTOD,    // body axis  ------> Greenwich TOD
           EGTOD_LVLH,    // GTOD  ---> Local vert. local horiz. (LVLH)
           Ebody_LVLH,    // body -----------> LVLH
           EGTOD_UVW,     // GTOD   -----> UVW system
           Ebuf, Ebuf1; 


//------ Matrices with Coordinates and Velocity vectors ------
matr3 Coo_2000, // inertial J-2000 system - coordinates
      Vel_2000, //                          velocity
      Coo_ITOD, // inertial true-of-date,  coordinates
      Vel_ITOD,	//                          velocity
      Coo_GTOD, // Greenwich true-of-date,  coordinates 
      Vel_GTOD,	//                          velocity 
      Precess,  // Precession angles, [0]-dzetta, [1]-ksi, [2] - teta  
      CosPrec,  // Cosines of precession angles       
      SinPrec,  // Sines of precession angles   	
      S_ITOD,   // Sun position in inertial true-of-date 
      S_GTOD,   // Sun position in GTOD 
      Polinom3, Buf3, Buf31; 

/*polar Geo, Solar;        Polar geographical coordinates */

Euler Euler_GTOD;  /* Euler angles for GTOD */
  /*      Euler_LVLH;   Euler angles for LVLH */

double JC1900,    /* Julian century from J1900 */ 
      JD1900,    /* Julian day from 1900 */
      JC2000,   /* Julian century from J2000 */
      GMST,     /* Greenwich Mean Siderial Time */
      gam;    /* velocity flight path angle */

double eps,   /* obliquity of the ecliptic of date */
       dK,dK_r,  /* nutation of longitude */
       de,de_r,    /* nutation of obliquity */
       aG,    /* Ascention of Greenwich */
       Psi,   /* Shuttle azimuth angle of velocity */
       R_sun,S_DEC,S_RA, /* sun-earth distance, decl and right acs in ITOD */
       Euler_Phi,Euler_Tet,Euler_Psi; /*Euler angles */
       
double longi,Med,O,L,La,M,Ma,a,b,c,d,e,f,Q,S,time_of_day; /* temporals*/

int Coordinates( matr3 Coo_M1950, matr3 Vel_M1950, double q[4], time_t ytime,
                 polar *Geo, polar *Geo_G, double *Vel_angle, 
                 Euler *Euler_LVLH, polar *Solar, double *Greenw_Phi, 
                 polar *Geo_Vel, polar *Geo_G_Vel);
double Radius( matr3 Coo);

//-------------------------------------------------------------------

/* Recalculation Matrixes ---------------------*/
/* E1950 --- > 2000 */
matr3x3 T1950_2000={{0.99992567,-0.01118148,-0.004859},
		    {0.01118148, 0.99993748,-0.00002716},
		    {0.004859,  -0.00002717, 0.99998819}};
/* Precession angles calculation */
matr3x3 T_Precess={{0.6406161, 0.0000839, 0.000005},
		   {0.6406161, 0.0003041, 0.0000051},
		   {0.5567530, -0.0001185,-0.0000116}};

matr3 eps_c={-0.0130042,-0.00000016, 0.000000504}, /* for nutation */
      GMST_c={8640184.812866, 0.093104,-6.2e-06},  /* for GMST calculation */
      O_c  ={ 259.18330,-1934.1420,  0.002078},    /* for O calculation */
      L_c  ={ 279.69670, 36000.7689, 0.000303},
      La_c ={ 270.43420, 481267.8831,-0.001133},
      M_c  ={ 358.47580, 35999.0498,-0.000150},
      Ma_c ={ 296.10460, 477198.8491,0.009192};

double C[6] = /* for calculation of the Sun vector */
    {358.475845, 0.985600267, 279.696678, 0.9856473345, 1.918, 0.0201};


/* Mult. of matrixes 3x3*3x3 */
int matr3x3Xmatr3x3(matr3x3 M_out, matr3x3 M_trans, matr3x3 M_in) {
  int i,j,k;

  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      M_out[i][j]=0.;
      for (k=0; k<3; k++) {
	M_out[i][j] += M_trans[i][k] * M_in[k][j];
      }
    }
  }
  return 1;
}

/* Mult. of matrixes 3x3*3 */
int matr3x3Xmatr3(matr3 M_out, matr3x3 M_trans, matr3 M_in) {
  int i,j;

  for (i=0; i<3; i++) {
    M_out[i]=0.;
    for (j=0; j<3; j++) {
      M_out[i] += M_trans[i][j]*(M_in[j]);
    }
  }
  return 1;
}

/*Mult of matrix 3*3 */
double matr3Xmatr3(matr3 M_trans, matr3 M_in) {
  int i;
  double d;
    d=0.;
    for (i=0; i<3; i++) {
      d += M_trans[i] * M_in[i];
    }
    return d;
}

/* calculation of radius */
double Radius(matr3 M) {
  int i;
  double R=0.;
  for (i=0; i<3; i++) 
    R += M[i]*M[i];
  R=sqrt(R);
  return R;
}

/* Transformation of coordinates */
int Coordinates(matr3 Coo_M1950, matr3 Vel_M1950, double q[4], time_t utime, polar *Ge, polar *Ge_G, double *V_a, Euler *E_LVLH, polar *Sol, double *Green_Phi, polar *Geo_Vel, polar *Geo_G_Vel) {
  double R, JC2000_0UT, time_of_day;
  int i,j=0,ii,jj;
  polar Geo, Solar, Geo_V, Geo_G_V;
  double Vel_angle, Greenw_Phi, dd;
  Euler Euler_LVLH;

  /* M1950------> J2000 */
  matr3x3Xmatr3(Coo_2000, T1950_2000, Coo_M1950);
  matr3x3Xmatr3(Vel_2000, T1950_2000, Vel_M1950); 

  time_of_day = utime - floor(utime/86400.)*86400.;
  JC2000 = (utime / 86400 - 10957.5) / 36525; /* Julina Century J2000 */
  JC2000_0UT=((utime-time_of_day) / 86400 - 10957.5) / 36525; /* J2000 for 0UT*/
  JC1900 = (utime / 86400. + 25567.) / 36525. ;  /* Julian century J1900 */
  JD1900=JC1900*36525.; /* Julian days J1900 */

  /* For I2000 ----> polar */
  Geo.R=Radius(Coo_2000);  /* cm */
  R=Radius(Vel_2000);      /* cm */
  /* geographical latitude, radians */
  Geo.Teta=asin(Coo_2000[2]/Geo.R);   
  /* geographical longitude, radians */
  Geo.Phi =atan2(Coo_2000[1],Coo_2000[0]);
  /* polar coord velocity */
  Geo_V.R=R;
  Geo_V.Teta=asin(Vel_2000[2]/Geo_V.R);   
  Geo_V.Phi =atan2(Vel_2000[1],Vel_2000[0]);

  *Ge=Geo;
  *Geo_Vel=Geo_V;
  Vel_angle = R/Geo.R; /* angular velocity, rad/sec */
  /* --- J2000 -------> inertial true-of-date ------*/
  /* accounting for precession */
  R=1.;
  for (i=0; i<3; i++) {
    R *=JC2000;
    Polinom3[i]=R;
  }

  matr3x3Xmatr3(Precess, T_Precess, Polinom3); /* calculates Prec.angles, deg */
  for (i=0; i<3; i++) {
    CosPrec[i]=cosd(Precess[i]);
    SinPrec[i]=sind(Precess[i]);
  }
  E2000_mean[0][0]= CosPrec[0]*CosPrec[2]*CosPrec[1] - SinPrec[0]*SinPrec[1];
  E2000_mean[0][1]=-SinPrec[0]*CosPrec[2]*CosPrec[1] - CosPrec[0]*SinPrec[1];
  E2000_mean[0][2]=-SinPrec[2]*CosPrec[1];
  E2000_mean[1][0]= CosPrec[0]*CosPrec[2]*SinPrec[1] + SinPrec[0]*CosPrec[1];
  E2000_mean[1][1]=-SinPrec[0]*CosPrec[2]*SinPrec[1] + CosPrec[0]*CosPrec[1];
  E2000_mean[1][2]=-SinPrec[2]*SinPrec[1];
  E2000_mean[2][0]= CosPrec[0]*SinPrec[1];
  E2000_mean[2][1]=-SinPrec[0]*SinPrec[1];
  E2000_mean[2][2]= CosPrec[2];

  /* Accounting for Nutation */
  R=1.;
  for (i=0; i<3; i++) {
    R *=JC1900;
    Polinom3[i]=R;
  }  
  eps = 23.439291 + matr3Xmatr3(Polinom3, eps_c); /* obliquity in degrees */
  R=1.;
  for (i=0; i<3; i++) {
    Polinom3[i]=R;
    R *=JC1900;
  }  
  O = matr3Xmatr3(Polinom3, O_c); /* O - Ma - in degrees */
  L = matr3Xmatr3(Polinom3, L_c);
  La= matr3Xmatr3(Polinom3,La_c);
  M = matr3Xmatr3(Polinom3, M_c);
  Ma= matr3Xmatr3(Polinom3,Ma_c);
  dK = -(17.2327+0.01737*JC1900)*sind(O) /* dKsi - in seconds of degree */
    -(1.2729 +0.00013*JC1900)*sind(2*L)
    +0.2088*sind(2*O)-0.2307*sind(2*La)
    +(0.1261-0.00031*JC1900)*sind(M)+0.0675*sind(Ma)
    -(0.0497-0.00012*JC1900)*sind(2*L+M)
    -0.0342*sind(2*La-O)-0.0261*sind(2*La+Ma)
    +0.0214*sind(2*L-M)-0.0149*sind(2*L-2*La+Ma)
    +0.0124*sind(2*L-O)+0.0114*sind(2*La-Ma);
  de = (9.2100+0.00091*JC1900)*cosd(O) /* depsilon - in seconds of degree */
    +(0.5522-0.00029*JC1900)*cosd(2*L)
    -0.0904*cosd(2*O)+0.0884*cosd(2*La)
    +0.0216*cosd(2*L+M)+0.0183*cosd(2*La-O)
    +0.0113*cosd(2*La+Ma)-0.0093*cosd(2*L-M)-0.0066*cosd(2*L-O);
  de_r = de/3600.; de_r *= Degr_Pi; /* seconds of degrees ---> radians */  
  dK_r = dK/3600.; dK_r *= Degr_Pi; /* seconds of degrees ---> radians */
  Emean_true[0][0]=Emean_true[1][1]=Emean_true[2][2]=1.;
  Emean_true[0][1]=-dK_r*cosd(eps); Emean_true[1][0]=-Emean_true[0][1];
  Emean_true[0][2]=-dK_r*sind(eps); Emean_true[2][0]=-Emean_true[0][2];
  Emean_true[1][2]=-de_r; Emean_true[2][1]=-Emean_true[1][2];

  matr3x3Xmatr3x3(E2000_ITOD, Emean_true, E2000_mean);

  matr3x3Xmatr3(Coo_ITOD, E2000_ITOD, Coo_2000);
  matr3x3Xmatr3(Vel_ITOD, E2000_ITOD, Vel_2000);

  Buf3[0]=1.;
  Buf3[1]=Buf3[2]=0.; /* Greenwich in I2000 */
  matr3x3Xmatr3(Buf31, E2000_ITOD, Buf3); /* Greenwich in ITOD */

  /* ITOD -----> Greenwich true-of-date */
  R=1.;
  for (i=0; i<3; i++) {
    R *= JC2000_0UT;
    Polinom3[i]=R;
  }
  GMST = (24110.54841 + matr3Xmatr3(Polinom3, GMST_c)+time_of_day) * 15; /*seconds*/  
  aG = GMST + dK*cosd(eps+de/3600);
  aG = aG/3600.; /* seconds --> grad */

  EITOD_GTOD[0][0]=EITOD_GTOD[1][1]=cosd(aG);
  EITOD_GTOD[0][1]=sind(aG); EITOD_GTOD[1][0]=-EITOD_GTOD[0][1];
  EITOD_GTOD[2][2]=1.;
  EITOD_GTOD[0][2]=EITOD_GTOD[2][0]=EITOD_GTOD[1][2]=EITOD_GTOD[2][1]=0.;

  matr3x3Xmatr3(Coo_GTOD, EITOD_GTOD, Coo_ITOD);
  matr3x3Xmatr3(Vel_GTOD, EITOD_GTOD, Vel_ITOD);

  Geo.R=Radius(Coo_GTOD);  /* cm */
  R=Radius(Vel_GTOD);      /* cm */
  /* geographical latitude, radians */
  Geo.Teta=asin(Coo_GTOD[2]/Geo.R);   
  /* geographical longitude, radians */
  Geo.Phi =atan2(Coo_GTOD[1],Coo_GTOD[0]);

  *Ge_G=Geo;

  Geo_G_V.R=R;
  Geo_G_V.Teta=asin(Vel_GTOD[2]/Geo_G_V.R);   
  Geo_G_V.Phi =atan2(Vel_GTOD[1],Vel_GTOD[0]);

  *Geo_G_Vel=Geo_G_V;


  *V_a=Vel_angle;

  matr3x3Xmatr3(Buf3, EITOD_GTOD, Buf31); /* Greenwich 2000 in GTOD */ 

  Greenw_Phi =atan2(Buf3[1],Buf3[0]); /* Greenwich 2000 in GTOD, phi */

  *Green_Phi = -Greenw_Phi+2*Pi;

  /*=========== ANGLES CALCULATION ===========*/
  /* body axis ----> M1950 */
  Ebod_1950[0][0]=q[0]*q[0]+q[1]*q[1]-q[2]*q[2]-q[3]*q[3];
  Ebod_1950[1][1]=q[0]*q[0]-q[1]*q[1]+q[2]*q[2]-q[3]*q[3];
  Ebod_1950[2][2]=q[0]*q[0]-q[1]*q[1]-q[2]*q[2]+q[3]*q[3];
  Ebod_1950[0][1]=2*(q[1]*q[2]-q[0]*q[3]);
  Ebod_1950[1][0]=2*(q[1]*q[2]+q[0]*q[3]);
  Ebod_1950[0][2]=2*(q[1]*q[3]+q[0]*q[2]);
  Ebod_1950[2][0]=2*(q[1]*q[3]-q[0]*q[2]);
  Ebod_1950[1][2]=2*(q[2]*q[3]-q[0]*q[1]);
  Ebod_1950[2][1]=2*(q[2]*q[3]+q[0]*q[1]);

  for (ii=0; ii<3; ii++) {
    for (jj=0; jj<3; jj++) {
      Ebody_1950[ii][jj]=Ebod_1950[jj][ii];
    }
  }


  /* body axis ---> Greenwich TOD */
  matr3x3Xmatr3x3(Ebuf, T1950_2000, Ebody_1950); 
  matr3x3Xmatr3x3(Ebuf1, E2000_ITOD, Ebuf);
  matr3x3Xmatr3x3(Ebody_GTOD, EITOD_GTOD, Ebuf1); 

  /* Pitch/Yaw/Roll Euler angles relative to GTOD, in radians */
  Euler_GTOD.Roll = atan2(-Ebody_GTOD[2][1],Ebody_GTOD[1][1]);
  Euler_GTOD.Pitch = atan2(-Ebody_GTOD[0][2],Ebody_GTOD[0][0]);
  Euler_GTOD.Yaw = asin(Ebody_GTOD[0][1]);

  /* GTOD -----> LVLH */
  a = Coo_GTOD[2]*Coo_GTOD[2]*Vel_GTOD[0] - Vel_GTOD[2]*Coo_GTOD[0]*Coo_GTOD[2]
    - Coo_GTOD[0]*Coo_GTOD[1]*Vel_GTOD[1] + Vel_GTOD[0]*Coo_GTOD[1]*Coo_GTOD[1];
  b = Coo_GTOD[0]*Coo_GTOD[0]*Vel_GTOD[1] - Vel_GTOD[0]*Coo_GTOD[0]*Coo_GTOD[1]
    - Coo_GTOD[1]*Coo_GTOD[2]*Vel_GTOD[2] + Vel_GTOD[1]*Coo_GTOD[2]*Coo_GTOD[2];
  c = Coo_GTOD[1]*Coo_GTOD[1]*Vel_GTOD[2] - Vel_GTOD[1]*Coo_GTOD[1]*Coo_GTOD[2]
    - Coo_GTOD[0]*Coo_GTOD[2]*Vel_GTOD[0] + Vel_GTOD[2]*Coo_GTOD[0]*Coo_GTOD[0];
  d = Vel_GTOD[1]*Coo_GTOD[2] - Coo_GTOD[1]*Vel_GTOD[2];
  e = Vel_GTOD[2]*Coo_GTOD[0] - Coo_GTOD[2]*Vel_GTOD[0];
  f = Vel_GTOD[0]*Coo_GTOD[1] - Coo_GTOD[0]*Vel_GTOD[1];
  Q = sqrt(a*a + b*b + c*c);
  S = sqrt(d*d + e*e + f*f);
  EGTOD_LVLH[0][0]=a/Q;
  EGTOD_LVLH[0][1]=b/Q;
  EGTOD_LVLH[0][2]=c/Q;
  EGTOD_LVLH[1][0]=d/S;
  EGTOD_LVLH[1][1]=e/S;
  EGTOD_LVLH[1][2]=f/S;
  EGTOD_LVLH[2][0]=-Coo_GTOD[0]/Geo.R;
  EGTOD_LVLH[2][1]=-Coo_GTOD[1]/Geo.R;
  EGTOD_LVLH[2][2]=-Coo_GTOD[2]/Geo.R;
  /* Body ----> LVLH*/
  matr3x3Xmatr3x3(Ebody_LVLH, EGTOD_LVLH, Ebody_GTOD);

  /* Pitch/Yaw/Roll Euler angles relative to LVLH, degrees */
  Euler_LVLH.Roll = atan2(-Ebody_LVLH[2][1],Ebody_LVLH[1][1]); /* roll */
  Euler_LVLH.Pitch= atan2(-Ebody_LVLH[0][2],Ebody_LVLH[0][0]); /* pitch */
  Euler_LVLH.Yaw = asin(Ebody_LVLH[0][1]);                   /* yaw */

  *E_LVLH = Euler_LVLH;

/* ============ SOLAR ANGLES calculation ========== */
  Med = modf((C[0] + C[1]*JD1900)/360.,&dd)*360.; /* mean anomaly of the sun, deg. */
  longi = C[2]+C[3]*JD1900+C[4]*sind(Med)+C[5]*sind(2*Med); /* deg. */
  /* Sun position in ITOD */
  R_sun = AE*(1.00014-0.01671*cosd(Med)-0.00014*cosd(2*Med)); /* Sun-earth dist*/
  S_ITOD[0]=cosd(longi);
  S_ITOD[1]=sind(longi)*cosd(eps);
  S_ITOD[2]=sind(longi)*sind(eps);
  /* Sun position in GTOD */
  matr3x3Xmatr3(S_GTOD, EITOD_GTOD, S_ITOD);
  Solar.R=R_sun;
  Solar.Phi =atan2(S_GTOD[1],S_GTOD[0]); 
  /* geographical longitude */
  Solar.Teta =asin(S_GTOD[2]);

  *Sol = Solar;

  return 0;
}
