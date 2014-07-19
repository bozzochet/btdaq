#ifndef GonClass_hh
#define GonClass_hh

#include "CommClass.hh"
#include "GonPos.hh"


class GonClass: public CommClass {

private:

  GonPos pos;
  GonPos setpos;

  
public:
 
  GonClass(char* address="pgdaq0",int port=6666);
  virtual ~GonClass(){};

  //! Read the Goniometer and give back the lin1 coo
  double GetLin1();
  //! Read the Goniometer and give back the lin2 coo
  double GetLin2();
  //! Read the Goniometer and give back the amgle coo
  double GetAlpha();
  //! Read the Craddle Goniometer and give back the amgle coo
  double GetCraddle();
  //! Query the Gon and write the postion into pos
  int GetPos(int no_close=0);
  //! Give back the lin1 coo in memory
  double ReadLin1()  {return pos.GetLin1(); };
  //! Give back the lin2 coo in memory
  double ReadLin2()  {return pos.GetLin2(); };
  //! Give back the angle coo in memory
  double ReadAlpha() {return pos.GetAlpha();};
  double ReadAlphaG() {return pos.GetAlphaGrad();};

  double ReadCraddle() {return pos.GetCraddle();};
  double ReadCraddleG() {return pos.GetCraddleGrad();};
  //! Send the absolute movement command
  int MoveTo();
  //! Send the relative movement command
  int MoveToRel(); 
  //! Stop immediately all movements
  int Abort(); 
  //! Set Lin1 coo for movements
  void SetLin1(double l1){setpos.SetLin1(l1);} 
  //! Set Lin2 coo for movements
  void SetLin2(double l2){setpos.SetLin2(l2);}
  //! Set Alpha(rad) coo for movements
  void SetAlpha(double an){setpos.SetAlpha(an);}
  //! Set Alpha(grad) coo for movements
  void SetAlphaGrad(double an){setpos.SetAlphaGrad(an);}
  //! Set coo for movements (rad)
  void SetPos(double an,double crad,double l1, double l2){
    setpos.SetPos(an,crad,l1,l2);}
  //! Set coo for movements (grad)
  void SetPosGrad(double an,double crad,double l1, double l2){
    setpos.SetPosGrad(an,crad,l1,l2);}
  //! Perform a Calibration (Reset+ 32 0 0 m + 5 refmove + 57.3 0 0 r+ SetZero)
  int Calibrate();
  //! Move to mechanical zero
  int Reset();
  //! Move to  0 0 0 
  int GotoZero();
  //! Set a new origin of coo in this pos
  int SetZero();
  //! Rotate whitout moving the sample
  int RotateInPlace(double angle,double off2=0);
  //! Print the pos to stdout
  void Print(){pos.Print();}
  //! Send the raw string to the Goniomenter
  int SendRawCommand(const char* comm);

};
#endif
