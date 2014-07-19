/****************************************************************************
** Form interface generated from reading ui file 'Crystal.ui'
**
** Created: Sun Aug 26 20:05:56 2007
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
class QFrame;
class QGroupBox;
class QLCDNumber;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QLabel;
class QLineEdit;

class CrystalGUI : public QMainWindow
{
    Q_OBJECT

public:
    CrystalGUI( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~CrystalGUI();

    QFrame* frame7;
    QGroupBox* gbMain;
    QGroupBox* gbTrig;
    QLCDNumber* lCDNumber9;
    QPushButton* pbTrigStart;
    QPushButton* pbTrigStop;
    QPushButton* pbTrigReset;
    QButtonGroup* buttonGroup5;
    QRadioButton* rbTrigEx;
    QRadioButton* rbTrigIn;
    QButtonGroup* buttonGroup7;
    QRadioButton* rbTrigSpillIn;
    QRadioButton* rbTrigSpillOut;
    QPushButton* pbTrigGet;
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
    QGridLayout* CrystalGUILayout;
    QHBoxLayout* gbTrigLayout;
    QVBoxLayout* layout59;
    QSpacerItem* spacer10;
    QSpacerItem* spacer9;
    QHBoxLayout* layout57;
    QSpacerItem* spacer6;
    QVBoxLayout* layout51;
    QHBoxLayout* buttonGroup5Layout;
    QHBoxLayout* buttonGroup7Layout;
    QHBoxLayout* layout50;
    QSpacerItem* spacer5;
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

protected slots:
    virtual void languageChange();

};

#endif // CRYSTALGUI_H
