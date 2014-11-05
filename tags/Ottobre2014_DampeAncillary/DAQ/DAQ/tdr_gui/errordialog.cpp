/****************************************************************************
** Form implementation generated from reading ui file 'errordialog.ui'
**
** Created: Thu Oct 25 00:27:45 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "errordialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "errordialog.ui.h"

/*
 *  Constructs a ErrorDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ErrorDialog::ErrorDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ErrorDialog" );
    setMinimumSize( QSize( 400, 150 ) );
    setMaximumSize( QSize( 400, 150 ) );
    setSizeGripEnabled( TRUE );
    ErrorDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "ErrorDialogLayout"); 

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    ErrorDialogLayout->addLayout( Layout1, 1, 0 );

    errLabel = new QLabel( this, "errLabel" );
    errLabel->setAlignment( int( QLabel::AlignCenter ) );

    ErrorDialogLayout->addWidget( errLabel, 0, 0 );
    languageChange();
    resize( QSize(400, 150).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ErrorDialog::~ErrorDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ErrorDialog::languageChange()
{
    setCaption( tr( "Error" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    errLabel->setText( tr( "textLabel1" ) );
}

