/****************************************************************************
** Form interface generated from reading ui file 'cldisplay.ui'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CLDISPLAY_H
#define CLDISPLAY_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>
#include "TH1F.h"
#include "Event.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class TQRootCanvas;
class QGroupBox;
class QLabel;
class QSpinBox;
class QRadioButton;
class QPushButton;
class QTextEdit;
class QFrame;

class ClDisplay : public QWidget
{
    Q_OBJECT

public:
    ClDisplay( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ClDisplay();

    QGroupBox* groupBox2;
    QLabel* textLabel1;
    QSpinBox* sbClS1;
    QLabel* textLabel2_2;
    QSpinBox* sbClS2;
    QLabel* textLabel2;
    QSpinBox* sbClS3;
    QRadioButton* rbNside;
    QRadioButton* rbPside;
    QSpinBox* sbClS4;
    QPushButton* pbDrawS;
    QTextEdit* teSOUT;
    QFrame* frame7_2;
    TQRootCanvas* CanvasS;

    TH1F* Sstat;
    TH1F* Snoise;
    TH1F* Ssig;
    AMSTrCluster* cls;

public slots:
    virtual void init();
    virtual void UpdateS();
    virtual void UpdateEvent();
    virtual void UpdateDigits();
    virtual void UpdateLayer();
    virtual void UpdateTkIdRange();
    virtual void UpdateTkId();
    virtual void UpdateNP();
    virtual void UpdateIclsRange();
    virtual void UpdateIdx();
    virtual void Exit();
    virtual void rbPside_toggled( bool sw );
    virtual void rbNside_toggled( bool sw );

protected:
    QVBoxLayout* groupBox2Layout;
    QHBoxLayout* layout6;
    QVBoxLayout* frame7_2Layout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // CLDISPLAY_H
