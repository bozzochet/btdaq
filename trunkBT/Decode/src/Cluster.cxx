#include "Cluster.hh"
#include "EventUtils.hh"
#include <cmath>
#include <string.h>
#include <functional>

Cluster::Cluster() {
  address = 0;
  length = 0;
  memset(Signal, 0, MAXLENGHT * sizeof(Signal[0]));
  memset(Noise, 0, MAXLENGHT * sizeof(Noise[0]));
  memset(Status, 0, MAXLENGHT * sizeof(Status[0]));
  memset(SignalVAEqualized, 0, MAXLENGHT * sizeof(SignalVAEqualized[0]));
  memset(NoiseVAEqualized, 0, MAXLENGHT * sizeof(NoiseVAEqualized[0]));
  bad = 0;
  golden = 0;
  ladder = -1;
  side = -1;
  SCHANN=-1;
  KCHANN=-1;
}

Cluster::Cluster(const Cluster &orig) : TObject(orig) {

  ladder = orig.ladder;
  side = orig.side;
  address = orig.address;
  length = orig.length;
  for (int ii = 0; ii < MAXLENGHT; ii++) {
    Signal[ii] = orig.Signal[ii];
    Noise[ii] = orig.Noise[ii];
    Status[ii] = orig.Status[ii];
    SignalVAEqualized[ii] = orig.SignalVAEqualized[ii];
    NoiseVAEqualized[ii] = orig.NoiseVAEqualized[ii];
  }
  bad = orig.bad;
  golden = orig.golden;
  SCHANN=orig.SCHANN;
  KCHANN=orig.KCHANN;
}

void Cluster::Build(int lad, int sid, int add, int len, float *sig, float *noi, int *stat, int Sig2NoiRatio,
                    int CNStatus, int PowBits, int badin) {

  ladder = lad;
  side = sid;
  address = add;
  if (len > MAXLENGHT)
    length = MAXLENGHT;
  else
    length = len;
  for (int ii = 0; ii < length; ii++) {
    Signal[ii] = sig[ii];
    Noise[ii] = noi[ii];
    Status[ii] = stat[ii];
  }
  snratio = Sig2NoiRatio;
  CNstatus = CNStatus;
  powbits = PowBits;
  bad = badin;

  //  this->ApplyVAEqualization();

  return;
}

void Cluster::Clear() {
  address = 0;
  length = 0;
  memset(Signal, 0, MAXLENGHT * sizeof(Signal[0]));
  memset(Noise, 0, MAXLENGHT * sizeof(Noise[0]));
  memset(Status, 0, MAXLENGHT * sizeof(Status[0]));
  memset(SignalVAEqualized, 0, MAXLENGHT * sizeof(SignalVAEqualized[0]));
  memset(NoiseVAEqualized, 0, MAXLENGHT * sizeof(NoiseVAEqualized[0]));
  bad = 0;
  ladder = -1;
  side = -1;
}

int Cluster::GetAddress() const { return address; }

int Cluster::GetSeed() const {
  float max = -1000;
  int seed = -1;
  for (int ii = 0; ii < length; ii++) {
    if (Noise[ii] != 0 && Signal[ii] / Noise[ii] >= max && Status[ii] == 0) {
      max = Signal[ii] / Noise[ii];
      seed = ii;
    }
  }
  return seed;
}

int Cluster::GetSeedAdd() const { return address + GetSeed(); }

float Cluster::GetCSignal(int aa) const {

  //  int stadd=address+aa;
  //   int vanum=stadd/64;
  //   float c1= Signal[aa];
  //   float corr=0.;
  //   if(aa>0){
  //     int stadd2=address+aa-1;
  //     int vanum2=stadd2/64;
  //     float c2= Signal[aa-1];
  //     corr=c2*0.;
  //   }
  //   if (side==1) return c1-corr;
  //   else return c1;

  float par0 = GainCorrectionPars::Instance()->GetPar(GetJinf(), GetTDR(), GetVA(aa), 0);
  float par1 = GainCorrectionPars::Instance()->GetPar(GetJinf(), GetTDR(), GetVA(aa), 1);
  // float par2=GainCorrectionPars::Instance()->GetPar(GetJinf(), GetTDR(), GetVA(aa), 2);//PER ADESSO CE NE SONO SOLO
  // 2!!!

  // float correctSignal = (Signal[aa]*par2*(1-par1)+par0*par2);
  float correctSignal = (par1 * Signal[aa] + par0);

  return correctSignal;
}

float Cluster::GetTotNoise() const {
  int se = GetSeed();
  int se_r = se + 1;
  int se_l = se - 1;
  float val_l = 0;
  float val_r = 0;
  if (se_l >= 0)
    val_l = Noise[se_l];
  else
    val_l = 0;
  if (se_r < length)
    val_r = Noise[se_r];
  else
    val_r = 0;
  if (val_l > val_r)
    return sqrt(val_l * val_l + Noise[se] * Noise[se]);
  else
    return sqrt(val_r * val_r + Noise[se] * Noise[se]);
}

float Cluster::GetSig() const {
  int se = GetSeed();
  int se_r = se + 1;
  int se_l = se - 1;
  float val_l = 0;
  float val_r = 0;
  if (se_l >= 0)
    val_l = GetCSignal(se_l);
  else
    val_l = 0;
  if (se_r < length)
    val_r = GetCSignal(se_r);
  else
    val_r = 0;
  if (val_l > val_r)
    return val_l + GetCSignal(se);
  else
    return val_r + GetCSignal(se);
}

float Cluster::GetTotSig() const {
  float val = 0;
  for (int ii = 0; ii < length; ii++) {
    val += GetCSignal(ii);
  }
  return val;
}

int Cluster::GetSec() const {
  int se = GetSeed();
  int se_r = se+1;
  int se_l = se-1;
  if (se==0)
    return 0; //if there is only one element in the cluster eg. seed

  float val_l = 0;
  float val_r = 0;

  if (se_l > 0)
    val_l = GetCSignal(se_l);
  if (se_r < length)
    val_r = GetCSignal(se_r);

  if (val_r > val_l && val_r / Noise[se_r] > 0)
    return se_r;
  if (val_l > val_r && val_l / Noise[se_l] > 0)
    return se_l;
  else
    return 0;
};

float Cluster::GetSecVal() const { return GetCSignal(GetSec()); }

float Cluster::GetEtaRaw() const {
  float ee;
  int se = GetSeed();
  int se_r = se + 1;
  int se_l = se - 1;
  float val_l = 0;
  float val_r = 0;
  float val_seed = GetCSignal(se);
  if (se_l >= 0)
    val_l = GetCSignal(se_l);
  if (se_r < length)
    val_r = GetCSignal(se_r);

  // eta definition: SR / (SR+SL)
  if (val_l > val_r && val_l / Noise[se_l] > 0) // the secondary is on the left
    ee = - (val_seed) / (val_l + val_seed); // negative etaraw indicate that secondary is on the left
  else if (val_r > val_l && val_r / Noise[se_r] > 0) // the secondary is on the right
    ee = (val_r) / (val_r + val_seed);// positive etaraw indicate that secondary is on the right
  else
    return -3;

  return ee;
}

float Cluster::GetEta() const { // remove the sign (i.e. information about the position of the secondary respect to seed) on the etaRaw
  float ee = GetEtaRaw();
  if (ee < -1 || ee > 1)
    return -3;
  else
    return fabs(ee);
}

float Cluster::GetCoG() const {
  int se = GetSeed();
  float ee = GetEtaRaw();
  if (ee < -1 || ee > 1)
    return address + se;
  else if (ee< 0)
    return address + se + fabs(ee) - 1; // starting of the cluster + position of the seed respect to the cluster + (EtaRaw+1)
  else
    return address + se + ee; // starting of the cluster + position of the seed respect to the cluster + EtaRaw
}

int Cluster::GetVA(int strip_address) {
  int vanum = 0;
  // Viviana: hardcoded number of channels per VA
  // MD: we need to put this under a #define, but however 256 is not possibile.
  // vanum=strip_address/64;
  vanum = strip_address / 256;

  return vanum;
}

double Cluster::GetPitch(int side) const { return GetPitch(GetJinf(), GetTDR(), side); };

double Cluster::GetNominalResolution(int side) const { return GetNominalResolution(GetJinf(), GetTDR(), side); };

double Cluster::GetPitch(int jinfnum, int tdrnum, int side) {
  //  printf("Called Cluster::GetPitch(%d, %d, %d) = %lf\n", jinfnum, tdrnum, side, LadderConf::Instance()->GetPitch(jinfnum, tdrnum, side));

  return LadderConf::Instance()->GetPitch(jinfnum, tdrnum, side);
};

double Cluster::GetNominalResolution(int jinfnum, int tdrnum, int side) {
  // printf("Called Cluster::GetNominalResolution(%d, %d, %d) = %lf\n", jinfnum, tdrnum, side,
  // LadderConf::Instance()->GetResolution(jinfnum, tdrnum, side));
  return LadderConf::Instance()->GetResolution(jinfnum, tdrnum, side);
};

void Cluster::ApplyVAEqualization() {

  int jinfnum = GetJinf();
  int tdrnum = GetTDR();
  for (int ii = 0; ii < length; ii++) {
    int vanum = GetVA(address + ii);
    SignalVAEqualized[ii] = Signal[ii] * GainCorrectionPars::Instance()->GetPar(jinfnum, tdrnum, vanum, 0) *
                            GainCorrectionPars::Instance()->GetPar(jinfnum, tdrnum, vanum, 1);
  }
  return;
}

// everything I'm making here is based on the cog but should have been done on the single strip address and only then
// the cog should have been performed
double Cluster::GetPosition(int mult) const {

  float cog = GetCoG();
  float cog2 = cog;

  double mult_shift = 0.0;
  float pitchcorr = 0.0;

  if (side == 0) { // S
    // The gaps between 0 and 1 and 638 to 639 are doubled. MD 17/Sep/2022: most likely true only for AMS-02 sensors...
    if (cog2 >= 0.5)
      pitchcorr += 1.0;
    //    if (cog2 >= 638.5)
    if (cog2 >= GetNChannels(0)-1.5)
      pitchcorr += 1.0;
    if (LadderConf::Instance()->GetStripMirroring(GetJinf(), GetTDR(), side)) {
      //      cog2 = 639 - cog2; // If the ladder is mirrored, reverse position
      cog2 = (GetNChannels(0)-1) - cog2; // If the ladder is mirrored, reverse position
    }
  } else {                   // K (K5; K7 is not implemented)
    cog2 -= GetNChannels(0); // N channels of S --> cog in [0, 383]
    int sensor = (int)((cog2 + mult * GetNChannels(1)) /
                       GetReadChannelK()); // cast to int but essentially is also used as 'floor'
    if (LadderConf::Instance()->GetBondingType(GetJinf(), GetTDR()) == 3) {
      sensor = 0;
    }
    bool multflip = LadderConf::Instance()->GetMultiplicityFlip(GetJinf(), GetTDR());
    // printf("VF: multflip = %d\n", multflip);
    if (multflip && sensor % 2) { // if sensor is odd (DISPARO)
      sensor -= 2;                // move 'back' of two sensors...
    }
    mult_shift = GetSensPitchK() * sensor;
    if (LadderConf::Instance()->GetBondingType(GetJinf(), GetTDR()) != 3) {
      //      if (cog2 > 191.5)
      if (cog2 > GetNChannels(1)/2-0.5)
	//        cog2 -= 192.0; //--> cog in [0, 191]
	cog2 -= GetNChannels(1);
      //      if (cog2 > 190.5)
      if (cog2 > GetNChannels(1)/2-1.5)
        pitchcorr = 0.5; // last strip of the sensor is half pitch more far. MD 17/Sep/2022: most likely true only for AMS-02 sensors...
    }
    if (LadderConf::Instance()->GetStripMirroring(GetJinf(), GetTDR(), side)) {
      //      cog2 = 383 - cog2; // If the ladder is mirrored, reverse position
      cog2 = (GetNChannels(1)-1) - cog2; // If the ladder is mirrored, reverse position
    }
  }

  //  printf("%f %f %f %d %f %f\n", GetCoG(), cog2, pitchcorr, side, GetPitch(side), mult_shift);

  return (cog2 + pitchcorr) * GetPitch(side) + mult_shift;
}

double Cluster::GetAlignedPosition(int mult) const {
  double align_shift = AlignmentPars::Instance()->GetPar(GetJinf(), GetTDR(), side);
  return GetPosition(mult) - align_shift;
}

// MD: must be a general function, cannot be divided for data and MC
// tried otherwise S K pitch and hardcoded things must be changed everywhere
double Cluster::GetAlignedPositionMC() const {
  double align_shift = AlignmentPars::Instance()->GetPar(GetJinf(), GetTDR(), side);
  float cog = GetCoG();
  double mult_shift = 0.0;
  // float pitchcorr = (side?177:224); // just for fun
  float pitchcorr = 112.0; // 0.110; // just for fun
  //  std::cout<<"***** COG "<<cog<<" PIT "<<  GetPitch(side)<<" ALI "<<align_shift<<"
  //  -->"<<(cog+pitchcorr)*GetPitch(side)+mult_shift-align_shift<<std::endl;

  return (cog + pitchcorr) * GetPitch(side) + mult_shift - align_shift;
}

double Cluster::GetZPosition() const { return AlignmentPars::Instance()->GetPar(GetJinf(), GetTDR(), 2); }

float Cluster::GetSeedVal() const { return GetCSignal(GetSeed()); }

float Cluster::GetSeedSN() const { return GetCSignal(GetSeed()) / Noise[GetSeed()]; }

float Cluster::GetTotSN() const { return GetTotSig() / GetTotNoise(); }

int Cluster::GetLength() const { return length; }

// length above a passed threshold
int Cluster::GetLength(float val) const {
  int se = GetSeed();
  int myle = 1;
  for (int ii = se - 1; ii >= 0; ii--)
    if (GetCSignal(ii) / Noise[ii] > val)
      myle++;
    else
      break;
  for (int ii = se + 1; ii < length; ii++)
    if (GetCSignal(ii) / Noise[ii] > val)
      myle++;
    else
      break;
  return myle;
}

std::vector <float> Cluster::Sort() {
  std::vector<float> v;
  int temp;
  for (int i=0; i<length; i++)
    v.push_back(Signal[i]);

  std::sort(begin(v), end(v), std::greater<float>{});
  return v;

}

void Cluster::Print() {

  printf(" ladd: %d  side: %d add: %d len: %d bad: %d \n", ladder, side, address, length, bad);
  printf(" Seed: %d SeedAddress: %d Seed:val %f SeedSN: %f \n", GetSeed(), GetSeedAdd(), GetSeedVal(), GetSeedSN());
  printf(" Strip:       Signal:     Noise:       Status: \n");
  for (int ii = 0; ii < length; ii++) {
    printf("    %d      %f       %f       %d \n", ii, GetCSignal(ii), Noise[ii], Status[ii]);
  }

  return;
}

int Cluster::GoldRegion() {
  int lowgold[6][2] = {{280, 720}, {50, 650}, {150, 660}, {160, 660}, {150, 860}, {220, 660}};
  int upgold[6][2] = {{340, 830}, {150, 850}, {400, 710}, {460, 710}, {450, 920}, {480, 710}};

  if (GetCoG() > lowgold[ladder][side] && GetCoG() <= upgold[ladder][side])
    return 1;
  else
    return 0;
}

bool Cluster::BorderRegion() {
  int width = 5;
  bool onborder = false;
  int border = 64;

  while (!onborder && border < 1024) {
    if (GetCoG() > border - width && GetCoG() < border + width) {
      onborder = true;
    }
    border += 64;
  }
  return onborder;
}
