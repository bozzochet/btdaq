#define max_bins 5000

class myHisto {

 public:
  myHisto();
  ~myHisto();
  void Update(float value);
  float GetBin(int bin);
  float *GetHisto();

 private:
  int nbin;
  float min;
  float max;
  float histo[max_bins];
};
