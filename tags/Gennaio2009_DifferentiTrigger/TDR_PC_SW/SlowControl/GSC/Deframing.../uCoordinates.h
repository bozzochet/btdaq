#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#define Year1997 852076800
#define Year1998 883612800-86400
#define Degr_Pi  0.017453292 /* Degrees ---> radians */ 
#define AE       149597875.1 /* astronomical unit, km */
#define Pi       3.141592654
#define Record_nbytes 2880 /* 12 * sizeof(float) * 60 recordov;*/
#define Record_in_file 60  /* Number of records in file */
#define temp_file "temp.temp" /* Temporal file */

typedef double matr3[3];
typedef double matr3x3[3][3];
typedef struct { /* polar coordinate system */
  double R; 
  double Teta; 
  double Phi;
} polar;
typedef struct { /* Euler angles system */
  double Roll;
  double Pitch;
  double Yaw;
} Euler;
//typedef unsigned char uchar;
  
matr3x3    E2000_mean,    /* Matrix for accounting for Precession */
           Emean_true,    /* Matrix for accounting for nutation */
           E2000_ITOD,    /* J2000 ---> inertial true-of-date */
           EITOD_GTOD,    /* internal true-of-date ---> Grenwich TOD */
           Ebod_1950,     /* M1950 -------> body */  
           Ebody_1950,    /* body axis  ------> M1950 */
           Ebody_GTOD,    /* body axis  ------> Greenwich TOD */
           EGTOD_LVLH,    /* GTOD  ---> Local vert. local horiz. LVLH */
           Ebody_LVLH,    /* body -----------> LVLH */
           EGTOD_UVW,     /* GTOD   -----> UVW system */
           Ebuf, Ebuf1; 


/*------ Matrixes with Coordinates and Velocity vectors --------*/
matr3 Coo_2000, /* inertial J-2000 system - coordinates */
      Vel_2000, /*                          velocity */
      Coo_ITOD,  /* inertial true-of-date,  coordinates */
      Vel_ITOD,	/*                          velocity */
      Coo_GTOD,  /* Greenwich true-of-date,  coordinates */
      Vel_GTOD,	/*                          velocity */
      Precess,  /* Precession angles, [0]-dzetta, [1]-ksi, [2] - teta */ 
      CosPrec,  /* Cosines of precession angles */      
      SinPrec,  /* Sines of precession angles   */	
      S_ITOD,   /* Sun position in inertial true-of-date */
      S_GTOD,   /* Sun position in GTOD */
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
       Euler_Phi,Euler_Tet,Euler_Psi,/*Euler angles */
       longi,Med,O,L,La,M,Ma,a,b,c,d,e,f,Q,S,time_of_day; /* temporals*/

int Coordinates(matr3 Coo_M1950, matr3 Vel_M1950, double q[4], time_t ytime, polar *Geo, polar *Geo_G, double *Vel_angle, Euler *Euler_LVLH, polar *Solar, double *Greenw_Phi, polar *Geo_Vel, polar *Geo_G_Vel);
double Radius(matr3 Coo);

/* ==== FOR RECORDS ========== */
uint _CalcCRC(uint *pData, int length);
uint record_n;
time_t Umin,Umax;
time_t utime;
int file_n;




