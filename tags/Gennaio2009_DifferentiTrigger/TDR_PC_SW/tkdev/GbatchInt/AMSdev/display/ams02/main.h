//  $Id: main.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include <TApplication.h>
class Myapp : public TApplication{

public:
 void HandleIdleTimer();
 Myapp(const char *appClassName,int *argc, char **argv):
 TApplication(appClassName,argc, argv){};

};
