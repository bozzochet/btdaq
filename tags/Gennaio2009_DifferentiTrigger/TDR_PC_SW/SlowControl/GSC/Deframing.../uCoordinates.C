#include <uCoordinates.h>

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

/*  CRC calculation */
uint _Table[256];
const uint CRC32=0x04c11db7;
uint Table_init=0;

void _InitTable(){
  if(!Table_init){
    int i,j;
    uint crc;
    for(i=0;i<256;i++){
      crc=i<<24;
      for(j=0;j<8;j++)
	crc=crc&0x80000000 ? (crc<<1)^CRC32 : crc<<1;
      _Table[i]=crc;
    }
    Table_init=1;
  }
}

uint _CalcCRC(uint *pData, int nsize){
  int i,j;
  uint crc, *pu= (uint *)pData;
 _InitTable();
  if( nsize < 1) return 0;
  crc=~pu[0];
    for(i=1;i<(nsize/4);i++){
      for(j=0;j<3;j++)
	crc=_Table[crc>>24]^(crc<<8);
     crc=crc^pu[i];  
    }
  return ~crc;
}
