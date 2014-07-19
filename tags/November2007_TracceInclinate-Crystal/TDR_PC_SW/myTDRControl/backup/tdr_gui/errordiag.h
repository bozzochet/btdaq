/****************************************************************************
** Form interface generated from reading ui file 'errordiag.ui'
**
** Created: Thu Oct 25 00:14:53 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ERRORDIAG_H
#define ERRORDIAG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QPushButton;

class ErrorDiag : public QDialog
{
    Q_OBJECT

public:
    ErrorDiag( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ErrorDiag();

    QLabel* textLabel3;
    QPushButton* pushButton68;

public slots:
    virtual void pushButton68_clicked();

protected:
    QGridLayout* ErrorDiagLayout;
    QSpacerItem* spacer36;
    QSpacerItem* spacer37;

protected slots:
    virtual void languageChange();

};

#endif // ERRORDIAG_H
