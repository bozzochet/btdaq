/****************************************************************************
** Form interface generated from reading ui file 'QuickUsbDiag.ui'
**
** Created by User Interface Compiler
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef QUICKUSBDISPLAY_H
#define QUICKUSBDISPLAY_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QLabel;
class QFrame;
class QLineEdit;
class QCheckBox;
class QTabWidget;
class QWidget;
class QGroupBox;
class QComboBox;
class QPushButton;
class QProgressBar;
class QTable;
class QButtonGroup;
class QTextEdit;

class QuickUsbDisplay : public QMainWindow
{
    Q_OBJECT

public:
    QuickUsbDisplay( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~QuickUsbDisplay();

    QLabel* textLabel1_5;
    QLabel* textLabel2_5;
    QFrame* line1;
    QLineEdit* intervalTextBox;
    QCheckBox* repeatLastButtonCheckBox;
    QTabWidget* PageControl;
    QWidget* tab;
    QGroupBox* groupBox1;
    QLabel* textLabel1;
    QLabel* textLabel2;
    QLabel* textLabel1_6;
    QLabel* textLabel2_6;
    QLabel* textLabel3_3;
    QComboBox* moduleComboBox;
    QLineEdit* stringDescriptorTextBox;
    QLineEdit* SerialNumberTextBox;
    QLineEdit* UsbSpeedTextBox;
    QLineEdit* HwVersionTextBox;
    QGroupBox* groupBox2;
    QPushButton* browseButton;
    QLineEdit* UploadFilenameTextBox;
    QComboBox* FpgaTypeComboBox;
    QProgressBar* progressBar;
    QPushButton* fpgaUploadButton;
    QWidget* tabd;
    QGroupBox* groupBox14;
    QLabel* textLabel3_2;
    QTable* CommandFlexGrid;
    QLabel* textLabel1_4;
    QLineEdit* CommandAddressTextBox;
    QLineEdit* CommandLengthTextBox;
    QLabel* textLabel2_4;
    QCheckBox* CommandAddressCheckBox;
    QPushButton* CommandWriteButton;
    QPushButton* CommandReadButton;
    QCheckBox* WordWideCheckBox;
    QGroupBox* groupBox14_2;
    QCheckBox* DataAddressCheckBox;
    QCheckBox* asyncCheckBox;
    QLabel* textLabel1_4_2;
    QLabel* textLabel2_4_2;
    QLabel* bufferCountLabel;
    QLabel* bufferSizeLabel;
    QLineEdit* bufferSizeTextBox;
    QLineEdit* bufferCountTextBox;
    QLineEdit* DataLengthTextBox;
    QLineEdit* DataAddressTextBox;
    QPushButton* DataCountButton;
    QLineEdit* ReadFileTextBox;
    QLineEdit* WriteFileTextBox;
    QPushButton* DataReadButton;
    QPushButton* ReadFileBrowseButton;
    QPushButton* WriteFileBrowseButton;
    QPushButton* DataWriteButton;
    QWidget* TabPage;
    QGroupBox* groupBox10;
    QGroupBox* groupBox11_3;
    QLabel* textLabel1_3_3;
    QLabel* textLabel2_3_3;
    QPushButton* PortEReadButton;
    QButtonGroup* PortEDirGroupBox;
    QCheckBox* checkBox13_3_5;
    QCheckBox* checkBox13_4_5;
    QCheckBox* checkBox13_5_2_5;
    QCheckBox* checkBox13_5_3_6;
    QCheckBox* checkBox13_5_3_2_5;
    QCheckBox* checkBox13_9;
    QCheckBox* checkBox13_2_5;
    QLineEdit* PortEDirBox;
    QCheckBox* checkBox13_5_7;
    QButtonGroup* PortEValueGroupBox;
    QCheckBox* checkBox13_3_6;
    QCheckBox* checkBox13_4_6;
    QCheckBox* checkBox13_5_2_6;
    QCheckBox* checkBox13_5_3_7;
    QCheckBox* checkBox13_2_6;
    QCheckBox* checkBox13_10;
    QCheckBox* checkBox13_5_3_2_6;
    QLineEdit* PortEValueBox;
    QCheckBox* checkBox13_5_8;
    QGroupBox* groupBox11;
    QLabel* textLabel2_3;
    QPushButton* PortAReadButton;
    QLabel* textLabel1_3;
    QButtonGroup* PortADirGroupBox;
    QCheckBox* checkBox13_5_3_2;
    QCheckBox* checkBox13_2;
    QCheckBox* checkBox13_3;
    QCheckBox* checkBox13_4;
    QCheckBox* checkBox13_5;
    QCheckBox* checkBox13_5_2;
    QLineEdit* PortADirBox;
    QCheckBox* checkBox13;
    QCheckBox* checkBox13_5_3;
    QButtonGroup* PortAValueGroupBox;
    QCheckBox* portACheckBox7;
    QCheckBox* portACheckBox4;
    QCheckBox* checkBox13_5_4;
    QCheckBox* checkBox13_5_2_2;
    QCheckBox* checkBox13_5_3_3;
    QCheckBox* checkBox13_5_3_2_2;
    QLineEdit* PortAValueBox;
    QCheckBox* portACheckBox6;
    QCheckBox* portACheckBox5;
    QGroupBox* groupBox11_4;
    QLabel* textLabel1_3_2;
    QLabel* textLabel2_3_2;
    QPushButton* PortCReadButton;
    QButtonGroup* PortCDirGroupBox;
    QCheckBox* checkBox13_9_2;
    QCheckBox* checkBox13_2_5_2;
    QCheckBox* checkBox13_3_5_2;
    QCheckBox* checkBox13_4_5_2;
    QCheckBox* checkBox13_5_7_2;
    QCheckBox* checkBox13_5_3_6_2;
    QCheckBox* checkBox13_5_3_2_5_2;
    QLineEdit* PortCDirBox;
    QCheckBox* checkBox13_5_2_5_2;
    QButtonGroup* PortCValueGroupBox;
    QCheckBox* checkBox13_5_3_7_2;
    QCheckBox* checkBox13_5_2_6_2;
    QCheckBox* checkBox13_5_8_2;
    QCheckBox* checkBox13_4_6_2;
    QCheckBox* checkBox13_3_6_2;
    QCheckBox* checkBox13_2_6_2;
    QCheckBox* checkBox13_10_2;
    QCheckBox* checkBox13_5_3_2_6_2;
    QLineEdit* PortCValueBox;
    QWidget* TabPage_2;
    QGroupBox* groupBox4;
    QLabel* textLabel2_2;
    QComboBox* RS232PortComboBox;
    QLabel* textLabel3;
    QPushButton* RS232FlushButton;
    QComboBox* RS232BaudRateComboBox;
    QGroupBox* groupBox5;
    QLineEdit* GetNumTextBox;
    QLabel* textLabel1_2;
    QTextEdit* RS232ReadTextBox;
    QPushButton* RS232ReadButton;
    QGroupBox* groupBox6;
    QTextEdit* RS232WriteTextBox;
    QPushButton* RS232WriteButton;
    QWidget* TabPage_3;
    QGroupBox* groupBox5_2_2;
    QLineEdit* I2cReadLengthTextBox;
    QPushButton* I2cReadButton;
    QTextEdit* I2cReadTextBox;
    QLabel* textLabel1_2_3_3;
    QLineEdit* I2cReadAddressTextBox;
    QLabel* textLabel1_2_3_2_4_2;
    QGroupBox* groupBox6_2_2;
    QTextEdit* I2cWriteTextBox;
    QPushButton* I2cWriteButton;
    QLineEdit* I2cWriteAddressTextBox;
    QLabel* textLabel1_2_3_2_4;
    QWidget* TabPage_4;
    QGroupBox* groupBox5_2;
    QLabel* textLabel1_2_3_2_2_2;
    QLineEdit* SpiReadLengthTextBox;
    QComboBox* SpiReadPortNumComboBox;
    QPushButton* SpiReadButton;
    QTextEdit* SpiReadTextBox;
    QLabel* textLabel1_2_3;
    QGroupBox* groupBox6_2;
    QPushButton* SpiWriteButton;
    QPushButton* SpiWriteReadButton;
    QTextEdit* SpiWriteTextBox;
    QLabel* textLabel1_2_3_2_2_2_2;
    QComboBox* SpiWritePortNumComboBox;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QPopupMenu *helpMenu;
    QAction* fileExitAction;
    QAction* helpAboutAction;

    virtual bool foundNewModules( const char * str );
    virtual void DisplayError( const char * function, int index );

public slots:
    virtual void UpdateModuleComboBox();
    virtual void PageControl_currentChanged( QWidget * );
    virtual void fpgaUploadButton_clicked();
    virtual void moduleComboBox_activated( const QString & str );
    virtual void browseButton_clicked();
    virtual void WordWideCheckBox_clicked();
    virtual void CommandReadButton_clicked();
    virtual void CommandWriteButton_clicked();
    virtual void CommandLengthTextBox_textChanged( const QString & );
    virtual void AsyncDataRead();
    virtual void AsyncDataWrite();
    virtual void DataReadButton_clicked();
    virtual void DataWriteButton_clicked();
    virtual void DataCountButton_clicked();
    virtual void PortAReadButton_clicked();
    virtual void PortCReadButton_clicked();
    virtual void PortEReadButton_clicked();
    virtual void ReadPortADirBits();
    virtual void ReadPortCDirBits();
    virtual void ReadPortEDirBits();
    virtual void PortADirGroupBox_clicked( int buttonID );
    virtual void PortAValueGroupBox_clicked( int buttonID );
    virtual void PortCDirGroupBox_clicked( int buttonID );
    virtual void PortCValueGroupBox_clicked( int buttonID );
    virtual void PortEDirGroupBox_clicked( int buttonID );
    virtual void PortEValueGroupBox_clicked( int buttonID );
    virtual void PortAValueBox_returnPressed();
    virtual void PortCValueBox_returnPressed();
    virtual void PortEValueBox_returnPressed();
    virtual void PortADirBox_returnPressed();
    virtual void PortCDirBox_returnPressed();
    virtual void PortEDirBox_returnPressed();
    virtual void RS232WriteButton_clicked();
    virtual void RS232ReadButton_clicked();
    virtual void RS232PortComboBox_activated( int );
    virtual void RS232FlushButton_clicked();
    virtual void RS232BaudRateComboBox_activated( const QString & str );
    virtual void SpiReadButton_clicked();
    virtual void SpiWriteButton_clicked();
    virtual void SpiWriteReadButton_clicked();
    virtual void I2cReadButton_clicked();
    virtual void I2cWriteButton_clicked();
    virtual void cleanUp();
    virtual void resetTimers( QPushButton * button );
    virtual void repeatLastButtonCheckBox_clicked();
    virtual void intervalTextBox_textChanged( const QString & str );
    virtual void pressButton();
    virtual void pause();
    virtual void HelpAbout_activated();
    virtual void aboutOnTop();
    virtual void FileExit_activated();
    virtual void closeEvent( QCloseEvent * event );
    virtual void asyncCheckBox_stateChanged( int );
    virtual void ReadFileBrowseButton_clicked();
    virtual void WriteFileBrowseButton_clicked();

signals:
    void clicked();

protected:

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;

    void init();

};

#endif // QUICKUSBDISPLAY_H
