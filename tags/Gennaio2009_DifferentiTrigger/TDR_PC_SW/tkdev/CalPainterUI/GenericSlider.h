#ifndef GenericSlider_h
#define GenericSlider_h

#include <RQ_OBJECT.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include "CalSlider.h"

class GenericSlider: public TGVerticalFrame {
RQ_OBJECT("GenericSlider")
public:
	TGTextButton *next,*prev;
	TGHorizontalFrame *frame;
	TGLabel *info;
	TGComboBox *graph;
	CalSlider *slider;

	GenericSlider(TGWindow *p);
	void Updated(); //*SIGNAL*
	void selectGraph(Int_t w,Int_t id);
	void setRootFile(char *filename);
	void setHistFile(char *filename);
	void showTkId(int tkid);
	void showHwId(int hwid);
	void showNext();
	void showPrev();
	void PrintThis();
	void PrintAll();

	ClassDef(GenericSlider,0);
};

#endif

