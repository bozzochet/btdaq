
#include <TString.h>
#include <TGLayout.h>
#include "ButtonPanel.h"

ButtonPanel::ButtonPanel(TGWindow *p,char *name,int rows, int columns,
		char **text):TGGroupFrame(p,name){

	nbuttons = rows * columns;
	bg = new TGButtonGroup(this,rows,columns);
	bg->SetExclusive();

	for(int i=0;i<MAXBUTTONS;i++)
		buttons[i] = NULL;

	for(int i=0;i<nbuttons;i++){
		buttons[i] = new ToggleButton(bg,text[i]);
		//if(rows!=1) buttons[i]->Resize(22,26);
		if(rows!=1)
			buttons[i]->Resize(25,16);
		else
			buttons[i]->Resize(15,16);
	}
	//buttons[0]->SetState(kButtonDown,kTRUE);
	AddFrame(bg,new TGLayoutHints(kLHintsExpandX));
	bg->Show();
}

ClassImp(ButtonPanel);

