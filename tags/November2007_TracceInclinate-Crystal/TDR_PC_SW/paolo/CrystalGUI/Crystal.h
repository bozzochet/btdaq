/****************************************************************************
** Form interface generated from reading ui file 'Crystal.ui'
**
** Created: Wed Oct 24 14:29:45 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CRYSTALGUI_H
#define CRYSTALGUI_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QSplitter;
class QFrame;
class QGroupBox;
class QLabel;
class QLCDNumber;
class QLineEdit;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QTextEdit;

class CrystalGUI : public QMainWindow
{
    Q_OBJECT

public:
    CrystalGUI( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~CrystalGUI();

    QSplitter* splitter1;
    QFrame* frame3;
    QGroupBox* gbGon;
    QLabel* tlb1;
    QLabel* tlb1_2;
    QLabel* tlb1_3;
    QLabel* tlb1_4;
    QLCDNumber* LCDGon1;
    QLCDNumber* LCDGon4;
    QLCDNumber* LCDGon2;
    QLCDNumber* LCDGon3;
    QLineEdit* leGonAlpha;
    QLineEdit* leGonCrad;
    QLineEdit* leGonLin1;
    QLineEdit* leGonLin2;
    QPushButton* pbGonGet;
    QButtonGroup* buttonGroup4;
    QRadioButton* rbGonMurad;
    QRadioButton* rbGonRad;
    QRadioButton* rbGonGrad;
    QPushButton* pbGonAbort;
    QPushButton* pbGonSet;
    QButtonGroup* buttonGroup3;
    QRadioButton* rbAbs;
    QRadioButton* rbRel;
    QGroupBox* gbTrig;
    QLabel* tlTrigRun;
    QLCDNumber* lCDNumber9;
    QPushButton* pbTrigStart;
    QPushButton* pbTrigStop;
    QPushButton* pbTrigReset;
    QPushButton* pbTrigGet;
    QButtonGroup* buttonGroup6;
    QRadioButton* rbTrigType2;
    QRadioButton* rbTrigType1;
    QRadioButton* rbTrigType3;
    QRadioButton* rbTrigType4;
    QFrame* frameDaq;
    QGroupBox* gbMain;
    QPushButton* pbSng;
    QPushButton* pbScan;
    QTextEdit* txEMainLog;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *Tools;
    QAction* filePrintAction;
    QAction* fileExitAction;
    QAction* toolsGoniometer_AdvancedAction;

public slots:
    virtual void filePrint();
    virtual void fileExit();
    virtual void CrystalGUI_dockWindowPositionChanged( QDockWindow * );
    virtual void CrystalGUI_usesTextLabelChanged( bool );
    virtual void pbGonGet_clicked();
    virtual void Init();
    virtual void UpdateGonDisplay();
    virtual void SetGonPos();
    virtual void toolsGoniometer_AdvancedAction_activated();
    virtual void rbGonMurad_toggled( bool );
    virtual void rbGonRad_toggled( bool );
    virtual void rbGonGrad_toggled( bool );

protected:
    QHBoxLayout* frame3Layout;
    QHBoxLayout* gbGonLayout;
    QVBoxLayout* layout99;
    QHBoxLayout* layout96;
    QVBoxLayout* layout95;
    QHBoxLayout* layout92;
    QHBoxLayout* layout93;
    QVBoxLayout* layout14;
    QVBoxLayout* buttonGroup4Layout;
    QVBoxLayout* layout13;
    QSpacerItem* spacer6_2;
    QVBoxLayout* buttonGroup3Layout;
    QVBoxLayout* gbTrigLayout;
    QHBoxLayout* layout12;
    QHBoxLayout* layout13_2;
    QSpacerItem* spacer6;
    QGridLayout* buttonGroup6Layout;
    QHBoxLayout* frameDaqLayout;

protected slots:
    virtual void languageChange();

};

#endif // CRYSTALGUI_H
