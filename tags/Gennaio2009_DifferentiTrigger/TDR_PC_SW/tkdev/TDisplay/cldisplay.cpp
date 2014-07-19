/****************************************************************************
** Form implementation generated from reading ui file 'cldisplay.ui'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "cldisplay.h"

#include <qvariant.h>
#include <TQRootCanvas.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qradiobutton.h>
#include <qtextedit.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "cldisplay.ui.h"
/*
 *  Constructs a ClDisplay as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
ClDisplay::ClDisplay( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "ClDisplay" );

    groupBox2 = new QGroupBox( this, "groupBox2" );
    groupBox2->setGeometry( QRect( 11, 11, 620, 180 ) );
    groupBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)5, 0, 0, groupBox2->sizePolicy().hasHeightForWidth() ) );
    groupBox2->setMinimumSize( QSize( 0, 180 ) );
    groupBox2->setMaximumSize( QSize( 32767, 180 ) );
    groupBox2->setColumnLayout(0, Qt::Vertical );
    groupBox2->layout()->setSpacing( 6 );
    groupBox2->layout()->setMargin( 11 );
    groupBox2Layout = new QVBoxLayout( groupBox2->layout() );
    groupBox2Layout->setAlignment( Qt::AlignTop );

    layout6 = new QHBoxLayout( 0, 0, 6, "layout6"); 

    textLabel1 = new QLabel( groupBox2, "textLabel1" );
    layout6->addWidget( textLabel1 );

    sbClS1 = new QSpinBox( groupBox2, "sbClS1" );
    sbClS1->setMinimumSize( QSize( 70, 0 ) );
    sbClS1->setMaximumSize( QSize( 100, 32767 ) );
    sbClS1->setMaxValue( 0 );
    layout6->addWidget( sbClS1 );

    textLabel2_2 = new QLabel( groupBox2, "textLabel2_2" );
    layout6->addWidget( textLabel2_2 );

    sbClS2 = new QSpinBox( groupBox2, "sbClS2" );
    sbClS2->setMinimumSize( QSize( 70, 0 ) );
    sbClS2->setMaximumSize( QSize( 100, 32767 ) );
    sbClS2->setMaxValue( 8 );
    sbClS2->setMinValue( 1 );
    sbClS2->setValue( 1 );
    layout6->addWidget( sbClS2 );

    textLabel2 = new QLabel( groupBox2, "textLabel2" );
    layout6->addWidget( textLabel2 );

    sbClS3 = new QSpinBox( groupBox2, "sbClS3" );
    sbClS3->setMinimumSize( QSize( 70, 0 ) );
    sbClS3->setMaximumSize( QSize( 100, 32767 ) );
    sbClS3->setMaxValue( 815 );
    sbClS3->setMinValue( -815 );
    sbClS3->setValue( 0 );
    layout6->addWidget( sbClS3 );

    rbNside = new QRadioButton( groupBox2, "rbNside" );
    rbNside->setEnabled( FALSE );
    layout6->addWidget( rbNside );

    rbPside = new QRadioButton( groupBox2, "rbPside" );
    rbPside->setEnabled( FALSE );
    layout6->addWidget( rbPside );

    sbClS4 = new QSpinBox( groupBox2, "sbClS4" );
    sbClS4->setEnabled( TRUE );
    sbClS4->setMinimumSize( QSize( 70, 0 ) );
    sbClS4->setMaximumSize( QSize( 100, 32767 ) );
    sbClS4->setMaxValue( 0 );
    layout6->addWidget( sbClS4 );

    pbDrawS = new QPushButton( groupBox2, "pbDrawS" );
    pbDrawS->setMinimumSize( QSize( 100, 0 ) );
    pbDrawS->setMaximumSize( QSize( 100, 32767 ) );
    layout6->addWidget( pbDrawS );
    groupBox2Layout->addLayout( layout6 );

    teSOUT = new QTextEdit( groupBox2, "teSOUT" );
    teSOUT->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, teSOUT->sizePolicy().hasHeightForWidth() ) );
    teSOUT->setMinimumSize( QSize( 0, 70 ) );
    teSOUT->setMaximumSize( QSize( 32767, 150 ) );
    teSOUT->setTextFormat( QTextEdit::AutoText );
    groupBox2Layout->addWidget( teSOUT );

    frame7_2 = new QFrame( this, "frame7_2" );
    frame7_2->setGeometry( QRect( 10, 200, 620, 406 ) );
    frame7_2->setMinimumSize( QSize( 300, 400 ) );
    frame7_2->setFrameShape( QFrame::Box );
    frame7_2->setFrameShadow( QFrame::Raised );
    frame7_2->setLineWidth( 2 );
    frame7_2Layout = new QVBoxLayout( frame7_2, 11, 6, "frame7_2Layout"); 

    CanvasS = new TQRootCanvas( frame7_2, "CanvasS" );
    frame7_2Layout->addWidget( CanvasS );
    languageChange();
    resize( QSize(641, 617).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pbDrawS, SIGNAL( clicked() ), this, SLOT( UpdateS() ) );
    connect( sbClS4, SIGNAL( valueChanged(int) ), this, SLOT( UpdateIdx() ) );
    connect( sbClS1, SIGNAL( valueChanged(int) ), this, SLOT( UpdateS() ) );
    connect( rbPside, SIGNAL( toggled(bool) ), this, SLOT( rbPside_toggled(bool) ) );
    connect( rbNside, SIGNAL( toggled(bool) ), this, SLOT( rbNside_toggled(bool) ) );
    connect( sbClS1, SIGNAL( valueChanged(int) ), this, SLOT( UpdateDigits() ) );
    connect( sbClS2, SIGNAL( valueChanged(int) ), this, SLOT( UpdateTkIdRange() ) );
    connect( sbClS3, SIGNAL( valueChanged(int) ), this, SLOT( UpdateNP() ) );
    connect( sbClS3, SIGNAL( valueChanged(int) ), this, SLOT( UpdateTkId() ) );
    connect( sbClS2, SIGNAL( valueChanged(int) ), this, SLOT( UpdateLayer() ) );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
ClDisplay::~ClDisplay()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ClDisplay::languageChange()
{
    setCaption( tr( "Cluster Display" ) );
    groupBox2->setTitle( QString::null );
    textLabel1->setText( tr( "index" ) );
    textLabel2_2->setText( tr( "Layer" ) );
    textLabel2->setText( tr( "TkID" ) );
    sbClS3->setPrefix( QString::null );
    sbClS3->setSuffix( QString::null );
    sbClS3->setSpecialValueText( QString::null );
    rbNside->setText( tr( "n" ) );
    rbPside->setText( tr( "p" ) );
    pbDrawS->setText( tr( "Draw" ) );
}

