/****************************************************************************
** Form interface generated from reading ui file 'clrdisplay.ui'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CLRDISPLAY_H
#define CLRDISPLAY_H

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
class QSpinBox;
class QPushButton;
class QTextEdit;
class QFrame;

class ClRDisplay : public QWidget
{
    Q_OBJECT

public:
    ClRDisplay( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~ClRDisplay();

    QGroupBox* groupBox2;
    QSpinBox* sbRClS;
    QPushButton* pbRDrawS;
    QTextEdit* teRSOUT;
    QFrame* frame7;
    TQRootCanvas* CanvasRS;

    AMSTrRawCluster* cls;
    TH1F* Ssig;
    TH1F* Snoise;
    TH1F* Sstat;

    virtual void init();

public slots:
    virtual void UpdateS();

protected:
    QVBoxLayout* ClRDisplayLayout;
    QVBoxLayout* groupBox2Layout;
    QHBoxLayout* layout17;
    QSpacerItem* spacer1;
    QVBoxLayout* frame7Layout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

};

#endif // CLRDISPLAY_H
