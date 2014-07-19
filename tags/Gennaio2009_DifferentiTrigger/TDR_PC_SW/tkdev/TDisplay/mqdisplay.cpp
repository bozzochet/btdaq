/****************************************************************************
** Form implementation generated from reading ui file 'mqdisplay.ui'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "mqdisplay.h"

#include <qvariant.h>
#include <TQRootCanvas.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "mqdisplay.ui.h"
/*
 *  Constructs a MQDisplay as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
MQDisplay::MQDisplay( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    if ( !name )
	setName( "MQDisplay" );
    setCursor( QCursor( 13 ) );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );
    MQDisplayLayout = new QHBoxLayout( centralWidget(), 11, 6, "MQDisplayLayout"); 

    buttonGroup2 = new QButtonGroup( centralWidget(), "buttonGroup2" );
    buttonGroup2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)7, 0, 0, buttonGroup2->sizePolicy().hasHeightForWidth() ) );
    buttonGroup2->setMinimumSize( QSize( 290, 0 ) );
    buttonGroup2->setMaximumSize( QSize( 290, 32767 ) );
    buttonGroup2->setFrameShape( QButtonGroup::GroupBoxPanel );
    buttonGroup2->setAlignment( int( QButtonGroup::AlignHCenter ) );
    buttonGroup2->setColumnLayout(0, Qt::Vertical );
    buttonGroup2->layout()->setSpacing( 6 );
    buttonGroup2->layout()->setMargin( 11 );
    buttonGroup2Layout = new QVBoxLayout( buttonGroup2->layout() );
    buttonGroup2Layout->setAlignment( Qt::AlignTop );

    layout16 = new QGridLayout( 0, 1, 1, 0, 6, "layout16"); 

    pbDraw = new QPushButton( buttonGroup2, "pbDraw" );

    layout16->addWidget( pbDraw, 1, 1 );

    sbEvent = new QSpinBox( buttonGroup2, "sbEvent" );
    sbEvent->setMaxValue( 99999999 );

    layout16->addWidget( sbEvent, 1, 0 );

    pbPREV = new QPushButton( buttonGroup2, "pbPREV" );

    layout16->addWidget( pbPREV, 0, 0 );

    pbNEXT = new QPushButton( buttonGroup2, "pbNEXT" );

    layout16->addWidget( pbNEXT, 0, 1 );
    buttonGroup2Layout->addLayout( layout16 );

    teOUT = new QTextEdit( buttonGroup2, "teOUT" );
    teOUT->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)4, (QSizePolicy::SizeType)7, 0, 0, teOUT->sizePolicy().hasHeightForWidth() ) );
    teOUT->setMinimumSize( QSize( 250, 0 ) );
    teOUT->setMaximumSize( QSize( 250, 32767 ) );
    teOUT->setTextFormat( QTextEdit::LogText );
    buttonGroup2Layout->addWidget( teOUT );
    MQDisplayLayout->addWidget( buttonGroup2 );

    frame4 = new QFrame( centralWidget(), "frame4" );
    frame4->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, frame4->sizePolicy().hasHeightForWidth() ) );
    frame4->setMinimumSize( QSize( 500, 550 ) );
    frame4->setFrameShape( QFrame::Box );
    frame4->setFrameShadow( QFrame::Raised );
    frame4->setLineWidth( 2 );
    frame4->setMargin( 2 );
    frame4Layout = new QGridLayout( frame4, 1, 1, 11, 6, "frame4Layout"); 

    Canvas1 = new TQRootCanvas( frame4, "Canvas1" );
    Canvas1->setMinimumSize( QSize( 0, 0 ) );

    frame4Layout->addWidget( Canvas1, 0, 0 );
    MQDisplayLayout->addWidget( frame4 );

    // actions
    fileNewAction = new QAction( this, "fileNewAction" );
    fileNewAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filenew" ) ) );
    fileOpenAction = new QAction( this, "fileOpenAction" );
    fileOpenAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "fileopen" ) ) );
    fileSaveAction = new QAction( this, "fileSaveAction" );
    fileSaveAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "filesave" ) ) );
    fileSaveAsAction = new QAction( this, "fileSaveAsAction" );
    filePrintAction = new QAction( this, "filePrintAction" );
    filePrintAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "print" ) ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    editUndoAction = new QAction( this, "editUndoAction" );
    editUndoAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "undo" ) ) );
    editRedoAction = new QAction( this, "editRedoAction" );
    editRedoAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "redo" ) ) );
    editCutAction = new QAction( this, "editCutAction" );
    editCutAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "editcut" ) ) );
    editCopyAction = new QAction( this, "editCopyAction" );
    editCopyAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "editcopy" ) ) );
    editPasteAction = new QAction( this, "editPasteAction" );
    editPasteAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "editpaste" ) ) );
    editFindAction = new QAction( this, "editFindAction" );
    editFindAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "searchfind" ) ) );


    // toolbars
    toolBar = new QToolBar( QString(""), this, DockTop ); 

    editCutAction->addTo( toolBar );
    editCopyAction->addTo( toolBar );
    toolBar->addSeparator();
    editPasteAction->addTo( toolBar );
    fileOpenAction->addTo( toolBar );
    filePrintAction->addTo( toolBar );

    pbAnalize = new QPushButton( toolBar, "pbAnalize" );

    pbCLD = new QPushButton( toolBar, "pbCLD" );

    pbRCLD = new QPushButton( toolBar, "pbRCLD" );
    toolBar->addSeparator();

    pb3DD = new QPushButton( toolBar, "pb3DD" );
    pb3DD->setPaletteBackgroundColor( QColor( 170, 170, 127 ) );
    pb3DD->setCursor( QCursor( 13 ) );


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );

    MenuBar->setCursor( QCursor( 0 ) );

    fileMenu = new QPopupMenu( this );
    fileNewAction->addTo( fileMenu );
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 2 );

    editMenu = new QPopupMenu( this );
    editUndoAction->addTo( editMenu );
    editRedoAction->addTo( editMenu );
    editMenu->insertSeparator();
    editCutAction->addTo( editMenu );
    editCopyAction->addTo( editMenu );
    editPasteAction->addTo( editMenu );
    editMenu->insertSeparator();
    editFindAction->addTo( editMenu );
    MenuBar->insertItem( QString(""), editMenu, 3 );

    languageChange();
    resize( QSize(818, 653).expandedTo(minimumSizeHint()) );
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
    connect( pbPREV, SIGNAL( clicked() ), this, SLOT( pbPREV_clicked() ) );
    connect( pbNEXT, SIGNAL( clicked() ), this, SLOT( pbNEXT_clicked() ) );
    connect( pbDraw, SIGNAL( clicked() ), this, SLOT( pbDraw_clicked() ) );
    connect( pbAnalize, SIGNAL( clicked() ), this, SLOT( pbAnalize_clicked() ) );
    connect( pbCLD, SIGNAL( clicked() ), this, SLOT( pbCLD_clicked() ) );
    connect( pbRCLD, SIGNAL( clicked() ), this, SLOT( pbRCLD_clicked() ) );
    connect( pb3DD, SIGNAL( clicked() ), this, SLOT( pb3DD_clicked() ) );
    connect( sbEvent, SIGNAL( valueChanged(int) ), this, SLOT( pbDraw_clicked() ) );
    connect( sbEvent, SIGNAL( valueChanged(int) ), this, SLOT( setEntryConn(int) ) );
    connect( this, SIGNAL( drawEvent() ), this, SLOT( drawEventConn() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
MQDisplay::~MQDisplay()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MQDisplay::languageChange()
{
    setCaption( tr( "Tracker Display" ) );
    buttonGroup2->setTitle( QString::null );
    pbDraw->setText( tr( "Draw" ) );
    QToolTip::add( sbEvent, tr( "Put the event nuber" ) );
    pbPREV->setText( tr( "Previous" ) );
    pbNEXT->setText( tr( "Next" ) );
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
    fileExitAction->setAccel( tr( "Ctrl+Q" ) );
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
    pbAnalize->setText( tr( "Hit/Clusters" ) );
    pbCLD->setText( tr( "Cluster Display" ) );
    pbRCLD->setText( tr( "Raw Cluster Display" ) );
    pb3DD->setText( tr( "3D Display" ) );
    QToolTip::add( pb3DD, tr( "Open a 3D display window" ) );
    QWhatsThis::add( pb3DD, QString::null );
    if (MenuBar->findItem(2))
        MenuBar->findItem(2)->setText( tr( "&File" ) );
    if (MenuBar->findItem(3))
        MenuBar->findItem(3)->setText( tr( "&Edit" ) );
}

