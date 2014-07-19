
#ifndef SearchPanel_h
#define SearchPanel_h

#include <TGWindow.h>
#include <TGFrame.h>
#include <TGNumberEntry.h>
#include "ButtonPanel.h"
#include "GenericSlider.h"

class SearchPanel: public TGGroupFrame {
public:
	GenericSlider *ref;
	TGNumberEntryField *te1,*te2;

	SearchPanel(TGWindow *p,char *name,GenericSlider *gs);
	void showHwId();
	void showTkId();


	ClassDef(SearchPanel,0)
};

#endif

