/****************************************************************************
** Form implementation generated from reading ui file 'wgonadv.ui'
**
** Created: Wed Oct 24 14:30:03 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "wgonadv.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "wgonadv.ui.h"
/*
 *  Constructs a wGonAdv as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
wGonAdv::wGonAdv( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "wGonAdv" );
    wGonAdvLayout = new QVBoxLayout( this, 11, 6, "wGonAdvLayout"); 

    pbGonSetPos = new QPushButton( this, "pbGonSetPos" );
    wGonAdvLayout->addWidget( pbGonSetPos );

    pbGonHwCal = new QPushButton( this, "pbGonHwCal" );
    wGonAdvLayout->addWidget( pbGonHwCal );

    pbGonSwCal = new QPushButton( this, "pbGonSwCal" );
    wGonAdvLayout->addWidget( pbGonSwCal );

    pbGonZero = new QPushButton( this, "pbGonZero" );
    wGonAdvLayout->addWidget( pbGonZero );

    pbGonAbort = new QPushButton( this, "pbGonAbort" );
    wGonAdvLayout->addWidget( pbGonAbort );

    pbGonSendRaw = new QPushButton( this, "pbGonSendRaw" );
    wGonAdvLayout->addWidget( pbGonSendRaw );

    leGonRawCom = new QLineEdit( this, "leGonRawCom" );
    wGonAdvLayout->addWidget( leGonRawCom );
    languageChange();
    resize( QSize(233, 286).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pbGonSetPos, SIGNAL( clicked() ), this, SLOT( pbGonSetPos_clicked() ) );
    connect( pbGonSwCal, SIGNAL( clicked() ), this, SLOT( pbGonSwCal_clicked() ) );
    connect( pbGonAbort, SIGNAL( clicked() ), this, SLOT( pbGonAbort_clicked() ) );
    connect( pbGonZero, SIGNAL( clicked() ), this, SLOT( pbGonZero_clicked() ) );
    connect( pbGonHwCal, SIGNAL( clicked() ), this, SLOT( pbGonHwCal_clicked() ) );
    connect( pbGonSendRaw, SIGNAL( clicked() ), this, SLOT( pbGonSendRaw_clicked() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
wGonAdv::~wGonAdv()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void wGonAdv::languageChange()
{
    setCaption( tr( "Goniometer Advanced" ) );
    pbGonSetPos->setText( tr( "Set Zero Here" ) );
    pbGonHwCal->setText( tr( "HW Reset" ) );
    pbGonSwCal->setText( tr( "SW Calibration" ) );
    pbGonZero->setText( tr( "Go to Zero" ) );
    pbGonAbort->setText( tr( "Abort" ) );
    pbGonSendRaw->setText( tr( "SendRawCommand" ) );
    QToolTip::add( leGonRawCom, tr( "Raw Gon Command" ) );
}

