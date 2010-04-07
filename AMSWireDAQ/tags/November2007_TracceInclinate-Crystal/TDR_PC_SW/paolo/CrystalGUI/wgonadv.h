/****************************************************************************
** Form interface generated from reading ui file 'wgonadv.ui'
**
** Created: Wed Oct 24 14:29:45 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef WGONADV_H
#define WGONADV_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLineEdit;

class wGonAdv : public QWidget
{
    Q_OBJECT

public:
    wGonAdv( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~wGonAdv();

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
    QVBoxLayout* wGonAdvLayout;

protected slots:
    virtual void languageChange();

};

#endif // WGONADV_H
