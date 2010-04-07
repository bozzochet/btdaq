/****************************************************************************
** Form implementation generated from reading ui file 'Crystal.ui'
**
** Created: Sun Aug 26 20:05:59 2007
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
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qlineedit.h>
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
    CrystalGUILayout = new QGridLayout( centralWidget(), 1, 1, 11, 6, "CrystalGUILayout"); 

    frame7 = new QFrame( centralWidget(), "frame7" );
    frame7->setFrameShape( QFrame::StyledPanel );
    frame7->setFrameShadow( QFrame::Raised );

    gbMain = new QGroupBox( frame7, "gbMain" );
    gbMain->setGeometry( QRect( 20, 500, 838, 180 ) );

    gbTrig = new QGroupBox( frame7, "gbTrig" );
    gbTrig->setGeometry( QRect( 20, 230, 429, 192 ) );
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

    gbGon = new QGroupBox( frame7, "gbGon" );
    gbGon->setGeometry( QRect( 0, 0, 759, 212 ) );
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

    CrystalGUILayout->addWidget( frame7, 0, 0 );

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

    languageChange();
    resize( QSize(871, 795).expandedTo(minimumSizeHint()) );
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
    gbMain->setTitle( tr( "Main  DAQ Control" ) );
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

