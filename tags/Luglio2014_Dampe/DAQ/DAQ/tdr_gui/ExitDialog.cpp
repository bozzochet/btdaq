/****************************************************************************
** Form implementation generated from reading ui file 'ExitDialog.ui'
**
** Created: Thu Oct 25 00:27:50 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "ExitDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a MyDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
MyDialog::MyDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "MyDialog" );
    setSizeGripEnabled( TRUE );
    MyDialogLayout = new QVBoxLayout( this, 11, 6, "MyDialogLayout"); 

    layout64 = new QVBoxLayout( 0, 0, 6, "layout64"); 

    layout33 = new QVBoxLayout( 0, 0, 6, "layout33"); 

    textLabel2 = new QLabel( this, "textLabel2" );
    layout33->addWidget( textLabel2 );
    spacer15 = new QSpacerItem( 271, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout33->addItem( spacer15 );
    layout64->addLayout( layout33 );

    layout63 = new QHBoxLayout( 0, 0, 6, "layout63"); 
    spacer30 = new QSpacerItem( 31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout63->addItem( spacer30 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    layout63->addWidget( buttonOk );
    Horizontal_Spacing2 = new QSpacerItem( 50, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout63->addItem( Horizontal_Spacing2 );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setToggleButton( TRUE );
    buttonCancel->setAutoDefault( TRUE );
    layout63->addWidget( buttonCancel );
    spacer30_2 = new QSpacerItem( 31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout63->addItem( spacer30_2 );
    layout64->addLayout( layout63 );
    MyDialogLayout->addLayout( layout64 );
    languageChange();
    resize( QSize(321, 139).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MyDialog::~MyDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MyDialog::languageChange()
{
    setCaption( tr( "ExitDialog" ) );
    textLabel2->setText( tr( "<p align=\"center\">You are Closing the Crystal GUI<br>Are you sure?</p>" ) );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( tr( "&Cancel" ) );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

