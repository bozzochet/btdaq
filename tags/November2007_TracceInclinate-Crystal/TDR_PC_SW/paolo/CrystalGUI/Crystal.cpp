/****************************************************************************
** Form implementation generated from reading ui file 'Crystal.ui'
**
** Created: Wed Oct 24 14:29:49 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "Crystal.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlcdnumber.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "Crystal.ui.h"
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

    splitter1 = new QSplitter( centralWidget(), "splitter1" );
    splitter1->setGeometry( QRect( 10, 10, 1151, 777 ) );
    splitter1->setOrientation( QSplitter::Vertical );

    frame3 = new QFrame( splitter1, "frame3" );
    frame3->setFrameShape( QFrame::StyledPanel );
    frame3->setFrameShadow( QFrame::Raised );
    frame3Layout = new QHBoxLayout( frame3, 11, 6, "frame3Layout"); 

    gbGon = new QGroupBox( frame3, "gbGon" );
    gbGon->setFrameShadow( QGroupBox::Sunken );
    gbGon->setLineWidth( 1 );
    gbGon->setMargin( 0 );
    gbGon->setMidLineWidth( 0 );
    gbGon->setColumnLayout(0, Qt::Vertical );
    gbGon->layout()->setSpacing( 6 );
    gbGon->layout()->setMargin( 11 );
    gbGonLayout = new QHBoxLayout( gbGon->layout() );
    gbGonLayout->setAlignment( Qt::AlignTop );

    layout99 = new QVBoxLayout( 0, 0, 6, "layout99"); 

    layout96 = new QHBoxLayout( 0, 0, 6, "layout96"); 

    tlb1 = new QLabel( gbGon, "tlb1" );
    QFont tlb1_font(  tlb1->font() );
    tlb1_font.setPointSize( 14 );
    tlb1->setFont( tlb1_font ); 
    layout96->addWidget( tlb1 );

    tlb1_2 = new QLabel( gbGon, "tlb1_2" );
    QFont tlb1_2_font(  tlb1_2->font() );
    tlb1_2_font.setPointSize( 14 );
    tlb1_2->setFont( tlb1_2_font ); 
    layout96->addWidget( tlb1_2 );

    tlb1_3 = new QLabel( gbGon, "tlb1_3" );
    QFont tlb1_3_font(  tlb1_3->font() );
    tlb1_3_font.setPointSize( 14 );
    tlb1_3->setFont( tlb1_3_font ); 
    layout96->addWidget( tlb1_3 );

    tlb1_4 = new QLabel( gbGon, "tlb1_4" );
    QFont tlb1_4_font(  tlb1_4->font() );
    tlb1_4_font.setPointSize( 14 );
    tlb1_4->setFont( tlb1_4_font ); 
    layout96->addWidget( tlb1_4 );
    layout99->addLayout( layout96 );

    layout95 = new QVBoxLayout( 0, 0, 6, "layout95"); 

    layout92 = new QHBoxLayout( 0, 0, 6, "layout92"); 

    LCDGon1 = new QLCDNumber( gbGon, "LCDGon1" );
    LCDGon1->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    LCDGon1->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    QFont LCDGon1_font(  LCDGon1->font() );
    LCDGon1->setFont( LCDGon1_font ); 
    LCDGon1->setNumDigits( 10 );
    LCDGon1->setMode( QLCDNumber::Dec );
    LCDGon1->setSegmentStyle( QLCDNumber::Flat );
    LCDGon1->setProperty( "value", 10.2 );
    layout92->addWidget( LCDGon1 );

    LCDGon4 = new QLCDNumber( gbGon, "LCDGon4" );
    LCDGon4->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    LCDGon4->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    LCDGon4->setNumDigits( 10 );
    LCDGon4->setSegmentStyle( QLCDNumber::Flat );
    LCDGon4->setProperty( "value", 10.2 );
    layout92->addWidget( LCDGon4 );

    LCDGon2 = new QLCDNumber( gbGon, "LCDGon2" );
    LCDGon2->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    LCDGon2->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    LCDGon2->setNumDigits( 10 );
    LCDGon2->setSegmentStyle( QLCDNumber::Flat );
    LCDGon2->setProperty( "value", 10.2 );
    layout92->addWidget( LCDGon2 );

    LCDGon3 = new QLCDNumber( gbGon, "LCDGon3" );
    LCDGon3->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    LCDGon3->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    LCDGon3->setNumDigits( 10 );
    LCDGon3->setSegmentStyle( QLCDNumber::Flat );
    LCDGon3->setProperty( "value", 10.2 );
    layout92->addWidget( LCDGon3 );
    layout95->addLayout( layout92 );

    layout93 = new QHBoxLayout( 0, 0, 6, "layout93"); 

    leGonAlpha = new QLineEdit( gbGon, "leGonAlpha" );
    leGonAlpha->setAlignment( int( QLineEdit::AlignRight ) );
    layout93->addWidget( leGonAlpha );

    leGonCrad = new QLineEdit( gbGon, "leGonCrad" );
    leGonCrad->setAlignment( int( QLineEdit::AlignRight ) );
    layout93->addWidget( leGonCrad );

    leGonLin1 = new QLineEdit( gbGon, "leGonLin1" );
    leGonLin1->setAlignment( int( QLineEdit::AlignRight ) );
    layout93->addWidget( leGonLin1 );

    leGonLin2 = new QLineEdit( gbGon, "leGonLin2" );
    leGonLin2->setAlignment( int( QLineEdit::AlignRight ) );
    layout93->addWidget( leGonLin2 );
    layout95->addLayout( layout93 );
    layout99->addLayout( layout95 );

    pbGonGet = new QPushButton( gbGon, "pbGonGet" );
    layout99->addWidget( pbGonGet );
    gbGonLayout->addLayout( layout99 );

    layout14 = new QVBoxLayout( 0, 0, 6, "layout14"); 

    buttonGroup4 = new QButtonGroup( gbGon, "buttonGroup4" );
    buttonGroup4->setColumnLayout(0, Qt::Vertical );
    buttonGroup4->layout()->setSpacing( 6 );
    buttonGroup4->layout()->setMargin( 11 );
    buttonGroup4Layout = new QVBoxLayout( buttonGroup4->layout() );
    buttonGroup4Layout->setAlignment( Qt::AlignTop );

    rbGonMurad = new QRadioButton( buttonGroup4, "rbGonMurad" );
    buttonGroup4Layout->addWidget( rbGonMurad );

    rbGonRad = new QRadioButton( buttonGroup4, "rbGonRad" );
    rbGonRad->setChecked( TRUE );
    buttonGroup4Layout->addWidget( rbGonRad );

    rbGonGrad = new QRadioButton( buttonGroup4, "rbGonGrad" );
    buttonGroup4Layout->addWidget( rbGonGrad );
    layout14->addWidget( buttonGroup4 );

    pbGonAbort = new QPushButton( gbGon, "pbGonAbort" );
    layout14->addWidget( pbGonAbort );

    pbGonSet = new QPushButton( gbGon, "pbGonSet" );
    layout14->addWidget( pbGonSet );
    gbGonLayout->addLayout( layout14 );

    layout13 = new QVBoxLayout( 0, 0, 6, "layout13"); 
    spacer6_2 = new QSpacerItem( 20, 101, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout13->addItem( spacer6_2 );

    buttonGroup3 = new QButtonGroup( gbGon, "buttonGroup3" );
    buttonGroup3->setLineWidth( 1 );
    buttonGroup3->setColumnLayout(0, Qt::Vertical );
    buttonGroup3->layout()->setSpacing( 6 );
    buttonGroup3->layout()->setMargin( 11 );
    buttonGroup3Layout = new QVBoxLayout( buttonGroup3->layout() );
    buttonGroup3Layout->setAlignment( Qt::AlignTop );

    rbAbs = new QRadioButton( buttonGroup3, "rbAbs" );
    rbAbs->setChecked( TRUE );
    buttonGroup3Layout->addWidget( rbAbs );

    rbRel = new QRadioButton( buttonGroup3, "rbRel" );
    buttonGroup3Layout->addWidget( rbRel );
    layout13->addWidget( buttonGroup3 );
    gbGonLayout->addLayout( layout13 );
    frame3Layout->addWidget( gbGon );

    gbTrig = new QGroupBox( frame3, "gbTrig" );
    gbTrig->setColumnLayout(0, Qt::Vertical );
    gbTrig->layout()->setSpacing( 6 );
    gbTrig->layout()->setMargin( 11 );
    gbTrigLayout = new QVBoxLayout( gbTrig->layout() );
    gbTrigLayout->setAlignment( Qt::AlignTop );

    tlTrigRun = new QLabel( gbTrig, "tlTrigRun" );
    tlTrigRun->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    tlTrigRun->setPaletteBackgroundColor( QColor( 170, 255, 255 ) );
    QFont tlTrigRun_font(  tlTrigRun->font() );
    tlTrigRun->setFont( tlTrigRun_font ); 
    tlTrigRun->setFrameShape( QLabel::Box );
    tlTrigRun->setFrameShadow( QLabel::Raised );
    tlTrigRun->setTextFormat( QLabel::LogText );
    tlTrigRun->setAlignment( int( QLabel::AlignCenter ) );
    gbTrigLayout->addWidget( tlTrigRun );

    lCDNumber9 = new QLCDNumber( gbTrig, "lCDNumber9" );
    lCDNumber9->setPaletteForegroundColor( QColor( 0, 0, 0 ) );
    lCDNumber9->setPaletteBackgroundColor( QColor( 255, 255, 255 ) );
    lCDNumber9->setFrameShadow( QLCDNumber::Raised );
    lCDNumber9->setSegmentStyle( QLCDNumber::Flat );
    gbTrigLayout->addWidget( lCDNumber9 );

    layout12 = new QHBoxLayout( 0, 0, 6, "layout12"); 

    pbTrigStart = new QPushButton( gbTrig, "pbTrigStart" );
    layout12->addWidget( pbTrigStart );

    pbTrigStop = new QPushButton( gbTrig, "pbTrigStop" );
    layout12->addWidget( pbTrigStop );

    pbTrigReset = new QPushButton( gbTrig, "pbTrigReset" );
    layout12->addWidget( pbTrigReset );
    gbTrigLayout->addLayout( layout12 );

    layout13_2 = new QHBoxLayout( 0, 0, 6, "layout13_2"); 
    spacer6 = new QSpacerItem( 141, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout13_2->addItem( spacer6 );

    pbTrigGet = new QPushButton( gbTrig, "pbTrigGet" );
    layout13_2->addWidget( pbTrigGet );
    gbTrigLayout->addLayout( layout13_2 );

    buttonGroup6 = new QButtonGroup( gbTrig, "buttonGroup6" );
    buttonGroup6->setColumnLayout(0, Qt::Vertical );
    buttonGroup6->layout()->setSpacing( 6 );
    buttonGroup6->layout()->setMargin( 11 );
    buttonGroup6Layout = new QGridLayout( buttonGroup6->layout() );
    buttonGroup6Layout->setAlignment( Qt::AlignTop );

    rbTrigType2 = new QRadioButton( buttonGroup6, "rbTrigType2" );

    buttonGroup6Layout->addWidget( rbTrigType2, 1, 0 );

    rbTrigType1 = new QRadioButton( buttonGroup6, "rbTrigType1" );
    rbTrigType1->setChecked( TRUE );

    buttonGroup6Layout->addWidget( rbTrigType1, 0, 0 );

    rbTrigType3 = new QRadioButton( buttonGroup6, "rbTrigType3" );

    buttonGroup6Layout->addWidget( rbTrigType3, 0, 1 );

    rbTrigType4 = new QRadioButton( buttonGroup6, "rbTrigType4" );

    buttonGroup6Layout->addWidget( rbTrigType4, 1, 1 );
    gbTrigLayout->addWidget( buttonGroup6 );
    frame3Layout->addWidget( gbTrig );

    frameDaq = new QFrame( splitter1, "frameDaq" );
    frameDaq->setMinimumSize( QSize( 0, 20 ) );
    frameDaq->setFrameShape( QFrame::StyledPanel );
    frameDaq->setFrameShadow( QFrame::Raised );
    frameDaqLayout = new QHBoxLayout( frameDaq, 11, 6, "frameDaqLayout"); 

    gbMain = new QGroupBox( splitter1, "gbMain" );
    gbMain->setMinimumSize( QSize( 0, 200 ) );

    pbSng = new QPushButton( gbMain, "pbSng" );
    pbSng->setGeometry( QRect( 879, 41, 161, 40 ) );

    pbScan = new QPushButton( gbMain, "pbScan" );
    pbScan->setGeometry( QRect( 879, 101, 161, 31 ) );

    txEMainLog = new QTextEdit( gbMain, "txEMainLog" );
    txEMainLog->setGeometry( QRect( 19, 41, 820, 130 ) );
    txEMainLog->setTextFormat( QTextEdit::LogText );

    // actions
    filePrintAction = new QAction( this, "filePrintAction" );
    filePrintAction->setIconSet( QIconSet( QPixmap::fromMimeSource( "" ) ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    toolsGoniometer_AdvancedAction = new QAction( this, "toolsGoniometer_AdvancedAction" );


    // toolbars


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );


    fileMenu = new QPopupMenu( this );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 1 );

    editMenu = new QPopupMenu( this );
    editMenu->insertSeparator();
    editMenu->insertSeparator();
    MenuBar->insertItem( QString(""), editMenu, 2 );

    Tools = new QPopupMenu( this );
    toolsGoniometer_AdvancedAction->addTo( Tools );
    MenuBar->insertItem( QString(""), Tools, 3 );

    MenuBar->insertSeparator( 4 );

    languageChange();
    resize( QSize(1100, 846).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( this, SIGNAL( dockWindowPositionChanged(QDockWindow*) ), this, SLOT( CrystalGUI_dockWindowPositionChanged(QDockWindow*) ) );
    connect( this, SIGNAL( usesTextLabelChanged(bool) ), this, SLOT( CrystalGUI_usesTextLabelChanged(bool) ) );
    connect( pbGonGet, SIGNAL( clicked() ), this, SLOT( pbGonGet_clicked() ) );
    connect( pbGonSet, SIGNAL( pressed() ), this, SLOT( SetGonPos() ) );
    connect( toolsGoniometer_AdvancedAction, SIGNAL( activated() ), this, SLOT( toolsGoniometer_AdvancedAction_activated() ) );
    connect( rbGonMurad, SIGNAL( toggled(bool) ), this, SLOT( rbGonMurad_toggled(bool) ) );
    connect( rbGonRad, SIGNAL( toggled(bool) ), this, SLOT( rbGonRad_toggled(bool) ) );
    connect( rbGonGrad, SIGNAL( toggled(bool) ), this, SLOT( rbGonGrad_toggled(bool) ) );
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
    tlb1->setText( tr( "<p align=\"center\">Alpha <br>(murad)</p>" ) );
    tlb1_2->setText( tr( "<p align=\"center\">Craddle <br>(murad)</p>" ) );
    tlb1_3->setText( tr( "<p align=\"center\">Linear1<br>(mm)</p>" ) );
    tlb1_4->setText( tr( "<p align=\"center\">Linear2<br> (mm)</p>" ) );
    QToolTip::add( LCDGon1, tr( "Alpha (murad)" ) );
    QToolTip::add( LCDGon4, tr( "Craddle (murad)" ) );
    QToolTip::add( LCDGon2, tr( "Linear1 (mm)" ) );
    QToolTip::add( LCDGon3, tr( "Linear2 (mm)" ) );
    leGonAlpha->setText( tr( "0" ) );
    QToolTip::add( leGonAlpha, tr( "Alpha" ) );
    leGonCrad->setText( tr( "0" ) );
    QToolTip::add( leGonCrad, tr( "Cradle" ) );
    leGonLin1->setText( tr( "0" ) );
    QToolTip::add( leGonLin1, tr( "Linear1" ) );
    leGonLin2->setText( tr( "0" ) );
    QToolTip::add( leGonLin2, tr( "Linear2" ) );
    pbGonGet->setText( tr( "Get Position" ) );
    buttonGroup4->setTitle( tr( "Display unit" ) );
    rbGonMurad->setText( tr( "muRadiant" ) );
    rbGonRad->setText( tr( "Radiants" ) );
    rbGonGrad->setText( tr( "Grad" ) );
    pbGonAbort->setText( tr( "Abort" ) );
    pbGonSet->setText( tr( "Move Gon" ) );
    buttonGroup3->setTitle( QString::null );
    rbAbs->setText( tr( "Absolute" ) );
    rbRel->setText( tr( "Relative" ) );
    gbTrig->setTitle( tr( "Trigger Control" ) );
    tlTrigRun->setText( tr( "textLabel1" ) );
    pbTrigStart->setText( tr( "Start" ) );
    pbTrigStop->setText( tr( "Stop" ) );
    pbTrigReset->setText( tr( "Reset" ) );
    pbTrigGet->setText( tr( "Get Status" ) );
    buttonGroup6->setTitle( tr( "Trig Type" ) );
    rbTrigType2->setText( tr( "Type2" ) );
    rbTrigType1->setText( tr( "Type1" ) );
    rbTrigType3->setText( tr( "Type3" ) );
    rbTrigType4->setText( tr( "Type4" ) );
    gbMain->setTitle( tr( "Main  DAQ Control" ) );
    pbSng->setText( tr( "SingleDaq" ) );
    pbScan->setText( tr( "Scan" ) );
    filePrintAction->setText( tr( "Print" ) );
    filePrintAction->setMenuText( tr( "&Print..." ) );
    filePrintAction->setAccel( tr( "Ctrl+P" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( QString::null );
    toolsGoniometer_AdvancedAction->setText( tr( "Goniometer Advanced" ) );
    toolsGoniometer_AdvancedAction->setMenuText( tr( "Goniometer Advanced" ) );
    if (MenuBar->findItem(1))
        MenuBar->findItem(1)->setText( tr( "&File" ) );
    if (MenuBar->findItem(2))
        MenuBar->findItem(2)->setText( tr( "&Edit" ) );
    if (MenuBar->findItem(3))
        MenuBar->findItem(3)->setText( tr( "Tools" ) );
}

