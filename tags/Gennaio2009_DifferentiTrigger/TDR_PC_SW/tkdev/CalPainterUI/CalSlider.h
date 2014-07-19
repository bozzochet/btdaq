#ifndef CalSlider_h
#define CalSlider_h

#include <TH1F.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TPaveLabel.h>

#include <TkDBc.h>
#include <TrCalDB.h>
#include <TrLadCal.h>
#include "SliderI.h"

class CalSlider: public SliderI {
private:
	static int Opened;
	static char* filename;
	TH1F* Ped;
	TH1F* Sig;
	TH1F* SigR;
	TH1F* SigU;
	TH1F* SigRU;
	TH1F* SigL;
	TH1F* SigRL;

	TH1F* SigR2;
	TH1F* SigU2;
	TH1F* SigRU2;
	TH1F* SigL2;
	TH1F* SigRL2;

	TH1F* ST0;
	TH1F* ST1;
	TH1F* ST2;
	TH1F* ST3;
	TH1F* ST4;
	TH1F* NG;
	TrLadCal* cal;
	TkLadder* lad;
	void SetHistStyle(TH1* hh);
public:
	TPaveLabel *label;
	TFile *rootfile,*histfile;
	TrCalDB *caldb;
	trcalIT calit_begin,calit_end,calit_cur;
	int graphtype;

	CalSlider(char *name,char *title,int xsize=1064, int ysize=600);
	~CalSlider();

	int try2Draw(TrLadCal *cc);

	/* To implement interface */
	void showNext();
	void showPrev();
	void setRootFile(char* filename);
	void setHistFile(char* filename);
	void selectGraph(int id);
	int showTkId(int tkid);
	int showHwId(int hwid);
	int GetHwId();
	int GetTkId();
	void PrintThis();
	void PrintAll();

	/* CalPainter methods */
	void Draw1(const Option_t *aa,int flag);
	void Draw2(const Option_t *aa,int flag);
	void Draw3(const Option_t *aa,int flag);
	void Draw4(const Option_t *aa,int flag);
	void Draw5(const Option_t *aa,int flag);

	void Draw(const Option_t* aa,int flag);
	void Draw(const Option_t* aa=""){Draw(aa,0);}
	void Draw(int flag){Draw("",flag);}
	void Clear(const Option_t* aa="");
	void OpenFile();
	static void CloseFile();
	static void SetFileName(char* name){filename=name; }

	ClassDef(CalSlider,0);
};

#endif

