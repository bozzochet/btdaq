
#ifndef InteractionPanel_h
#define InteractionPanel_h

#include <TGWindow.h>
#include <TGFrame.h>
#include "ButtonPanel.h"
#include "GenericSlider.h"

class InteractionPanel: public TGGroupFrame {
public:
	ButtonPanel *bp1,*bp2;
	Int_t crate,tdr;
	GenericSlider *ref;

	InteractionPanel(TGWindow *p,char *name,GenericSlider *gs);
	void updateCrate(Int_t id);
	void updateTDR(Int_t id);
	void Update();

	ClassDef(InteractionPanel,0)
};

#endif

