#ifndef GonPos_HH
#define GonPos_HH
#define PI 3.141592653589793

#include <cstdio>

class  GonPos{

private:
  //! Angle of the rotor (microrad)
  double alpha;
  //! Angle of the Craddle (microrad)
  double craddle;
  //! Position of the tralastor fixed to the rotor (mm)
  double lin2;
  //! Position of the tralastor fixed to the bench (mm)
  double lin1;


public:
  GonPos(double an=0.,double cl=0.,double l1=0.,double l2=0.){lin1=l1;lin2=l2; alpha=an;craddle=cl;}
  GonPos(GonPos& org) {lin1=org.lin1;lin2=org.lin2; alpha=org.alpha;craddle=org.craddle;}  
  virtual ~GonPos(){};
  inline void Reset(){lin1=0.; lin2=0., alpha=0.;craddle=0.;}
  inline void SetPos(double an1,double cl,double in1 ,double in2){lin1=in1; lin2=in2; alpha=an1;craddle=cl;}
  inline void SetPosGrad(double an1,double cl, double in1,double in2){lin1=in1; lin2=in2; alpha=an1*PI/180.;craddle=cl*PI/180.;}
  inline void SetLin1(double in){ lin1=in;};
  inline void SetLin2(double in){ lin2=in;};
  inline void SetAlpha(double in ){alpha=in;};
  inline void SetAlphaGrad(double in ){alpha=in*PI/180.;};

  inline void SetCraddle(double in ){craddle=in;};
  inline void SetCraddleGrad(double in ){craddle=in*PI/180.;};

  inline double GetLin1(){return lin1;};
  inline double GetLin2(){return lin2;};
  inline double GetAlpha(){return alpha;};
  inline double GetAlphaGrad(){return alpha/PI*180.;};

  inline double GetCraddle(){return craddle;};
  inline double GetCraddleGrad(){return craddle/PI*180.;};

  void Print();
  void PrintGrad();
  

  inline bool operator==(GonPos b){
    return lin1==b.lin1 && lin2==b.lin2 && alpha==b.alpha && craddle==b.craddle ;
  }

  inline GonPos* operator+=(GonPos a){
    lin1+=a.lin1;
    lin2+=a.lin2;
    alpha+=a.alpha;
    craddle+=a.craddle;
    return this;
  }


  inline GonPos* operator/=(double count){
    lin1/=count;
    lin2/=count;
    alpha/=count;
    craddle/=count;
    return this;
  }


};



#endif
