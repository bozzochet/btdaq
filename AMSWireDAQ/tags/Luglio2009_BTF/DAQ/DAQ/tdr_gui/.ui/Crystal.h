/****************************************************************************
** Form interface generated from reading ui file 'Crystal.ui'
**
** Created: Sat Aug 25 17:50:26 2007
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
class QLabel;
class QSpinBox;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QTextBrowser;

class CrystalGUI : public QMainWindow
{
    Q_OBJECT

public:
    CrystalGUI( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~CrystalGUI();

    QFrame* frame7;
    QGroupBox* gbGon;
    QLCDNumber* LCDGon3;
    QLCDNumber* LCDGon4;
    QLabel* tlb1_4;
    QLabel* tlb1;
    QSpinBox* sbLin2;
    QSpinBox* sbAlpha;
    QSpinBox* sbLin1;
    QLabel* tlb1_3;
    QSpinBox* sbCrad;
    QLCDNumber* LCDGon1;
    QLCDNumber* LCDGon2;
    QLabel* tlb1_2;
    QPushButton* pbGonGet;
    QPushButton* pbGonSet;
    QButtonGroup* buttonGroup3;
    QRadioButton* rbAbs;
    QRadioButton* rbRel;
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
    QGroupBox* gbDAQPG0;
    QTextBrowser* tbPG0Log;
    QPushButton* pbPG0GetLog;
    QPushButton* pbPG0Clear;
    QPushButton* pbPG0NewLog;
    QPushButton* pbPG0Kill;
    QPushButton* pbPG0Init;
    QPushButton* pbPG0Start;
    QPushButton* pbPG0Stop;
    QGroupBox* gbMain;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QToolBar *toolBar;
    QAction* fileNewAction;
    QAction* fileOpenAction;
    QAction* fileSaveAction;
    QAction* fileSaveAsAction;
    QAction* filePrintAction;
    QAction* fileExitAction;
    QAction* editUndoAction;
    QAction* editRedoAction;
    QAction* editCutAction;
    QAction* editCopyAction;
    QAction* editPasteAction;
    QAction* editFindAction;

public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editFind();
    virtual void CrystalGUI_dockWindowPositionChanged( QDockWindow * );
    virtual void CrystalGUI_usesTextLabelChanged( bool );

protected:
    QVBoxLayout* gbGonLayout;
    QHBoxLayout* layout38;
    QGridLayout* layout37;
    QHBoxLayout* layout39;
    QHBoxLayout* buttonGroup3Layout;
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
    QHBoxLayout* gbDAQPG0Layout;
    QVBoxLayout* layout62;
    QHBoxLayout* layout61;
    QVBoxLayout* layout63;
    QSpacerItem* spacer11;

protected slots:
    virtual void languageChange();

};

#endif // CRYSTALGUI_H
