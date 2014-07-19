/****************************************************************************
** Form interface generated from reading ui file 'ExitDialog.ui'
**
** Created: Thu Oct 25 00:14:53 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MYDIALOG_H
#define MYDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QPushButton;

class MyDialog : public QDialog
{
    Q_OBJECT

public:
    MyDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~MyDialog();

    QLabel* textLabel2;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;

protected:
    QVBoxLayout* MyDialogLayout;
    QVBoxLayout* layout64;
    QVBoxLayout* layout33;
    QSpacerItem* spacer15;
    QHBoxLayout* layout63;
    QSpacerItem* spacer30;
    QSpacerItem* Horizontal_Spacing2;
    QSpacerItem* spacer30_2;

protected slots:
    virtual void languageChange();

};

#endif // MYDIALOG_H
