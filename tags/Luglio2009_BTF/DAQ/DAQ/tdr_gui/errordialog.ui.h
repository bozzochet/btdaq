/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/



void ErrorDialog::setError(char* aa)
{
    QString bb(aa);
errLabel->setText(bb);
}

void ErrorDialog::setError(QString& aa)
{
errLabel->setText(aa);
}
