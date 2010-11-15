#ifndef TRACKERLADDERS_H 
#define TRACKERLADDERS_H


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXLAD 192


class TkLadder {

 public:

  TkLadder();
  TkLadder(int x,int layer,int slot, char *name,int crate,int pwg,int pwgpos,int tdr,double lc80,double lc90);
  TkLadder(const TkLadder& ladder);
  //virtual ~Ladder();

  int GetX() { return fX; }
  int GetLayer() { return fLayer; }
  int GetSlot() { return fSlot; }
  const char *GetName() { return fName; }
  const char *GetLowercaseName() { return fNameLow; }
  int GetCrate() { return fCrate; }
  int GetPwg() { return fPwg; }
  int GetPwgpos() { return fPwgpos; }
  int GetTdr() { return fTdr; }
  double GetLc80() { return fLc80; }
  double GetLc90() { return fLc90; }

  //  void SetLadder(trackerlad ladder);
  void SetLadder(TkLadder Tkladder);
  void SetLadder(int x,int layer,int slot, char *name,int crate,int pwg,int pwgpos,int tdr,double lc80,double lc90);

 private:
  int fX;         // "p"=1, "m"=-1
  int fLayer;
  int fSlot;
  char fName[20], fNameLow[20];
  int fCrate;    // crate, starts at 0
  int fPwg;      // power group start at 0
  int fPwgpos;   // from 0 to 5
  int fTdr;      // from 0 to 23, redundant info, as it can be found from pwg and pwgpos.
  double fLc80;
  double fLc90;


};


class TrackerLadders {

 private:
  TkLadder *fLadder[MAXLAD+1];
  int fNlads;

 public:

  TrackerLadders();
  ~TrackerLadders();

  TkLadder* GetLadder(int lad) { return fLadder[lad]; }
  int FindLadderIndex(int crate, int pwg, int pwgpos);
  int FindLadderIndex(char *name);
  int FindLadderIndex(int crate, int tdr);
  TkLadder* FindLadder(int crate, int pwg, int pwgpos);
  TkLadder* FindLadder(char *name);
  TkLadder* FindLadder(int crate, int tdr);
  void SetTrackerLadder(int lad, TkLadder ladder);
  void ReadFile(const char *filename);
  int GetNlads() { return fNlads; }
  int CheckTdrAssignment(int lad);
  const char * GetLadderName(int lad) { return fLadder[lad]->GetName(); }
  const char * GetLadderLowercaseName(int lad) { return fLadder[lad]->GetLowercaseName(); }
};


#endif /* TRACKERLADDERS_H */
