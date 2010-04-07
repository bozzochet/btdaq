/****************************************************************************
** Form implementation generated from reading ui file 'hitdump.ui'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "hitdump.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "hitdump.ui.h"
/*
 *  Constructs a HitDump as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
HitDump::HitDump( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "HitDump" );
    HitDumpLayout = new QVBoxLayout( this, 11, 6, "HitDumpLayout"); 

    groupBox1 = new QGroupBox( this, "groupBox1" );
    groupBox1->setMinimumSize( QSize( 0, 65 ) );
    groupBox1->setMaximumSize( QSize( 32767, 65 ) );
    groupBox1->setColumnLayout(0, Qt::Vertical );
    groupBox1->layout()->setSpacing( 6 );
    groupBox1->layout()->setMargin( 11 );
    groupBox1Layout = new QHBoxLayout( groupBox1->layout() );
    groupBox1Layout->setAlignment( Qt::AlignTop );

    cbType = new QComboBox( FALSE, groupBox1, "cbType" );
    cbType->setMinimumSize( QSize( 110, 20 ) );
    cbType->setMaximumSize( QSize( 110, 20 ) );
    cbType->setInsertionPolicy( QComboBox::NoInsertion );
    cbType->setDuplicatesEnabled( FALSE );
    groupBox1Layout->addWidget( cbType );

    cbLayers = new QComboBox( FALSE, groupBox1, "cbLayers" );
    cbLayers->setMinimumSize( QSize( 120, 20 ) );
    cbLayers->setMaximumSize( QSize( 120, 20 ) );
    cbLayers->setSizeLimit( 8 );
    cbLayers->setInsertionPolicy( QComboBox::NoInsertion );
    cbLayers->setDuplicatesEnabled( FALSE );
    groupBox1Layout->addWidget( cbLayers );
    spacer1 = new QSpacerItem( 91, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    groupBox1Layout->addItem( spacer1 );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3"); 

    textLabel1 = new QLabel( groupBox1, "textLabel1" );
    layout3->addWidget( textLabel1 );

    spinBox3 = new QSpinBox( groupBox1, "spinBox3" );
    layout3->addWidget( spinBox3 );

    pbDClus = new QPushButton( groupBox1, "pbDClus" );
    layout3->addWidget( pbDClus );
    groupBox1Layout->addLayout( layout3 );
    HitDumpLayout->addWidget( groupBox1 );

    teDump = new QTextEdit( this, "teDump" );
    teDump->setMinimumSize( QSize( 560, 350 ) );
    teDump->setTextFormat( QTextEdit::LogText );
    HitDumpLayout->addWidget( teDump );
    languageChange();
    resize( QSize(602, 810).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( cbType, SIGNAL( textChanged(const QString&) ), this, SLOT( Update() ) );
    connect( cbLayers, SIGNAL( textChanged(const QString&) ), this, SLOT( Update() ) );
    connect( cbLayers, SIGNAL( activated(const QString&) ), this, SLOT( Update() ) );
    connect( cbType, SIGNAL( activated(int) ), this, SLOT( Update() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
HitDump::~HitDump()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void HitDump::languageChange()
{
    setCaption( tr( "Hit Cluster Dump" ) );
    groupBox1->setTitle( QString::null );
    textLabel1->setText( tr( "Hit #" ) );
    pbDClus->setText( tr( "Display Clusters" ) );
}

