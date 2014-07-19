/****************************************************************************
** Form implementation generated from reading ui file 'TDRGUI.ui'
**
** Created: Thu Oct 25 00:27:55 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "TDRGUI.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "TDRGUI.ui.h"

/*
 *  Constructs a Daq0 as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
Daq0::Daq0( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "Daq0" );
    Daq0Layout = new QHBoxLayout( this, 11, 6, "Daq0Layout"); 

    layout13 = new QVBoxLayout( 0, 0, 6, "layout13"); 

    layout12 = new QHBoxLayout( 0, 0, 6, "layout12"); 

    txLaTitle = new QLabel( this, "txLaTitle" );
    txLaTitle->setMinimumSize( QSize( 100, 0 ) );
    layout12->addWidget( txLaTitle );

    pbGetLog = new QPushButton( this, "pbGetLog" );
    layout12->addWidget( pbGetLog );

    pbStopLog = new QPushButton( this, "pbStopLog" );
    layout12->addWidget( pbStopLog );

    pbNewLog = new QPushButton( this, "pbNewLog" );
    layout12->addWidget( pbNewLog );
    layout13->addLayout( layout12 );

    textEdit2 = new QTextEdit( this, "textEdit2" );
    layout13->addWidget( textEdit2 );

    layout11 = new QHBoxLayout( 0, 0, 6, "layout11"); 

    pbUpdate = new QPushButton( this, "pbUpdate" );
    layout11->addWidget( pbUpdate );

    pbClear = new QPushButton( this, "pbClear" );
    layout11->addWidget( pbClear );
    layout13->addLayout( layout11 );
    Daq0Layout->addLayout( layout13 );

    buttonGroup2 = new QButtonGroup( this, "buttonGroup2" );
    buttonGroup2->setMinimumSize( QSize( 200, 0 ) );
    buttonGroup2->setMaximumSize( QSize( 200, 32767 ) );
    buttonGroup2->setColumnLayout(0, Qt::Vertical );
    buttonGroup2->layout()->setSpacing( 6 );
    buttonGroup2->layout()->setMargin( 11 );
    buttonGroup2Layout = new QVBoxLayout( buttonGroup2->layout() );
    buttonGroup2Layout->setAlignment( Qt::AlignTop );

    textEdCount = new QTextEdit( buttonGroup2, "textEdCount" );
    buttonGroup2Layout->addWidget( textEdCount );

    pbInit = new QPushButton( buttonGroup2, "pbInit" );
    buttonGroup2Layout->addWidget( pbInit );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    layout4 = new QVBoxLayout( 0, 0, 6, "layout4"); 

    textLabel1 = new QLabel( buttonGroup2, "textLabel1" );
    layout4->addWidget( textLabel1 );

    leRun = new QLineEdit( buttonGroup2, "leRun" );
    leRun->setAlignment( int( QLineEdit::AlignRight ) );
    layout4->addWidget( leRun );
    layout6->addLayout( layout4 );

    layout5 = new QVBoxLayout( 0, 0, 6, "layout5"); 

    textLabel2 = new QLabel( buttonGroup2, "textLabel2" );
    layout5->addWidget( textLabel2 );

    leEvMax = new QLineEdit( buttonGroup2, "leEvMax" );
    leEvMax->setAlignment( int( QLineEdit::AlignRight ) );
    layout5->addWidget( leEvMax );
    layout6->addLayout( layout5 );
    buttonGroup2Layout->addLayout( layout6 );

    pbStart = new QPushButton( buttonGroup2, "pbStart" );
    buttonGroup2Layout->addWidget( pbStart );

    pbStop = new QPushButton( buttonGroup2, "pbStop" );
    buttonGroup2Layout->addWidget( pbStop );
    spacer16 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    buttonGroup2Layout->addItem( spacer16 );

    pbCalib = new QPushButton( buttonGroup2, "pbCalib" );
    buttonGroup2Layout->addWidget( pbCalib );

    pbKill = new QPushButton( buttonGroup2, "pbKill" );
    buttonGroup2Layout->addWidget( pbKill );
    Daq0Layout->addWidget( buttonGroup2 );
    languageChange();
    resize( QSize(964, 510).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pbGetLog, SIGNAL( clicked() ), this, SLOT( pbGetLog_clicked() ) );
    connect( pbClear, SIGNAL( clicked() ), this, SLOT( pbClear_clicked() ) );
    connect( pbInit, SIGNAL( clicked() ), this, SLOT( pbInit_clicked() ) );
    connect( pbStart, SIGNAL( clicked() ), this, SLOT( pbStart_clicked() ) );
    connect( textEdCount, SIGNAL( doubleClicked(int,int) ), this, SLOT( UpdateCount() ) );
    connect( pbStopLog, SIGNAL( clicked() ), this, SLOT( pbStopLog_clicked() ) );
    connect( textEdit2, SIGNAL( doubleClicked(int,int) ), this, SLOT( UpdateLog() ) );
    connect( pbUpdate, SIGNAL( clicked() ), this, SLOT( pbUpdate_clicked() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
Daq0::~Daq0()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Daq0::languageChange()
{
    setCaption( tr( "Daq0" ) );
    txLaTitle->setText( tr( "textLabel2" ) );
    pbGetLog->setText( tr( "Start Log" ) );
    pbStopLog->setText( tr( "Stop Log" ) );
    pbNewLog->setText( tr( "New Log" ) );
    pbUpdate->setText( tr( "Update" ) );
    pbClear->setText( tr( "Clear" ) );
    buttonGroup2->setTitle( QString::null );
    pbInit->setText( tr( "Init" ) );
    textLabel1->setText( tr( "<p align=\"center\">Run</p>" ) );
    leRun->setText( tr( "100" ) );
    textLabel2->setText( tr( "<p align=\"center\">Ev Max</p>" ) );
    leEvMax->setText( tr( "1000" ) );
    pbStart->setText( tr( "Start" ) );
    pbStop->setText( tr( "Stop" ) );
    pbCalib->setText( tr( "Calibrate" ) );
    pbKill->setText( tr( "Kill" ) );
}

