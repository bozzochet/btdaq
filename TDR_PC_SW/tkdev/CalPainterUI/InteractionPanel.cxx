
#include "InteractionPanel.h"

ClassImp(InteractionPanel);

InteractionPanel::InteractionPanel(TGWindow *p, char *name,GenericSlider *gs)
	:TGGroupFrame(p,name){
	char *b1text[] = {"0","1","2","3","4","5","6","7"};
	char *b2text[] = { "0","1","2","3","4","5","6","7","8","9",
		"10","11","12","13","14","15","16","17","18","19",
		"20","21","22","23"
	};
	
	ref = gs;
	crate = 0;
	tdr = 0;
	
	bp1 = new ButtonPanel(this,"Crate",1,8,b1text);
	bp1->Connect("Clicked(Int_t)","InteractionPanel",this,
		"updateCrate(Int_t)");
	AddFrame(bp1,new TGLayoutHints(kLHintsExpandX));

	bp2 = new ButtonPanel(this,"TDR",4,6,b2text);
	bp2->Connect("Clicked(Int_t)","InteractionPanel",this,
		"updateTDR(Int_t)");
	AddFrame(bp2,new TGLayoutHints(kLHintsExpandX));

}

void InteractionPanel::updateCrate(Int_t id){
	crate = id-1;
	ref->showHwId(100*crate+tdr);
}

void InteractionPanel::updateTDR(Int_t id){
	tdr = id-1;
	ref->showHwId(100*crate+tdr);
}

void InteractionPanel::Update(){
	int hwid = ref->slider->GetHwId();
	tdr = hwid%100;
	crate = hwid/100;
	for(int i=0;i<8;i++){
		EButtonState s;
		if(i == crate)
			s = kButtonDown;
		else
			s = kButtonUp;
		bp1->buttons[i]->SetState(s,kFALSE);
	}
	for(int i=0;i<24;i++){
		EButtonState s;
		if(i == tdr)
			s = kButtonDown;
		else
			s = kButtonUp;
		bp2->buttons[i]->SetState(s,kFALSE);
	}
}

