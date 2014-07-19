
#include <TGLabel.h>
#include "SearchPanel.h"

ClassImp(SearchPanel);

SearchPanel::SearchPanel(TGWindow *p, char *name, GenericSlider *gs)
	:TGGroupFrame(p,name){
	
	ref = gs;
	TGHorizontalFrame *l1 = new TGHorizontalFrame(this);
	TGLabel *lab1 = new TGLabel(l1,"HwId");
	l1->AddFrame(lab1);
	te1 = new TGNumberEntryField(l1,0,0,
		TGNumberFormat::kNESInteger,TGNumberFormat::kNEANonNegative);
	te1->Resize(100,26);
	te1->Connect("ReturnPressed()","SearchPanel",this,"showHwId()");
	l1->AddFrame(te1);
	TGTextButton *bu1 = new TGTextButton(l1,"Search");
	bu1->Connect("Clicked()","SearchPanel",this,"showHwId()");
	l1->AddFrame(bu1);
	AddFrame(l1,new TGLayoutHints(kLHintsRight));

	TGHorizontalFrame *l2 = new TGHorizontalFrame(this);
	TGLabel *lab2 = new TGLabel(l2,"TkId");
	l2->AddFrame(lab2);
	te2 = new TGNumberEntryField(l2,0,0,
		TGNumberFormat::kNESInteger,TGNumberFormat::kNEAAnyNumber);
	te2->Resize(100,26);
	te2->Connect("ReturnPressed()","SearchPanel",this,"showHwId()");
	l2->AddFrame(te2);
	TGTextButton *bu2 = new TGTextButton(l2,"Search");
	bu2->Connect("Clicked()","SearchPanel",this,"showTkId()");
	l2->AddFrame(bu2);
	AddFrame(l2,new TGLayoutHints(kLHintsRight));
}

void SearchPanel::showTkId(){
	ref->showTkId(te2->GetIntNumber());
}

void SearchPanel::showHwId(){
	ref->showHwId(te1->GetIntNumber());
}

