
#include "TrackerPanel.h"

ClassImp(TrackerPanel);

TrackerPanel::TrackerPanel(TGWindow *p, char *name,GenericSlider *gs)
	:TGGroupFrame(p,name){
	char *b1text[] = {"1","2","3","4","5","6","7","8"};
	char *b2text[] = { "15M","15P","14M","14P","13M","13P","12M","12P",
		"11M", "11P", "10M","10P","9M","9P","8M","8P","7M","7P","6M",
		"6P", "5M","5P","4M","4P","3M","3P","2M","2P","1M","1P"
	};
	ref = gs;
	layer = 1;
	slot = 1;
	
	bp1 = new ButtonPanel(this,"Layer",1,8,b1text);
	bp1->Connect("Clicked(Int_t)","TrackerPanel",this,
		"updateLayer(Int_t)");
	AddFrame(bp1,new TGLayoutHints(kLHintsExpandX));

	bp2 = new ButtonPanel(this,"Slot",15,2,b2text);
	bp2->bg->SetLayoutBroken();
	for(int i=0; i<30; i++){
		int s;
		float k = 0.033;
		if(i<16)
			s = (int)sqrt((i/2+1)*15/k);
		else
			s = (int)sqrt(((14-i/2)+1)*15/k);
		/* parabolic approx is too tight for first buttons */
		if(i==0||i==1||i==28||i==29) s += 5;
		if(i%2)
			bp2->buttons[i]->MoveResize(60,8*(i-1),s,15);
		else
			bp2->buttons[i]->MoveResize(60-s,8*i,s,15);
	}
	bp2->Connect("Clicked(Int_t)","TrackerPanel",this,
		"updateSlot(Int_t)");
	AddFrame(bp2,new TGLayoutHints(kLHintsExpandX));

}

void TrackerPanel::updateLayer(Int_t id){
	layer = id;
	ref->showTkId(abs(slot)/slot*(100*layer+abs(slot)));
}

void TrackerPanel::updateSlot(Int_t id){
	int id2slot[] = {-15,15,-14,14,-13,13,-12,12,-11,11,-10,10,-9,9,-8,8,-7,
		7,-6,6,-5,5,-4,4,-3,3,-2,2,-1,1
	};
	
	slot = id2slot[id-1];
	ref->showTkId(abs(slot)/slot*(100*layer+abs(slot)));
}

void TrackerPanel::Update(){
	int tkid = ref->slider->GetTkId();
	slot = tkid%100;
	layer = abs(slot)/slot*(tkid/100);
	for(int i=0;i<8;i++){
		EButtonState s;
		if(i == layer-1)
			s = kButtonDown;
		else
			s = kButtonUp;
		bp1->buttons[i]->SetState(s,kFALSE);
	}
	for(int i=0;i<30;i++){
		EButtonState s;
		int bn;
		if(slot>0)
			bn = 31-(2*slot);
		else
			bn = 30+(2*slot);
		if(i == bn)
			s = kButtonDown;
		else
			s = kButtonUp;
		bp2->buttons[i]->SetState(s,kFALSE);
	}
}
