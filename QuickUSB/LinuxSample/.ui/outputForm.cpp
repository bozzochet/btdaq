/****************************************************************************
** Form implementation generated from reading ui file 'outputForm.ui'
**
** Created by User Interface Compiler
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "outputForm.h"

#include <qvariant.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

#include "../outputForm.ui.h"
/*
 *  Constructs a outputForm as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
outputForm::outputForm( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "outputForm" );

    outputFormRichEditBox = new QTextEdit( this, "outputFormRichEditBox" );
    outputFormRichEditBox->setGeometry( QRect( 0, 0, 270, 510 ) );
    QFont outputFormRichEditBox_font(  outputFormRichEditBox->font() );
    outputFormRichEditBox_font.setFamily( "Fixed [Misc]" );
    outputFormRichEditBox->setFont( outputFormRichEditBox_font ); 
    outputFormRichEditBox->setResizePolicy( QTextEdit::Manual );
    languageChange();
    resize( QSize(275, 539).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
outputForm::~outputForm()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void outputForm::languageChange()
{
    setCaption( tr( "Input/Output" ) );
}

