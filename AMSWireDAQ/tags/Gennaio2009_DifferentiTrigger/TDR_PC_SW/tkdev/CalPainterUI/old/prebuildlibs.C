{
	gSystem->Load("../AMSdev/tkdev/lib/libTracker");
	gSystem->AddIncludePath("-I../AMSdev/tkdev/TrackerLib/include/");
	gROOT->ProcessLine(".L ../AMSdev/tkdev/Scripts/CalPainter.cxx+");

}
