/****************************************************************************
** Form implementation generated from reading ui file 'gonioadv.ui'
**
** Created: Sun Aug 26 19:16:36 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "gonioadv.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "gonioadv.ui.h"
/*
 *  Constructs a GonioAdv as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
GonioAdv::GonioAdv( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "GonioAdv" );
    GonioAdvLayout = new QVBoxLayout( this, 11, 6, "GonioAdvLayout"); 

    pbGonSetPos = new QPushButton( this, "pbGonSetPos" );
    GonioAdvLayout->addWidget( pbGonSetPos );

    pbGonHwCal = new QPushButton( this, "pbGonHwCal" );
    GonioAdvLayout->addWidget( pbGonHwCal );

    pbGonSwCal = new QPushButton( this, "pbGonSwCal" );
    GonioAdvLayout->addWidget( pbGonSwCal );

    pbGonZero = new QPushButton( this, "pbGonZero" );
    GonioAdvLayout->addWidget( pbGonZero );

    pbGonAbort = new QPushButton( this, "pbGonAbort" );
    GonioAdvLayout->addWidget( pbGonAbort );

    pbGonSendRaw = new QPushButton( this, "pbGonSendRaw" );
    GonioAdvLayout->addWidget( pbGonSendRaw );

    leGonRawCom = new QLineEdit( this, "leGonRawCom" );
    GonioAdvLayout->addWidget( leGonRawCom );
    languageChange();
    resize( QSize(256, 281).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pbGonSetPos, SIGNAL( clicked() ), this, SLOT( pbGonSetPos_clicked() ) );
    connect( pbGonHwCal, SIGNAL( clicked() ), this, SLOT( pbGonHwCal_clicked() ) );
    connect( pbGonZero, SIGNAL( clicked() ), this, SLOT( pbGonZero_clicked() ) );
    connect( pbGonAbort, SIGNAL( clicked() ), this, SLOT( pbGonAbort_clicked() ) );
    connect( pbGonSendRaw, SIGNAL( clicked() ), this, SLOT( pbGonSendRaw_clicked() ) );
    connect( pbGonSwCal, SIGNAL( clicked() ), this, SLOT( pbGonSwCal_clicked() ) );

    // tab order
    setTabOrder( pbGonSendRaw, leGonRawCom );
    setTabOrder( leGonRawCom, pbGonSetPos );
    setTabOrder( pbGonSetPos, pbGonHwCal );
    setTabOrder( pbGonHwCal, pbGonZero );
    setTabOrder( pbGonZero, pbGonAbort );
}

/*
 *  Destroys the object and frees any allocated resources
 */
GonioAdv::~GonioAdv()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void GonioAdv::languageChange()
{
    setCaption( tr( "Goniometer Advanced " ) );
    pbGonSetPos->setText( tr( "Set Zero Here" ) );
    pbGonHwCal->setText( tr( "HW Reset" ) );
    pbGonSwCal->setText( tr( "SW Calibration" ) );
    pbGonZero->setText( tr( "Go to Zero" ) );
    pbGonAbort->setText( tr( "Abort" ) );
    pbGonSendRaw->setText( tr( "SendRawCommand" ) );
    QToolTip::add( leGonRawCom, tr( "Raw Gon Command" ) );
}

