/****************************************************************************
** Form implementation generated from reading ui file 'clrdisplay.ui'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "clrdisplay.h"

#include <qvariant.h>
#include <TQRootCanvas.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qspinbox.h>
#include <qtextedit.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "clrdisplay.ui.h"
/*
 *  Constructs a ClRDisplay as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
ClRDisplay::ClRDisplay( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "ClRDisplay" );
    ClRDisplayLayout = new QVBoxLayout( this, 11, 6, "ClRDisplayLayout"); 

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setMinimumSize( QSize( 0, 180 ) );
    groupBox2->setMaximumSize( QSize( 32767, 180 ) );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    layout17 = new QHBoxLayout( 0, 0, 6, "layout17"); 

    sbRClS = new QSpinBox( groupBox2, "sbRClS" );
    sbRClS->setMinimumSize( QSize( 100, 0 ) );
    sbRClS->setMaximumSize( QSize( 100, 32767 ) );
    sbRClS->setMaxValue( 999 );
    layout17->addWidget( sbRClS );

    pbRDrawS = new QPushButton( groupBox2, "pbRDrawS" );
    pbRDrawS->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)0, 0, 0, pbRDrawS->sizePolicy().hasHeightForWidth() ) );
    pbRDrawS->setMinimumSize( QSize( 100, 0 ) );
    pbRDrawS->setMaximumSize( QSize( 100, 32767 ) );
    layout17->addWidget( pbRDrawS );
    spacer1 = new QSpacerItem( 372, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout17->addItem( spacer1 );
    groupBox2Layout->addLayout( layout17 );

    teRSOUT = new QTextEdit( groupBox2, "teRSOUT" );
    teRSOUT->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, teRSOUT->sizePolicy().hasHeightForWidth() ) );
    teRSOUT->setMinimumSize( QSize( 0, 70 ) );
    teRSOUT->setMaximumSize( QSize( 32767, 150 ) );
    teRSOUT->setTextFormat( QTextEdit::AutoText );
    groupBox2Layout->addWidget( teRSOUT );
    ClRDisplayLayout->addWidget( groupBox2 );

    frame7 = new QFrame( this, "frame7" );
    frame7->setMinimumSize( QSize( 300, 400 ) );
    frame7->setFrameShape( QFrame::Box );
    frame7->setFrameShadow( QFrame::Raised );
    frame7->setLineWidth( 2 );
    frame7Layout = new QVBoxLayout( frame7, 11, 6, "frame7Layout"); 

    CanvasRS = new TQRootCanvas( frame7, "CanvasRS" );
    frame7Layout->addWidget( CanvasRS );
    ClRDisplayLayout->addWidget( frame7 );
    languageChange();
    resize( QSize(627, 614).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pbRDrawS, SIGNAL( clicked() ), this, SLOT( UpdateS() ) );
    connect( sbRClS, SIGNAL( valueChanged(int) ), this, SLOT( UpdateS() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
ClRDisplay::~ClRDisplay()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ClRDisplay::languageChange()
{
    setCaption( tr( "Raw Cluster Display" ) );
    groupBox2->setTitle( QString::null );
    pbRDrawS->setText( tr( "Draw " ) );
}

