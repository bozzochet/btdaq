
#ifndef SliderI_h
#define SliderI_h

//#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>

//class SliderI: public TRootEmbeddedCanvas {
class SliderI: public TCanvas {
public:
	virtual void setRootFile(char *filename) = 0;
	virtual void setHistFile(char *filename) = 0;
	virtual void selectGraph(int id) = 0;
	virtual void showNext() = 0;
	virtual void showPrev() = 0;
	virtual int showTkId(int tkid) = 0;
	virtual int showHwId(int hwid) = 0;
	virtual int GetHwId() = 0;
	virtual int GetTkId() = 0;
	virtual void PrintThis() = 0;
	virtual void PrintAll() = 0;

	SliderI(char *name, char *title,UInt_t w,UInt_t h)
		:TCanvas(name,title,w,h){};

	ClassDef(SliderI,0);
};

#endif

