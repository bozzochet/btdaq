/****************************************************************************
** Form implementation generated from reading ui file 'errordiag.ui'
**
** Created: Thu Oct 25 00:27:38 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "errordiag.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "errordiag.ui.h"
/*
 *  Constructs a ErrorDiag as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ErrorDiag::ErrorDiag( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ErrorDiag" );
    ErrorDiagLayout = new QGridLayout( this, 1, 1, 11, 6, "ErrorDiagLayout"); 

    textLabel3 = new QLabel( this, "textLabel3" );

    ErrorDiagLayout->addMultiCellWidget( textLabel3, 0, 0, 0, 2 );

    pushButton68 = new QPushButton( this, "pushButton68" );

    ErrorDiagLayout->addWidget( pushButton68, 1, 1 );
    spacer36 = new QSpacerItem( 140, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ErrorDiagLayout->addItem( spacer36, 1, 0 );
    spacer37 = new QSpacerItem( 131, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    ErrorDiagLayout->addItem( spacer37, 1, 2 );
    languageChange();
    resize( QSize(332, 105).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( pushButton68, SIGNAL( clicked() ), this, SLOT( close() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ErrorDiag::~ErrorDiag()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ErrorDiag::languageChange()
{
    setCaption( tr( "Error" ) );
    textLabel3->setText( tr( "<p align=\"center\">Error the Daq Object Pointer is NULL</p>" ) );
    pushButton68->setText( tr( "OK" ) );
}

