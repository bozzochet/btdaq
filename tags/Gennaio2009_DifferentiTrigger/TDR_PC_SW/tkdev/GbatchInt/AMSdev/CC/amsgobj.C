//  $Id: amsgobj.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "node.h"
#include "snode.h"
#include "uzstat.h"
#include "amsgobj.h"
AMSNodeMap AMSgObj::GTrMatMap;
AMSNodeMap AMSgObj::GTrMedMap;
//AMSNodeMap AMSgObj::GVolMap;
AMSStat     AMSgObj::BookTimer;
AMSStatErr  AMSgObj::Messages;
