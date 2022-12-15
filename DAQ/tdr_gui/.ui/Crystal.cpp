/****************************************************************************
** Form implementation generated from reading ui file 'Crystal.ui'
**
** Created: Sat Aug 25 17:50:28 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "Crystal.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlcdnumber.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qtextbrowser.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "../Crystal.ui.h"
/*
 *  Constructs a CrystalGUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
CrystalGUI::CrystalGUI( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "CrystalGUI" );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );

    frame7 = new QFrame( centralWidget(), "frame7" );
    frame7->setGeometry( QRect( 0, 10, 880, 750 ) );
    frame7->setFrameShape( QFrame::StyledPanel );
    frame7->setFrameShadow( QFrame::Raised );

    gbGon = new QGroupBox( frame7, "gbGon" );
    gbGon->setGeometry( QRect( 440, 0, 414, 210 ) );
    gbGon->setColumnLayout(0, Qt::Vertical );
    gbGon->layout()->setSpacing( 6 );
    gbGon->layout()->setMargin( 11 );
    gbGonLayout = new QVBoxLayout( gbGon->layout() );
    gbGonLayout->setAlignment( Qt::AlignTop );

    layout38 = new QHBoxLayout( 0, 0, 6, "layout38"); 

    layout37 = new QGridLayout( 0, 1, 1, 0, 6, "layout37"); 

    LCDGon3 = new QLCDNumber( gbGon, "LCDGon3" );
    LCDGon3->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    LCDGon3->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    LCDGon3->setSegmentStyle( QLCDNumber::Flat );
    LCDGon3->setProperty( "value", 10.2 );

    layout37->addWidget( LCDGon3, 1, 4 );

    LCDGon4 = new QLCDNumber( gbGon, "LCDGon4" );
    LCDGon4->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    LCDGon4->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    LCDGon4->setSegmentStyle( QLCDNumber::Flat );
    LCDGon4->setProperty( "value", 10.2 );

    layout37->addWidget( LCDGon4, 1, 1 );

    tlb1_4 = new QLabel( gbGon, "tlb1_4" );

    layout37->addWidget( tlb1_4, 0, 4 );

    tlb1 = new QLabel( gbGon, "tlb1" );

    layout37->addWidget( tlb1, 0, 0 );

    sbLin2 = new QSpinBox( gbGon, "sbLin2" );

    layout37->addWidget( sbLin2, 2, 4 );

    sbAlpha = new QSpinBox( gbGon, "sbAlpha" );
    sbAlpha->setMaxValue( 300000 );
    sbAlpha->setMinValue( -300000 );

    layout37->addWidget( sbAlpha, 2, 0 );

    sbLin1 = new QSpinBox( gbGon, "sbLin1" );

    layout37->addMultiCellWidget( sbLin1, 2, 2, 2, 3 );

    tlb1_3 = new QLabel( gbGon, "tlb1_3" );

    layout37->addWidget( tlb1_3, 0, 3 );

    sbCrad = new QSpinBox( gbGon, "sbCrad" );
    sbCrad->setMaxValue( 300000 );
    sbCrad->setMinValue( -300000 );

    layout37->addWidget( sbCrad, 2, 1 );

    LCDGon1 = new QLCDNumber( gbGon, "LCDGon1" );
    LCDGon1->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    LCDGon1->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    LCDGon1->setSegmentStyle( QLCDNumber::Flat );
    LCDGon1->setProperty( "value", 10.2 );

    layout37->addWidget( LCDGon1, 1, 0 );

    LCDGon2 = new QLCDNumber( gbGon, "LCDGon2" );
    LCDGon2->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    LCDGon2->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    LCDGon2->setSegmentStyle( QLCDNumber::Flat );
    LCDGon2->setProperty( "value", 10.2 );

    layout37->addMultiCellWidget( LCDGon2, 1, 1, 2, 3 );

    tlb1_2 = new QLabel( gbGon, "tlb1_2" );

    layout37->addMultiCellWidget( tlb1_2, 0, 0, 1, 2 );
    layout38->addLayout( layout37 );

    pbGonGet = new QPushButton( gbGon, "pbGonGet" );
    layout38->addWidget( pbGonGet );
    gbGonLayout->addLayout( layout38 );

    layout39 = new QHBoxLayout( 0, 0, 6, "layout39"); 

    pbGonSet = new QPushButton( gbGon, "pbGonSet" );
    layout39->addWidget( pbGonSet );

    buttonGroup3 = new QButtonGroup( gbGon, "buttonGroup3" );
    buttonGroup3->setLineWidth( 0 );
    buttonGroup3->setColumnLayout(0, Qt::Vertical );
    buttonGroup3->layout()->setSpacing( 6 );
    buttonGroup3->layout()->setMargin( 11 );
    buttonGroup3Layout = new QHBoxLayout( buttonGroup3->layout() );
    buttonGroup3Layout->setAlignment( Qt::AlignTop );

    rbAbs = new QRadioButton( buttonGroup3, "rbAbs" );
    rbAbs->setChecked( TRUE );
    buttonGroup3Layout->addWidget( rbAbs );

    rbRel = new QRadioButton( buttonGroup3, "rbRel" );
    buttonGroup3Layout->addWidget( rbRel );
    layout39->addWidget( buttonGroup3 );
    gbGonLayout->addLayout( layout39 );

    gbTrig = new QGroupBox( frame7, "gbTrig" );
    gbTrig->setGeometry( QRect( 10, 10, 429, 192 ) );
    gbTrig->setColumnLayout(0, Qt::Vertical );
    gbTrig->layout()->setSpacing( 6 );
    gbTrig->layout()->setMargin( 11 );
    gbTrigLayout = new QHBoxLayout( gbTrig->layout() );
    gbTrigLayout->setAlignment( Qt::AlignTop );

    layout59 = new QVBoxLayout( 0, 0, 6, "layout59"); 
    spacer10 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout59->addItem( spacer10 );

    lCDNumber9 = new QLCDNumber( gbTrig, "lCDNumber9" );
    lCDNumber9->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    lCDNumber9->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    lCDNumber9->setFrameShadow( QLCDNumber::Sunken );
    lCDNumber9->setSegmentStyle( QLCDNumber::Flat );
    layout59->addWidget( lCDNumber9 );
    spacer9 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout59->addItem( spacer9 );

    layout57 = new QHBoxLayout( 0, 0, 6, "layout57"); 

    pbTrigStart = new QPushButton( gbTrig, "pbTrigStart" );
    layout57->addWidget( pbTrigStart );
    spacer6 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout57->addItem( spacer6 );

    pbTrigStop = new QPushButton( gbTrig, "pbTrigStop" );
    layout57->addWidget( pbTrigStop );
    layout59->addLayout( layout57 );

    pbTrigReset = new QPushButton( gbTrig, "pbTrigReset" );
    layout59->addWidget( pbTrigReset );
    gbTrigLayout->addLayout( layout59 );

    layout51 = new QVBoxLayout( 0, 0, 6, "layout51"); 

    buttonGroup5 = new QButtonGroup( gbTrig, "buttonGroup5" );
    buttonGroup5->setLineWidth( 0 );
    buttonGroup5->setColumnLayout(0, Qt::Vertical );
    buttonGroup5->layout()->setSpacing( 6 );
    buttonGroup5->layout()->setMargin( 11 );
    buttonGroup5Layout = new QHBoxLayout( buttonGroup5->layout() );
    buttonGroup5Layout->setAlignment( Qt::AlignTop );

    rbTrigEx = new QRadioButton( buttonGroup5, "rbTrigEx" );
    rbTrigEx->setChecked( TRUE );
    buttonGroup5Layout->addWidget( rbTrigEx );

    rbTrigIn = new QRadioButton( buttonGroup5, "rbTrigIn" );
    buttonGroup5Layout->addWidget( rbTrigIn );
    layout51->addWidget( buttonGroup5 );

    buttonGroup7 = new QButtonGroup( gbTrig, "buttonGroup7" );
    buttonGroup7->setLineWidth( 0 );
    buttonGroup7->setColumnLayout(0, Qt::Vertical );
    buttonGroup7->layout()->setSpacing( 6 );
    buttonGroup7->layout()->setMargin( 11 );
    buttonGroup7Layout = new QHBoxLayout( buttonGroup7->layout() );
    buttonGroup7Layout->setAlignment( Qt::AlignTop );

    rbTrigSpillIn = new QRadioButton( buttonGroup7, "rbTrigSpillIn" );
    rbTrigSpillIn->setChecked( TRUE );
    buttonGroup7Layout->addWidget( rbTrigSpillIn );

    rbTrigSpillOut = new QRadioButton( buttonGroup7, "rbTrigSpillOut" );
    buttonGroup7Layout->addWidget( rbTrigSpillOut );
    layout51->addWidget( buttonGroup7 );

    layout50 = new QHBoxLayout( 0, 0, 6, "layout50"); 
    spacer5 = new QSpacerItem( 41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout50->addItem( spacer5 );

    pbTrigGet = new QPushButton( gbTrig, "pbTrigGet" );
    layout50->addWidget( pbTrigGet );
    layout51->addLayout( layout50 );
    gbTrigLayout->addLayout( layout51 );

    gbDAQPG0 = new QGroupBox( frame7, "gbDAQPG0" );
    gbDAQPG0->setGeometry( QRect( 10, 230, 490, 250 ) );
    gbDAQPG0->setColumnLayout(0, Qt::Vertical );
    gbDAQPG0->layout()->setSpacing( 6 );
    gbDAQPG0->layout()->setMargin( 11 );
    gbDAQPG0Layout = new QHBoxLayout( gbDAQPG0->layout() );
    gbDAQPG0Layout->setAlignment( Qt::AlignTop );

    layout62 = new QVBoxLayout( 0, 0, 6, "layout62"); 

    tbPG0Log = new QTextBrowser( gbDAQPG0, "tbPG0Log" );
    tbPG0Log->setAcceptDrops( FALSE );
    layout62->addWidget( tbPG0Log );

    layout61 = new QHBoxLayout( 0, 0, 6, "layout61"); 

    pbPG0GetLog = new QPushButton( gbDAQPG0, "pbPG0GetLog" );
    layout61->addWidget( pbPG0GetLog );

    pbPG0Clear = new QPushButton( gbDAQPG0, "pbPG0Clear" );
    layout61->addWidget( pbPG0Clear );

    pbPG0NewLog = new QPushButton( gbDAQPG0, "pbPG0NewLog" );
    layout61->addWidget( pbPG0NewLog );
    layout62->addLayout( layout61 );
    gbDAQPG0Layout->addLayout( layout62 );

    layout63 = new QVBoxLayout( 0, 0, 6, "layout63"); 

    pbPG0Kill = new QPushButton( gbDAQPG0, "pbPG0Kill" );
    layout63->addWidget( pbPG0Kill );

    pbPG0Init = new QPushButton( gbDAQPG0, "pbPG0Init" );
    layout63->addWidget( pbPG0Init );

    pbPG0Start = new QPushButton( gbDAQPG0, "pbPG0Start" );
    layout63->addWidget( pbPG0Start );

    pbPG0Stop = new QPushButton( gbDAQPG0, "pbPG0Stop" );
    layout63->addWidget( pbPG0Stop );
    spacer11 = new QSpacerItem( 20, 41, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout63->addItem( spacer11 );
    gbDAQPG0Layout->addLayout( layout63 );

    gbMain = new QGroupBox( frame7, "gbMain" );
    gbMain->setGeometry( QRect( 20, 500, 838, 180 ) );

    // actions
    fileNewAction = new QAction( this, "fileNewAction" );
    fileNewAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    fileSaveAction = new QAction( this, "fileSaveAction" );
    fileSaveAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    fileSaveAsAction = new QAction( this, "fileSaveAsAction" );
    filePrintAction = new QAction( this, "filePrintAction" );
    filePrintAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    editUndoAction = new QAction( this, "editUndoAction" );
    editUndoAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    editRedoAction = new QAction( this, "editRedoAction" );
    editRedoAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    editCutAction = new QAction( this, "editCutAction" );
    editCutAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    editCopyAction = new QAction( this, "editCopyAction" );
    editCopyAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    editPasteAction = new QAction( this, "editPasteAction" );
    editPasteAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    editFindAction = new QAction( this, "editFindAction" );
    editFindAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );


    // toolbars
    toolBar = new QToolBar( QString(""), this, DockTop ); 

    fileNewAction->addTo( toolBar );
    fileOpenAction->addTo( toolBar );
    fileSaveAction->addTo( toolBar );
    fileSaveAsAction->addTo( toolBar );
    filePrintAction->addTo( toolBar );
    fileExitAction->addTo( toolBar );
    editUndoAction->addTo( toolBar );
    editRedoAction->addTo( toolBar );
    editCutAction->addTo( toolBar );
    editCopyAction->addTo( toolBar );
    editPasteAction->addTo( toolBar );


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );


    fileMenu = new QPopupMenu( this );
    fileNewAction->addTo( fileMenu );
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 1 );

    editMenu = new QPopupMenu( this );
    editUndoAction->addTo( editMenu );
    editRedoAction->addTo( editMenu );
    editMenu->insertSeparator();
    editCutAction->addTo( editMenu );
    editCopyAction->addTo( editMenu );
    editPasteAction->addTo( editMenu );
    editMenu->insertSeparator();
    editFindAction->addTo( editMenu );
    MenuBar->insertItem( QString(""), editMenu, 2 );

    languageChange();
    resize( QSize(870, 795).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    connect( fileOpenAction, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    connect( fileSaveAction, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    connect( fileSaveAsAction, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( editUndoAction, SIGNAL( activated() ), this, SLOT( editUndo() ) );
    connect( editRedoAction, SIGNAL( activated() ), this, SLOT( editRedo() ) );
    connect( editCutAction, SIGNAL( activated() ), this, SLOT( editCut() ) );
    connect( editCopyAction, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    connect( editPasteAction, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    connect( editFindAction, SIGNAL( activated() ), this, SLOT( editFind() ) );
    connect( this, SIGNAL( dockWindowPositionChanged(QDockWindow*) ), this, SLOT( CrystalGUI_dockWindowPositionChanged(QDockWindow*) ) );
    connect( this, SIGNAL( usesTextLabelChanged(bool) ), this, SLOT( CrystalGUI_usesTextLabelChanged(bool) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
CrystalGUI::~CrystalGUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void CrystalGUI::languageChange()
{
    setCaption( tr( "Crystal Main DAQ" ) );
    gbGon->setTitle( tr( "Goniometer Control" ) );
    QToolTip::add( LCDGon3, tr( "Linear2 (mm)" ) );
    QToolTip::add( LCDGon4, tr( "Craddle (murad)" ) );
    tlb1_4->setText( tr( "<p align=\"center\">Linear2<br> (mm)</p>" ) );
    tlb1->setText( tr( "<p align=\"center\">Alpha <br>(murad)</p>" ) );
    QToolTip::add( sbLin2, tr( "Linear2" ) );
    QToolTip::add( sbAlpha, tr( "Alpha" ) );
    QToolTip::add( sbLin1, tr( "Linear1" ) );
    tlb1_3->setText( tr( "<p align=\"center\">Linear1<br>(mm)</p>" ) );
    QToolTip::add( sbCrad, tr( "Craddle" ) );
    QToolTip::add( LCDGon1, tr( "Alpha (murad)" ) );
    QToolTip::add( LCDGon2, tr( "Linear1 (mm)" ) );
    tlb1_2->setText( tr( "<p align=\"center\">Craddle <br>(murad)</p>" ) );
    pbGonGet->setText( tr( "Get\n"
"Position" ) );
    pbGonSet->setText( tr( "Set Position" ) );
    buttonGroup3->setTitle( QString::null );
    rbAbs->setText( tr( "Absolute" ) );
    rbRel->setText( tr( "Relative" ) );
    gbTrig->setTitle( tr( "Trigger Control" ) );
    pbTrigStart->setText( tr( "Start" ) );
    pbTrigStop->setText( tr( "Stop" ) );
    pbTrigReset->setText( tr( "Reset" ) );
    buttonGroup5->setTitle( QString::null );
    rbTrigEx->setText( tr( "External" ) );
    rbTrigIn->setText( tr( "Periodic" ) );
    buttonGroup7->setTitle( QString::null );
    rbTrigSpillIn->setText( tr( "InSpill" ) );
    rbTrigSpillOut->setText( tr( "Out of Spill" ) );
    pbTrigGet->setText( tr( "Get Status" ) );
    gbDAQPG0->setTitle( tr( "DAQ Perugia 0" ) );
    pbPG0GetLog->setText( tr( "Get Log" ) );
    pbPG0Clear->setText( tr( "Clear" ) );
    pbPG0NewLog->setText( tr( "New Log" ) );
    pbPG0Kill->setText( tr( "Kill" ) );
    pbPG0Init->setText( tr( "Init" ) );
    pbPG0Start->setText( tr( "Start" ) );
    pbPG0Stop->setText( tr( "Stop" ) );
    gbMain->setTitle( tr( "Main  DAQ Control" ) );
    fileNewAction->setText( tr( "New" ) );
    fileNewAction->setMenuText( tr( "&New" ) );
    fileNewAction->setAccel( tr( "Ctrl+N" ) );
    fileOpenAction->setText( tr( "Open" ) );
    fileOpenAction->setMenuText( tr( "&Open..." ) );
    fileOpenAction->setAccel( tr( "Ctrl+O" ) );
    fileSaveAction->setText( tr( "Save" ) );
    fileSaveAction->setMenuText( tr( "&Save" ) );
    fileSaveAction->setAccel( tr( "Ctrl+S" ) );
    fileSaveAsAction->setText( tr( "Save As" ) );
    fileSaveAsAction->setMenuText( tr( "Save &As..." ) );
    fileSaveAsAction->setAccel( QString::null );
    filePrintAction->setText( tr( "Print" ) );
    filePrintAction->setMenuText( tr( "&Print..." ) );
    filePrintAction->setAccel( tr( "Ctrl+P" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( QString::null );
    editUndoAction->setText( tr( "Undo" ) );
    editUndoAction->setMenuText( tr( "&Undo" ) );
    editUndoAction->setAccel( tr( "Ctrl+Z" ) );
    editRedoAction->setText( tr( "Redo" ) );
    editRedoAction->setMenuText( tr( "&Redo" ) );
    editRedoAction->setAccel( tr( "Ctrl+Y" ) );
    editCutAction->setText( tr( "Cut" ) );
    editCutAction->setMenuText( tr( "Cu&t" ) );
    editCutAction->setAccel( tr( "Ctrl+X" ) );
    editCopyAction->setText( tr( "Copy" ) );
    editCopyAction->setMenuText( tr( "&Copy" ) );
    editCopyAction->setAccel( tr( "Ctrl+C" ) );
    editPasteAction->setText( tr( "Paste" ) );
    editPasteAction->setMenuText( tr( "&Paste" ) );
    editPasteAction->setAccel( tr( "Ctrl+V" ) );
    editFindAction->setText( tr( "Find" ) );
    editFindAction->setMenuText( tr( "&Find..." ) );
    editFindAction->setAccel( tr( "Ctrl+F" ) );
    toolBar->setLabel( tr( "Tools" ) );
    if (MenuBar->findItem(1))
        MenuBar->findItem(1)->setText( tr( "&File" ) );
    if (MenuBar->findItem(2))
        MenuBar->findItem(2)->setText( tr( "&Edit" ) );
}

