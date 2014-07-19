
#ifndef ButtonPanel_h
#define ButtonPanel_h

#include <TGWindow.h>
#include <TGButtonGroup.h>
#include "ToggleButton.h"

#define MAXBUTTONS	100	// maximum matrix size 10x10

class ButtonPanel: public TGGroupFrame {
public:
	/* TGButtonGroup is child of TGGroupFrame, so i could subclass
	 * directly it. Anyway, as TGMatrixLayout does NOT use TGLayoutHints,
	 * is better to have an extra frame so everything is tidy.
	 */
	TGButtonGroup *bg;
	ToggleButton *buttons[MAXBUTTONS];
	int nbuttons;

	ButtonPanel(TGWindow *p,char *name,int rows, int columns,char **text);
	Bool_t Connect(const char* signal, const char* receiver_class, 
		void* receiver, const char* slot){
			return bg->Connect(signal,receiver_class,receiver,slot);
	}
	ClassDef(ButtonPanel,0);
};

#endif

