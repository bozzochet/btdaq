/****************************************************************************
** Form interface generated from reading ui file 'errordialog.ui'
**
** Created: Thu Oct 25 00:14:53 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QLabel;

class ErrorDialog : public QDialog
{
    Q_OBJECT

public:
    ErrorDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ErrorDialog();

    QPushButton* buttonOk;
    QLabel* errLabel;

public slots:
    virtual void setError( char * aa );
    virtual void setError( QString & aa );

protected:
    QGridLayout* ErrorDialogLayout;
    QHBoxLayout* Layout1;

protected slots:
    virtual void languageChange();

};

#endif // ERRORDIALOG_H
