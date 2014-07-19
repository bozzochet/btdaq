#ifndef QDIAG_H 
#define QDIAG_H



#include "QList.h"



class QDiag {

 private:
  QEvent *fQref;
  FILE *fLogFile;

 public:

  QDiag::QDiag();
  QDiag::QDiag(QEvent *qref);
  QDiag::~QDiag();

  void SetQRef(QEvent *qref);
  void Clear();
  int Compare(QEvent *qevent);
  void DiagTBS(QData *qdataref, QData *qdata);
  void DiagTPSFE(QData *qdataref, QData *qdata);
  void DiagS9011AT(QData *qdataref, QData *qdata);
  void SetLogFile(FILE *file);
};





#endif // QDIAG_H
