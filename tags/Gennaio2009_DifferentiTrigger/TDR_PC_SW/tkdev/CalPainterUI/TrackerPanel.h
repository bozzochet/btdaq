
#ifndef TrackerPanel_h
#define TrackerPanel_h

#include <TGWindow.h>
#include <TGFrame.h>
#include "ButtonPanel.h"
#include "GenericSlider.h"

class TrackerPanel: public TGGroupFrame {
public:
	ButtonPanel *bp1,*bp2;
	Int_t layer,slot;
	GenericSlider *ref;

	TrackerPanel(TGWindow *p,char *name,GenericSlider *gs);
	void updateLayer(Int_t id);
	void updateSlot(Int_t id);
	void Update();

	ClassDef(TrackerPanel,0)
};

#endif

