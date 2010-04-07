/****************************************************************************
** Form interface generated from reading ui file 'gonioadv.ui'
**
** Created: Sun Aug 26 19:16:35 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef GONIOADV_H
#define GONIOADV_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLineEdit;

class GonioAdv : public QDialog
{
    Q_OBJECT

public:
    GonioAdv( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~GonioAdv();

    QPushButton* pbGonSetPos;
    QPushButton* pbGonHwCal;
    QPushButton* pbGonSwCal;
    QPushButton* pbGonZero;
    QPushButton* pbGonAbort;
    QPushButton* pbGonSendRaw;
    QLineEdit* leGonRawCom;

public slots:
    virtual void pbGonSetPos_clicked();
    virtual void pbGonHwCal_clicked();
    virtual void pbGonZero_clicked();
    virtual void pbGonAbort_clicked();
    virtual void pbGonSendRaw_clicked();
    virtual void pbGonSwCal_clicked();

protected:
    QVBoxLayout* GonioAdvLayout;

protected slots:
    virtual void languageChange();

};

#endif // GONIOADV_H
