/****************************************************************************
** Form interface generated from reading ui file 'mqdisplay.ui'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MQDISPLAY_H
#define MQDISPLAY_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>
#include "RTDisplay.h"
#include "RTConnectorMQD.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class TQRootCanvas;
class QButtonGroup;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QFrame;

class MQDisplay : public QMainWindow
{
    Q_OBJECT

public:
    MQDisplay( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~MQDisplay();

    QButtonGroup* buttonGroup2;
    QPushButton* pbDraw;
    QSpinBox* sbEvent;
    QPushButton* pbPREV;
    QPushButton* pbNEXT;
    QTextEdit* teOUT;
    QFrame* frame4;
    TQRootCanvas* Canvas1;
    QPushButton* pbAnalize;
    QPushButton* pbCLD;
    QPushButton* pbRCLD;
    QPushButton* pb3DD;
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

    RTConnectorMQD *fConn;

    virtual void TextUpdate();
    virtual void SetFile( const char * fname );

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
    virtual void pbPREV_clicked();
    virtual void pbNEXT_clicked();
    virtual void pbOPEN_clicked();
    virtual void pbDraw_clicked();
    virtual void pbAnalize_clicked();
    virtual void pbCLD_clicked();
    virtual void pbRCLD_clicked();
    virtual void pb3DD_clicked();
    virtual void SetEntry( int entry );
    virtual void drawEventConn();
    virtual void setEntryConn( int entry );
    virtual void setMaxConn( int max );

signals:
    void drawEvent();
    void quitApp();
    void setEntry(int);

protected:
    QHBoxLayout* MQDisplayLayout;
    QVBoxLayout* buttonGroup2Layout;
    QGridLayout* layout16;
    QGridLayout* frame4Layout;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

    void init();

};

#endif // MQDISPLAY_H
