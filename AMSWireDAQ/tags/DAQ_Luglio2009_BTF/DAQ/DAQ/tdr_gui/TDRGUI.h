/****************************************************************************
** Form interface generated from reading ui file 'TDRGUI.ui'
**
** Created: Thu Oct 25 00:14:53 2007
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DAQ0_H
#define DAQ0_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QPushButton;
class QTextEdit;
class QButtonGroup;
class QLineEdit;

class Daq0 : public QWidget
{
    Q_OBJECT

public:
    Daq0( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Daq0();

    QLabel* txLaTitle;
    QPushButton* pbGetLog;
    QPushButton* pbStopLog;
    QPushButton* pbNewLog;
    QTextEdit* textEdit2;
    QPushButton* pbUpdate;
    QPushButton* pbClear;
    QButtonGroup* buttonGroup2;
    QTextEdit* textEdCount;
    QPushButton* pbInit;
    QLabel* textLabel1;
    QLineEdit* leRun;
    QLabel* textLabel2;
    QLineEdit* leEvMax;
    QPushButton* pbStart;
    QPushButton* pbStop;
    QPushButton* pbCalib;
    QPushButton* pbKill;

    virtual bool CheckConn();

public slots:
    virtual bool pbGetLog_clicked();
    virtual void UpdateLog();
    virtual void pbClear_clicked();
    virtual void pbInit_clicked();
    virtual void pbStart_clicked();
    virtual void UpdateCount();
    virtual void pbStopLog_clicked();
    virtual void pbUpdate_clicked();

protected:
    QHBoxLayout* Daq0Layout;
    QVBoxLayout* layout13;
    QHBoxLayout* layout12;
    QHBoxLayout* layout11;
    QVBoxLayout* buttonGroup2Layout;
    QSpacerItem* spacer16;
    QHBoxLayout* layout6;
    QVBoxLayout* layout4;
    QVBoxLayout* layout5;

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // DAQ0_H
