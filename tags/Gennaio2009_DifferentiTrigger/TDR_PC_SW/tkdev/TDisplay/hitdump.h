/****************************************************************************
** Form interface generated from reading ui file 'hitdump.ui'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.3   edited Nov 24 2003 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef HITDUMP_H
#define HITDUMP_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QGroupBox;
class QComboBox;
class QLabel;
class QSpinBox;
class QPushButton;
class QTextEdit;

class HitDump : public QWidget
{
    Q_OBJECT

public:
    HitDump( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~HitDump();

    QGroupBox* groupBox1;
    QComboBox* cbType;
    QComboBox* cbLayers;
    QLabel* textLabel1;
    QSpinBox* spinBox3;
    QPushButton* pbDClus;
    QTextEdit* teDump;

public slots:
    virtual void Update();

protected:
    QVBoxLayout* HitDumpLayout;
    QHBoxLayout* groupBox1Layout;
    QSpacerItem* spacer1;
    QHBoxLayout* layout3;

protected slots:
    virtual void languageChange();

private:
    void init();

};

#endif // HITDUMP_H
