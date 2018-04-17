/****************************************************************************
** Form interface generated from reading ui file 'AboutForm.ui'
**
** Created by User Interface Compiler
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef ABOUTFORM_H
#define ABOUTFORM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTextEdit;
class QLabel;

class AboutForm : public QDialog
{
    Q_OBJECT

public:
    AboutForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~AboutForm();

    QTextEdit* aboutFormRichEdit;
    QLabel* QuickUsbLogo;

public slots:
    virtual void showForm( int x, int y, AboutForm * * ptr );
    virtual void closeEvent( QCloseEvent * event );

protected:

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // ABOUTFORM_H
