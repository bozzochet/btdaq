/****************************************************************************
** Form interface generated from reading ui file 'outputForm.ui'
**
** Created by User Interface Compiler
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef OUTPUTFORM_H
#define OUTPUTFORM_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QTextEdit;

class outputForm : public QDialog
{
    Q_OBJECT

public:
    outputForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~outputForm();

    QTextEdit* outputFormRichEditBox;

    virtual QString toHex( int decimal, int length );

public slots:
    virtual void showForm( int x, int y, int height, outputForm * * ptr );
    virtual void setText( QString str );
    virtual void showByteData( unsigned char * theData, unsigned long dataLength );
    virtual void closeEvent( QCloseEvent * event );

protected:

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // OUTPUTFORM_H
