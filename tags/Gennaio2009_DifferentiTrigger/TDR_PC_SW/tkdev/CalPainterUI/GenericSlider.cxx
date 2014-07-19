
#include "GenericSlider.h"

ClassImp(GenericSlider);

GenericSlider::GenericSlider(TGWindow *p)
		:TGVerticalFrame(p){
	slider = NULL;

	/* Graphics stuff */
	frame = new TGHorizontalFrame(this);

	prev = new TGTextButton(frame,"&Prev");
	prev->Connect("Clicked()","GenericSlider",this,"showPrev()");
	frame->AddFrame(prev,new TGLayoutHints(kLHintsLeft | kLHintsTop,
		2,2,2,2));
	
	next = new TGTextButton(frame,"&Next");
	next->Connect("Clicked()","GenericSlider",this,"showNext()");
	frame->AddFrame(next,new TGLayoutHints(kLHintsLeft | kLHintsTop,
		2,2,2,2));

	graph = new TGComboBox(frame,10);
	graph->AddEntry("Calibration",1);
	graph->AddEntry("Graph2",2);
	graph->AddEntry("Graph3",3);
	graph->AddEntry("Graph4",4);
	graph->AddEntry("Graph5",5);
	graph->Select(1);
	graph->Resize(100,20);
	graph->Connect("Selected(Int_t,Int_t)","GenericSlider",this,
		"selectGraph(Int_t,Int_t)");
	frame->AddFrame(graph,new TGLayoutHints(kLHintsRight | kLHintsTop,
		2,2,2,2));
	
	AddFrame(frame,new TGLayoutHints(kLHintsTop | kLHintsExpandX));
	info = new TGLabel(this);
	AddFrame(info,new TGLayoutHints(kLHintsTop | kLHintsExpandX));

}

void GenericSlider::showHwId(int hwid){
	if(!slider) return;
	int ret = slider->showHwId(hwid);
	switch(ret){
		case 0:
			info->SetText("");
			Updated();
			break;
		case 1:
			info->SetText("HwId NOT FOUND.");
			break;
		case 2:
			info->SetText("Open a file before.");
			break;
	}
	Layout();
}

void GenericSlider::showTkId(int tkid){
	if(!slider) return;
	int ret = slider->showTkId(tkid);
	switch(ret){
		case 0:
			Updated();
			info->SetText("");
			break;
		case 1:
			info->SetText("TkId NOT FOUND.");
			break;
		case 2:
			info->SetText("Open a file before.");
			break;
	}
	Layout();
}

void GenericSlider::PrintThis(){
	if(!slider) return;
	info->SetText("This canvas has been printed.");
	Layout();
	slider->PrintThis();
}

void GenericSlider::PrintAll(){
	if(!slider) return;
	info->SetText("All canvas have been printed.");
	Layout();
	slider->PrintAll();
}

void GenericSlider::showNext(){
	if(!slider) return;
	slider->showNext();
	info->SetText("");
	Layout();
	Updated();
}

void GenericSlider::showPrev(){
	if(!slider) return;
	slider->showPrev();
	info->SetText("");
	Layout();
	Updated();
}

void GenericSlider::setRootFile(char *filename){
	slider = new CalSlider("Ecanvas","CalPainterUI Canvas");
	slider->ToggleEventStatus();
	slider->setRootFile(filename);
	info->SetText("");
	Layout();
	Updated();
}

void GenericSlider::setHistFile(char *filename){
	if(!slider){
		info->SetText("Select a Cal file before.");
		Layout();
		return;
	}
	slider->setHistFile(filename);
}

void GenericSlider::Updated(){
	Emit("Updated()");
}

void GenericSlider::selectGraph(Int_t w, Int_t id){
	if(!slider) return;
	slider->selectGraph(id);
	slider->Draw();
}

