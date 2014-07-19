
#include <TGFrame.h>
#include "GenericSlider.h"
#include "InteractionPanel.h"
#include "TrackerPanel.h"
#include "SearchPanel.h"

class CalPainterUI: public TGMainFrame{
public:
	TGMainFrame* fMain;
	GenericSlider *gs;
	InteractionPanel *intpan;
	SearchPanel *search;
	TrackerPanel *track;

	CalPainterUI(const TGWindow *p,UInt_t w,UInt_t h,char *filename);
	~CalPainterUI();
	void HandleMenu(Int_t id);
	void CloseWindow();

	ClassDef(CalPainterUI,0);
};

