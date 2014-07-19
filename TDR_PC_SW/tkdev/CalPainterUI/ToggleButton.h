#ifndef ToggleButton_h
#define ToggleButton_h

#include <TGButton.h>
#include <TGWindow.h>
#include <TGDimension.h>

class ToggleButton: public TGTextButton{
public:
	EButtonState       fPrevState;     // previous check button state
	ToggleButton(TGWindow *p,char *t);
	Bool_t IsToggleButton() const {return kTRUE;}
	Bool_t IsExclusiveToggle() const {return kTRUE;}
	void SetState(EButtonState state, Bool_t emit);
	void EmitSignals(Bool_t wasUp=kTRUE);
	/* BASTARD!! BASTARD!! BASTARD!! 
	   default behaviour is to return fWidth+8, fHeight+7
	*/
	TGDimension GetDefaultSize() const{
		return TGDimension(fWidth,fHeight);
	}
	Bool_t HandleButton(Event_t *event);
	ClassDef(ToggleButton,0);
};

#endif

