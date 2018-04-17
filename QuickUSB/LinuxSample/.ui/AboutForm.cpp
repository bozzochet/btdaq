/****************************************************************************
** Form implementation generated from reading ui file 'AboutForm.ui'
**
** Created by User Interface Compiler
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "AboutForm.h"

#include <qvariant.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "../AboutForm.ui.h"
/*
 *  Constructs a AboutForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
AboutForm::AboutForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "AboutForm" );
    setFocusPolicy( QDialog::StrongFocus );
    setModal( TRUE );

    aboutFormRichEdit = new QTextEdit( this, "aboutFormRichEdit" );
    aboutFormRichEdit->setGeometry( QRect( 0, 80, 270, 105 ) );
    aboutFormRichEdit->setFrameShape( QTextEdit::NoFrame );
    aboutFormRichEdit->setFrameShadow( QTextEdit::Plain );
    aboutFormRichEdit->setVScrollBarMode( QTextEdit::AlwaysOff );
    aboutFormRichEdit->setHScrollBarMode( QTextEdit::AlwaysOff );
    aboutFormRichEdit->setDragAutoScroll( FALSE );
    aboutFormRichEdit->setReadOnly( TRUE );

    QuickUsbLogo = new QLabel( this, "QuickUsbLogo" );
    QuickUsbLogo->setGeometry( QRect( 0, 0, 267, 83 ) );
    QuickUsbLogo->setFocusPolicy( QLabel::StrongFocus );
    QuickUsbLogo->setPixmap( QPixmap::fromMimeSource( "QuickUSB_Logo.jpeg" ) );
    QuickUsbLogo->setScaledContents( TRUE );
    languageChange();
    resize( QSize(260, 154).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
AboutForm::~AboutForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AboutForm::languageChange()
{
    setCaption( tr( "About QuickUSB Diagnostics" ) );
    aboutFormRichEdit->setText( QString::null );
}

