/****************************************************************************
** Form implementation generated from reading ui file 'QuickUsbDiag.ui'
**
** Created by User Interface Compiler
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "QuickUsbDiag.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qframe.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <qtable.h>
#include <qbuttongroup.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>

#include "../QuickUsbDiag.ui.h"
static const unsigned char image0_data[] = { 
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x73, 0x7a, 0x7a, 0xf4, 0x00, 0x00, 0x01,
    0x53, 0x49, 0x44, 0x41, 0x54, 0x58, 0x85, 0xbd, 0x96, 0x41, 0x8a, 0xc3,
    0x30, 0x0c, 0x45, 0xbf, 0x43, 0x16, 0xb9, 0x8f, 0xa1, 0x8b, 0xe4, 0x5c,
    0x5d, 0x0d, 0xc3, 0x90, 0xb6, 0x74, 0xd3, 0x5e, 0xab, 0xb3, 0x08, 0xf8,
    0x3e, 0x5a, 0x18, 0xdc, 0x45, 0x30, 0x34, 0x6e, 0xec, 0x48, 0x89, 0x6d,
    0x41, 0x30, 0x82, 0x20, 0xe5, 0x49, 0xdf, 0x52, 0x94, 0x99, 0x0c, 0x60,
    0x01, 0xb4, 0x00, 0x59, 0x42, 0xd7, 0x76, 0x2c, 0x5f, 0xf7, 0xda, 0xe1,
    0x80, 0x99, 0xc9, 0x28, 0xb2, 0x84, 0x86, 0x2c, 0x2d, 0x82, 0x73, 0x7d,
    0x00, 0x70, 0x4e, 0xf6, 0x5c, 0xc7, 0x65, 0xf2, 0xae, 0xed, 0xd0, 0xf8,
    0xa0, 0x9e, 0x8c, 0xe3, 0xeb, 0x93, 0x76, 0x4e, 0xc8, 0xff, 0xbc, 0x03,
    0xbf, 0x7f, 0xcb, 0xe4, 0xb0, 0x98, 0x2b, 0x20, 0x21, 0x27, 0x4b, 0xe2,
    0x72, 0xdf, 0x2e, 0xc0, 0xf9, 0x67, 0x99, 0xdc, 0xc7, 0x6b, 0x24, 0xe4,
    0x64, 0x09, 0x43, 0x3f, 0x88, 0xe8, 0x63, 0xe4, 0x3e, 0x9e, 0x58, 0x03,
    0xb9, 0xc8, 0x77, 0x69, 0x40, 0xd2, 0xfb, 0x2d, 0xf2, 0x7d, 0x1a, 0xc8,
    0x48, 0x2e, 0xd6, 0x00, 0xb7, 0xf7, 0x5c, 0x72, 0xb1, 0x06, 0x72, 0x93,
    0x8b, 0x34, 0xc0, 0xe9, 0xbd, 0x94, 0x5c, 0xa4, 0x81, 0x12, 0xe4, 0x6c,
    0x0d, 0x6c, 0xf5, 0x7e, 0x2f, 0x39, 0x5b, 0x03, 0xa5, 0xc8, 0xbd, 0xaf,
    0xcc, 0x64, 0xa2, 0x5b, 0x2f, 0xd5, 0xfb, 0xe7, 0x7d, 0x4e, 0x1e, 0x5a,
    0x58, 0x89, 0xad, 0xad, 0xda, 0x7e, 0x7e, 0x59, 0x78, 0xa6, 0x8c, 0x2c,
    0xf0, 0x18, 0x01, 0xff, 0x56, 0xd8, 0x06, 0xee, 0xa9, 0x62, 0xff, 0x03,
    0xba, 0xe7, 0x4d, 0x3d, 0x5f, 0x09, 0x29, 0xb9, 0xf7, 0xa3, 0x1a, 0x00,
    0x00, 0xa5, 0xe6, 0x27, 0x66, 0xc5, 0x35, 0x90, 0xaa, 0xc4, 0x51, 0x72,
    0xd6, 0x1c, 0x88, 0x25, 0xcf, 0x41, 0xce, 0x9a, 0x03, 0xb1, 0x9e, 0x1f,
    0xb9, 0xf7, 0xec, 0x39, 0xb0, 0x76, 0x05, 0x73, 0x92, 0x6f, 0xee, 0x82,
    0xd2, 0xe4, 0x49, 0x0d, 0x84, 0xe3, 0xb7, 0x04, 0x79, 0x5a, 0x03, 0x15,
    0xc8, 0xa3, 0x1a, 0xf8, 0xec, 0x7d, 0x49, 0xf2, 0xa8, 0x06, 0x6a, 0x91,
    0xaf, 0x6a, 0xc0, 0xdf, 0xfb, 0x1a, 0xe4, 0xab, 0x1a, 0xa8, 0x49, 0xfe,
    0xa5, 0x01, 0x7d, 0xd2, 0xee, 0x3a, 0xd6, 0x23, 0xff, 0xda, 0x05, 0x43,
    0x3f, 0x38, 0xe0, 0xf8, 0x6c, 0x97, 0xfa, 0xea, 0xf5, 0xff, 0x12, 0xed,
    0xef, 0xdc, 0xe7, 0x1b, 0xdd, 0xeb, 0x6a, 0x9f, 0xb4, 0xa2, 0x6f, 0x3f,
    0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
};


/*
 *  Constructs a QuickUsbDisplay as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
QuickUsbDisplay::QuickUsbDisplay( QWidget* parent, const char* name, WFlags fl )
    : QMainWindow( parent, name, fl )
{
    (void)statusBar();
    QImage img;
    img.loadFromData( image0_data, sizeof( image0_data ), "PNG" );
    image0 = img;
    if ( !name )
	setName( "QuickUsbDisplay" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 21, 115 ) );
    setMaximumSize( QSize( 450, 525 ) );
    setIcon( image0 );
    setFocusPolicy( QMainWindow::StrongFocus );
    setDockWindowsMovable( FALSE );
    setCentralWidget( new QWidget( this, "qt_central_widget" ) );

    textLabel1_5 = new QLabel( centralWidget(), "textLabel1_5" );
    textLabel1_5->setGeometry( QRect( 180, 443, 36, 16 ) );
    textLabel1_5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel1_5->sizePolicy().hasHeightForWidth() ) );

    textLabel2_5 = new QLabel( centralWidget(), "textLabel2_5" );
    textLabel2_5->setGeometry( QRect( 280, 442, 16, 16 ) );
    textLabel2_5->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, textLabel2_5->sizePolicy().hasHeightForWidth() ) );

    line1 = new QFrame( centralWidget(), "line1" );
    line1->setGeometry( QRect( 0, 465, 460, 16 ) );
    line1->setFrameShape( QFrame::HLine );
    line1->setFrameShadow( QFrame::Sunken );
    line1->setFrameShape( QFrame::HLine );

    intervalTextBox = new QLineEdit( centralWidget(), "intervalTextBox" );
    intervalTextBox->setGeometry( QRect( 225, 440, 45, 22 ) );
    intervalTextBox->setDragEnabled( TRUE );

    repeatLastButtonCheckBox = new QCheckBox( centralWidget(), "repeatLastButtonCheckBox" );
    repeatLastButtonCheckBox->setGeometry( QRect( 35, 440, 130, 22 ) );

    PageControl = new QTabWidget( centralWidget(), "PageControl" );
    PageControl->setGeometry( QRect( 5, 5, 435, 425 ) );
    PageControl->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, PageControl->sizePolicy().hasHeightForWidth() ) );
    PageControl->setMaximumSize( QSize( 450, 500 ) );
    PageControl->setTabShape( QTabWidget::Rounded );

    tab = new QWidget( PageControl, "tab" );

    groupBox1 = new QGroupBox( tab, "groupBox1" );
    groupBox1->setGeometry( QRect( 10, 10, 410, 165 ) );

    textLabel1 = new QLabel( groupBox1, "textLabel1" );
    textLabel1->setGeometry( QRect( 15, 15, 57, 20 ) );

    textLabel2 = new QLabel( groupBox1, "textLabel2" );
    textLabel2->setGeometry( QRect( 15, 45, 60, 20 ) );

    textLabel1_6 = new QLabel( groupBox1, "textLabel1_6" );
    textLabel1_6->setGeometry( QRect( 15, 75, 69, 20 ) );

    textLabel2_6 = new QLabel( groupBox1, "textLabel2_6" );
    textLabel2_6->setGeometry( QRect( 15, 105, 70, 20 ) );

    textLabel3_3 = new QLabel( groupBox1, "textLabel3_3" );
    textLabel3_3->setGeometry( QRect( 15, 135, 70, 20 ) );

    moduleComboBox = new QComboBox( FALSE, groupBox1, "moduleComboBox" );
    moduleComboBox->setGeometry( QRect( 95, 15, 130, 20 ) );

    stringDescriptorTextBox = new QLineEdit( groupBox1, "stringDescriptorTextBox" );
    stringDescriptorTextBox->setGeometry( QRect( 95, 45, 300, 20 ) );
    stringDescriptorTextBox->setCursor( QCursor( 0 ) );
    stringDescriptorTextBox->setReadOnly( TRUE );

    SerialNumberTextBox = new QLineEdit( groupBox1, "SerialNumberTextBox" );
    SerialNumberTextBox->setGeometry( QRect( 95, 75, 145, 20 ) );
    SerialNumberTextBox->setReadOnly( TRUE );

    UsbSpeedTextBox = new QLineEdit( groupBox1, "UsbSpeedTextBox" );
    UsbSpeedTextBox->setGeometry( QRect( 95, 105, 300, 20 ) );
    UsbSpeedTextBox->setReadOnly( TRUE );

    HwVersionTextBox = new QLineEdit( groupBox1, "HwVersionTextBox" );
    HwVersionTextBox->setGeometry( QRect( 95, 135, 300, 20 ) );
    HwVersionTextBox->setReadOnly( TRUE );

    groupBox2 = new QGroupBox( tab, "groupBox2" );
    groupBox2->setGeometry( QRect( 10, 185, 410, 135 ) );

    browseButton = new QPushButton( groupBox2, "browseButton" );
    browseButton->setGeometry( QRect( 10, 60, 65, 25 ) );

    UploadFilenameTextBox = new QLineEdit( groupBox2, "UploadFilenameTextBox" );
    UploadFilenameTextBox->setGeometry( QRect( 85, 60, 275, 21 ) );
    UploadFilenameTextBox->setFrameShape( QLineEdit::LineEditPanel );
    UploadFilenameTextBox->setFrameShadow( QLineEdit::Sunken );

    FpgaTypeComboBox = new QComboBox( FALSE, groupBox2, "FpgaTypeComboBox" );
    FpgaTypeComboBox->setGeometry( QRect( 85, 100, 245, 20 ) );
    FpgaTypeComboBox->setEditable( FALSE );

    progressBar = new QProgressBar( groupBox2, "progressBar" );
    progressBar->setGeometry( QRect( 85, 20, 310, 22 ) );

    fpgaUploadButton = new QPushButton( groupBox2, "fpgaUploadButton" );
    fpgaUploadButton->setGeometry( QRect( 10, 20, 65, 25 ) );
    PageControl->insertTab( tab, QString::fromLatin1("") );

    tabd = new QWidget( PageControl, "tabd" );

    groupBox14 = new QGroupBox( tabd, "groupBox14" );
    groupBox14->setGeometry( QRect( 10, 5, 410, 190 ) );

    textLabel3_2 = new QLabel( groupBox14, "textLabel3_2" );
    textLabel3_2->setGeometry( QRect( 190, 15, 99, 16 ) );

    CommandFlexGrid = new QTable( groupBox14, "CommandFlexGrid" );
    CommandFlexGrid->setGeometry( QRect( 190, 35, 180, 145 ) );
    CommandFlexGrid->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)7, 0, 0, CommandFlexGrid->sizePolicy().hasHeightForWidth() ) );
    CommandFlexGrid->setMinimumSize( QSize( 0, 0 ) );
    CommandFlexGrid->setAcceptDrops( FALSE );
    CommandFlexGrid->setLineWidth( 2 );
    CommandFlexGrid->setMargin( 0 );
    CommandFlexGrid->setMidLineWidth( 0 );
    CommandFlexGrid->setResizePolicy( QTable::Default );
    CommandFlexGrid->setNumRows( 8 );
    CommandFlexGrid->setNumCols( 2 );
    CommandFlexGrid->setShowGrid( TRUE );

    textLabel1_4 = new QLabel( groupBox14, "textLabel1_4" );
    textLabel1_4->setGeometry( QRect( 20, 20, 46, 16 ) );

    CommandAddressTextBox = new QLineEdit( groupBox14, "CommandAddressTextBox" );
    CommandAddressTextBox->setGeometry( QRect( 80, 15, 45, 22 ) );

    CommandLengthTextBox = new QLineEdit( groupBox14, "CommandLengthTextBox" );
    CommandLengthTextBox->setGeometry( QRect( 80, 45, 45, 20 ) );

    textLabel2_4 = new QLabel( groupBox14, "textLabel2_4" );
    textLabel2_4->setGeometry( QRect( 20, 45, 40, 16 ) );

    CommandAddressCheckBox = new QCheckBox( groupBox14, "CommandAddressCheckBox" );
    CommandAddressCheckBox->setGeometry( QRect( 20, 120, 92, 20 ) );
    CommandAddressCheckBox->setChecked( TRUE );

    CommandWriteButton = new QPushButton( groupBox14, "CommandWriteButton" );
    CommandWriteButton->setGeometry( QRect( 105, 75, 65, 25 ) );

    CommandReadButton = new QPushButton( groupBox14, "CommandReadButton" );
    CommandReadButton->setGeometry( QRect( 20, 75, 65, 25 ) );

    WordWideCheckBox = new QCheckBox( tabd, "WordWideCheckBox" );
    WordWideCheckBox->setGeometry( QRect( 25, 369, 92, 20 ) );
    WordWideCheckBox->setChecked( TRUE );

    groupBox14_2 = new QGroupBox( tabd, "groupBox14_2" );
    groupBox14_2->setGeometry( QRect( 10, 200, 410, 169 ) );

    DataAddressCheckBox = new QCheckBox( groupBox14_2, "DataAddressCheckBox" );
    DataAddressCheckBox->setGeometry( QRect( 20, 140, 92, 20 ) );
    DataAddressCheckBox->setChecked( TRUE );

    asyncCheckBox = new QCheckBox( groupBox14_2, "asyncCheckBox" );
    asyncCheckBox->setGeometry( QRect( 125, 140, 63, 20 ) );
    asyncCheckBox->setChecked( FALSE );

    textLabel1_4_2 = new QLabel( groupBox14_2, "textLabel1_4_2" );
    textLabel1_4_2->setGeometry( QRect( 20, 20, 46, 16 ) );

    textLabel2_4_2 = new QLabel( groupBox14_2, "textLabel2_4_2" );
    textLabel2_4_2->setGeometry( QRect( 20, 45, 40, 16 ) );

    bufferCountLabel = new QLabel( groupBox14_2, "bufferCountLabel" );
    bufferCountLabel->setGeometry( QRect( 20, 70, 50, 16 ) );

    bufferSizeLabel = new QLabel( groupBox14_2, "bufferSizeLabel" );
    bufferSizeLabel->setGeometry( QRect( 20, 90, 50, 16 ) );

    bufferSizeTextBox = new QLineEdit( groupBox14_2, "bufferSizeTextBox" );
    bufferSizeTextBox->setGeometry( QRect( 75, 90, 55, 22 ) );

    bufferCountTextBox = new QLineEdit( groupBox14_2, "bufferCountTextBox" );
    bufferCountTextBox->setGeometry( QRect( 75, 65, 55, 22 ) );
    bufferCountTextBox->setMaxLength( 6 );

    DataLengthTextBox = new QLineEdit( groupBox14_2, "DataLengthTextBox" );
    DataLengthTextBox->setGeometry( QRect( 75, 40, 55, 22 ) );

    DataAddressTextBox = new QLineEdit( groupBox14_2, "DataAddressTextBox" );
    DataAddressTextBox->setGeometry( QRect( 75, 15, 55, 22 ) );

    DataCountButton = new QPushButton( groupBox14_2, "DataCountButton" );
    DataCountButton->setGeometry( QRect( 135, 85, 45, 25 ) );

    ReadFileTextBox = new QLineEdit( groupBox14_2, "ReadFileTextBox" );
    ReadFileTextBox->setGeometry( QRect( 190, 15, 150, 25 ) );

    WriteFileTextBox = new QLineEdit( groupBox14_2, "WriteFileTextBox" );
    WriteFileTextBox->setGeometry( QRect( 190, 50, 150, 25 ) );

    DataReadButton = new QPushButton( groupBox14_2, "DataReadButton" );
    DataReadButton->setGeometry( QRect( 135, 15, 45, 25 ) );

    ReadFileBrowseButton = new QPushButton( groupBox14_2, "ReadFileBrowseButton" );
    ReadFileBrowseButton->setGeometry( QRect( 345, 15, 50, 25 ) );

    WriteFileBrowseButton = new QPushButton( groupBox14_2, "WriteFileBrowseButton" );
    WriteFileBrowseButton->setGeometry( QRect( 345, 50, 50, 25 ) );

    DataWriteButton = new QPushButton( groupBox14_2, "DataWriteButton" );
    DataWriteButton->setGeometry( QRect( 135, 50, 45, 25 ) );
    PageControl->insertTab( tabd, QString::fromLatin1("") );

    TabPage = new QWidget( PageControl, "TabPage" );

    groupBox10 = new QGroupBox( TabPage, "groupBox10" );
    groupBox10->setGeometry( QRect( 10, 5, 410, 380 ) );

    groupBox11_3 = new QGroupBox( groupBox10, "groupBox11_3" );
    groupBox11_3->setGeometry( QRect( 10, 255, 380, 106 ) );

    textLabel1_3_3 = new QLabel( groupBox11_3, "textLabel1_3_3" );
    textLabel1_3_3->setGeometry( QRect( 20, 30, 20, 16 ) );

    textLabel2_3_3 = new QLabel( groupBox11_3, "textLabel2_3_3" );
    textLabel2_3_3->setGeometry( QRect( 20, 55, 35, 16 ) );

    PortEReadButton = new QPushButton( groupBox11_3, "PortEReadButton" );
    PortEReadButton->setGeometry( QRect( 290, 50, 65, 25 ) );

    PortEDirGroupBox = new QButtonGroup( groupBox11_3, "PortEDirGroupBox" );
    PortEDirGroupBox->setGeometry( QRect( 70, 25, 215, 25 ) );
    PortEDirGroupBox->setPaletteBackgroundColor( QColor( 220, 220, 220 ) );
    PortEDirGroupBox->setFrameShape( QButtonGroup::NoFrame );
    PortEDirGroupBox->setFlat( FALSE );

    checkBox13_3_5 = new QCheckBox( PortEDirGroupBox, "checkBox13_3_5" );
    checkBox13_3_5->setGeometry( QRect( 45, 5, 16, 20 ) );
    PortEDirGroupBox->insert( checkBox13_3_5, 5 );

    checkBox13_4_5 = new QCheckBox( PortEDirGroupBox, "checkBox13_4_5" );
    checkBox13_4_5->setGeometry( QRect( 65, 5, 16, 20 ) );
    PortEDirGroupBox->insert( checkBox13_4_5, 4 );

    checkBox13_5_2_5 = new QCheckBox( PortEDirGroupBox, "checkBox13_5_2_5" );
    checkBox13_5_2_5->setGeometry( QRect( 105, 5, 16, 20 ) );
    PortEDirGroupBox->insert( checkBox13_5_2_5, 2 );

    checkBox13_5_3_6 = new QCheckBox( PortEDirGroupBox, "checkBox13_5_3_6" );
    checkBox13_5_3_6->setGeometry( QRect( 125, 5, 16, 20 ) );
    PortEDirGroupBox->insert( checkBox13_5_3_6, 1 );

    checkBox13_5_3_2_5 = new QCheckBox( PortEDirGroupBox, "checkBox13_5_3_2_5" );
    checkBox13_5_3_2_5->setGeometry( QRect( 145, 5, 16, 20 ) );
    PortEDirGroupBox->insert( checkBox13_5_3_2_5, 0 );

    checkBox13_9 = new QCheckBox( PortEDirGroupBox, "checkBox13_9" );
    checkBox13_9->setGeometry( QRect( 5, 5, 16, 20 ) );
    PortEDirGroupBox->insert( checkBox13_9, 7 );

    checkBox13_2_5 = new QCheckBox( PortEDirGroupBox, "checkBox13_2_5" );
    checkBox13_2_5->setGeometry( QRect( 25, 5, 16, 20 ) );
    PortEDirGroupBox->insert( checkBox13_2_5, 6 );

    PortEDirBox = new QLineEdit( PortEDirGroupBox, "PortEDirBox" );
    PortEDirBox->setGeometry( QRect( 165, 5, 45, 22 ) );
    PortEDirBox->setReadOnly( FALSE );

    checkBox13_5_7 = new QCheckBox( PortEDirGroupBox, "checkBox13_5_7" );
    checkBox13_5_7->setGeometry( QRect( 85, 5, 16, 20 ) );
    PortEDirGroupBox->insert( checkBox13_5_7, 3 );

    PortEValueGroupBox = new QButtonGroup( groupBox11_3, "PortEValueGroupBox" );
    PortEValueGroupBox->setGeometry( QRect( 70, 50, 215, 25 ) );
    PortEValueGroupBox->setPaletteBackgroundColor( QColor( 220, 220, 220 ) );
    PortEValueGroupBox->setFrameShape( QButtonGroup::NoFrame );
    PortEValueGroupBox->setFlat( FALSE );

    checkBox13_3_6 = new QCheckBox( PortEValueGroupBox, "checkBox13_3_6" );
    checkBox13_3_6->setGeometry( QRect( 45, 5, 16, 20 ) );
    PortEValueGroupBox->insert( checkBox13_3_6, 5 );

    checkBox13_4_6 = new QCheckBox( PortEValueGroupBox, "checkBox13_4_6" );
    checkBox13_4_6->setGeometry( QRect( 65, 5, 16, 20 ) );
    PortEValueGroupBox->insert( checkBox13_4_6, 4 );

    checkBox13_5_2_6 = new QCheckBox( PortEValueGroupBox, "checkBox13_5_2_6" );
    checkBox13_5_2_6->setGeometry( QRect( 105, 5, 16, 20 ) );
    PortEValueGroupBox->insert( checkBox13_5_2_6, 2 );

    checkBox13_5_3_7 = new QCheckBox( PortEValueGroupBox, "checkBox13_5_3_7" );
    checkBox13_5_3_7->setGeometry( QRect( 125, 5, 16, 20 ) );
    PortEValueGroupBox->insert( checkBox13_5_3_7, 1 );

    checkBox13_2_6 = new QCheckBox( PortEValueGroupBox, "checkBox13_2_6" );
    checkBox13_2_6->setGeometry( QRect( 25, 5, 16, 20 ) );
    PortEValueGroupBox->insert( checkBox13_2_6, 6 );

    checkBox13_10 = new QCheckBox( PortEValueGroupBox, "checkBox13_10" );
    checkBox13_10->setGeometry( QRect( 5, 5, 16, 20 ) );
    PortEValueGroupBox->insert( checkBox13_10, 7 );

    checkBox13_5_3_2_6 = new QCheckBox( PortEValueGroupBox, "checkBox13_5_3_2_6" );
    checkBox13_5_3_2_6->setGeometry( QRect( 145, 5, 16, 20 ) );
    PortEValueGroupBox->insert( checkBox13_5_3_2_6, 0 );

    PortEValueBox = new QLineEdit( PortEValueGroupBox, "PortEValueBox" );
    PortEValueBox->setGeometry( QRect( 165, 5, 45, 22 ) );
    PortEValueBox->setDragEnabled( TRUE );

    checkBox13_5_8 = new QCheckBox( PortEValueGroupBox, "checkBox13_5_8" );
    checkBox13_5_8->setGeometry( QRect( 85, 5, 16, 20 ) );
    PortEValueGroupBox->insert( checkBox13_5_8, 3 );

    groupBox11 = new QGroupBox( groupBox10, "groupBox11" );
    groupBox11->setGeometry( QRect( 10, 20, 380, 106 ) );

    textLabel2_3 = new QLabel( groupBox11, "textLabel2_3" );
    textLabel2_3->setGeometry( QRect( 20, 55, 35, 16 ) );

    PortAReadButton = new QPushButton( groupBox11, "PortAReadButton" );
    PortAReadButton->setGeometry( QRect( 290, 50, 65, 25 ) );

    textLabel1_3 = new QLabel( groupBox11, "textLabel1_3" );
    textLabel1_3->setGeometry( QRect( 20, 30, 20, 16 ) );

    PortADirGroupBox = new QButtonGroup( groupBox11, "PortADirGroupBox" );
    PortADirGroupBox->setGeometry( QRect( 70, 25, 215, 25 ) );
    PortADirGroupBox->setPaletteBackgroundColor( QColor( 220, 220, 220 ) );
    PortADirGroupBox->setFrameShape( QButtonGroup::NoFrame );
    PortADirGroupBox->setFlat( FALSE );

    checkBox13_5_3_2 = new QCheckBox( PortADirGroupBox, "checkBox13_5_3_2" );
    checkBox13_5_3_2->setGeometry( QRect( 145, 5, 16, 20 ) );
    PortADirGroupBox->insert( checkBox13_5_3_2, 0 );

    checkBox13_2 = new QCheckBox( PortADirGroupBox, "checkBox13_2" );
    checkBox13_2->setGeometry( QRect( 25, 5, 16, 20 ) );
    PortADirGroupBox->insert( checkBox13_2, 6 );

    checkBox13_3 = new QCheckBox( PortADirGroupBox, "checkBox13_3" );
    checkBox13_3->setGeometry( QRect( 45, 5, 16, 20 ) );
    PortADirGroupBox->insert( checkBox13_3, 5 );

    checkBox13_4 = new QCheckBox( PortADirGroupBox, "checkBox13_4" );
    checkBox13_4->setGeometry( QRect( 65, 5, 16, 20 ) );
    PortADirGroupBox->insert( checkBox13_4, 4 );

    checkBox13_5 = new QCheckBox( PortADirGroupBox, "checkBox13_5" );
    checkBox13_5->setGeometry( QRect( 85, 5, 16, 20 ) );
    PortADirGroupBox->insert( checkBox13_5, 3 );

    checkBox13_5_2 = new QCheckBox( PortADirGroupBox, "checkBox13_5_2" );
    checkBox13_5_2->setGeometry( QRect( 105, 5, 16, 20 ) );
    PortADirGroupBox->insert( checkBox13_5_2, 2 );

    PortADirBox = new QLineEdit( PortADirGroupBox, "PortADirBox" );
    PortADirBox->setGeometry( QRect( 165, 5, 45, 22 ) );
    PortADirBox->setReadOnly( FALSE );

    checkBox13 = new QCheckBox( PortADirGroupBox, "checkBox13" );
    checkBox13->setGeometry( QRect( 5, 5, 16, 20 ) );
    PortADirGroupBox->insert( checkBox13, 7 );

    checkBox13_5_3 = new QCheckBox( PortADirGroupBox, "checkBox13_5_3" );
    checkBox13_5_3->setGeometry( QRect( 125, 5, 16, 20 ) );
    PortADirGroupBox->insert( checkBox13_5_3, 1 );

    PortAValueGroupBox = new QButtonGroup( groupBox11, "PortAValueGroupBox" );
    PortAValueGroupBox->setGeometry( QRect( 70, 50, 215, 25 ) );
    PortAValueGroupBox->setPaletteBackgroundColor( QColor( 220, 220, 220 ) );
    PortAValueGroupBox->setFrameShape( QButtonGroup::NoFrame );
    PortAValueGroupBox->setFlat( FALSE );

    portACheckBox7 = new QCheckBox( PortAValueGroupBox, "portACheckBox7" );
    portACheckBox7->setGeometry( QRect( 5, 5, 16, 20 ) );
    PortAValueGroupBox->insert( portACheckBox7, 7 );

    portACheckBox4 = new QCheckBox( PortAValueGroupBox, "portACheckBox4" );
    portACheckBox4->setGeometry( QRect( 65, 5, 16, 20 ) );
    PortAValueGroupBox->insert( portACheckBox4, 4 );

    checkBox13_5_4 = new QCheckBox( PortAValueGroupBox, "checkBox13_5_4" );
    checkBox13_5_4->setGeometry( QRect( 85, 5, 16, 20 ) );
    PortAValueGroupBox->insert( checkBox13_5_4, 3 );

    checkBox13_5_2_2 = new QCheckBox( PortAValueGroupBox, "checkBox13_5_2_2" );
    checkBox13_5_2_2->setGeometry( QRect( 105, 5, 16, 20 ) );
    PortAValueGroupBox->insert( checkBox13_5_2_2, 2 );

    checkBox13_5_3_3 = new QCheckBox( PortAValueGroupBox, "checkBox13_5_3_3" );
    checkBox13_5_3_3->setGeometry( QRect( 125, 5, 16, 20 ) );
    PortAValueGroupBox->insert( checkBox13_5_3_3, 1 );

    checkBox13_5_3_2_2 = new QCheckBox( PortAValueGroupBox, "checkBox13_5_3_2_2" );
    checkBox13_5_3_2_2->setGeometry( QRect( 145, 5, 16, 20 ) );
    PortAValueGroupBox->insert( checkBox13_5_3_2_2, 0 );

    PortAValueBox = new QLineEdit( PortAValueGroupBox, "PortAValueBox" );
    PortAValueBox->setGeometry( QRect( 165, 5, 45, 22 ) );
    PortAValueBox->setReadOnly( FALSE );

    portACheckBox6 = new QCheckBox( PortAValueGroupBox, "portACheckBox6" );
    portACheckBox6->setGeometry( QRect( 25, 5, 16, 20 ) );
    PortAValueGroupBox->insert( portACheckBox6, 6 );

    portACheckBox5 = new QCheckBox( PortAValueGroupBox, "portACheckBox5" );
    portACheckBox5->setGeometry( QRect( 45, 5, 16, 20 ) );
    PortAValueGroupBox->insert( portACheckBox5, 5 );

    groupBox11_4 = new QGroupBox( groupBox10, "groupBox11_4" );
    groupBox11_4->setGeometry( QRect( 10, 135, 380, 106 ) );

    textLabel1_3_2 = new QLabel( groupBox11_4, "textLabel1_3_2" );
    textLabel1_3_2->setGeometry( QRect( 20, 30, 20, 16 ) );

    textLabel2_3_2 = new QLabel( groupBox11_4, "textLabel2_3_2" );
    textLabel2_3_2->setGeometry( QRect( 20, 55, 35, 16 ) );

    PortCReadButton = new QPushButton( groupBox11_4, "PortCReadButton" );
    PortCReadButton->setGeometry( QRect( 290, 50, 65, 25 ) );

    PortCDirGroupBox = new QButtonGroup( groupBox11_4, "PortCDirGroupBox" );
    PortCDirGroupBox->setGeometry( QRect( 70, 25, 215, 25 ) );
    PortCDirGroupBox->setPaletteBackgroundColor( QColor( 220, 220, 220 ) );
    PortCDirGroupBox->setFrameShape( QButtonGroup::NoFrame );
    PortCDirGroupBox->setFlat( FALSE );

    checkBox13_9_2 = new QCheckBox( PortCDirGroupBox, "checkBox13_9_2" );
    checkBox13_9_2->setGeometry( QRect( 5, 5, 16, 20 ) );
    PortCDirGroupBox->insert( checkBox13_9_2, 7 );

    checkBox13_2_5_2 = new QCheckBox( PortCDirGroupBox, "checkBox13_2_5_2" );
    checkBox13_2_5_2->setGeometry( QRect( 25, 5, 16, 20 ) );
    PortCDirGroupBox->insert( checkBox13_2_5_2, 6 );

    checkBox13_3_5_2 = new QCheckBox( PortCDirGroupBox, "checkBox13_3_5_2" );
    checkBox13_3_5_2->setGeometry( QRect( 45, 5, 16, 20 ) );
    PortCDirGroupBox->insert( checkBox13_3_5_2, 5 );

    checkBox13_4_5_2 = new QCheckBox( PortCDirGroupBox, "checkBox13_4_5_2" );
    checkBox13_4_5_2->setGeometry( QRect( 65, 5, 16, 20 ) );
    PortCDirGroupBox->insert( checkBox13_4_5_2, 4 );

    checkBox13_5_7_2 = new QCheckBox( PortCDirGroupBox, "checkBox13_5_7_2" );
    checkBox13_5_7_2->setGeometry( QRect( 85, 5, 16, 20 ) );
    PortCDirGroupBox->insert( checkBox13_5_7_2, 3 );

    checkBox13_5_3_6_2 = new QCheckBox( PortCDirGroupBox, "checkBox13_5_3_6_2" );
    checkBox13_5_3_6_2->setGeometry( QRect( 125, 5, 16, 20 ) );
    PortCDirGroupBox->insert( checkBox13_5_3_6_2, 1 );

    checkBox13_5_3_2_5_2 = new QCheckBox( PortCDirGroupBox, "checkBox13_5_3_2_5_2" );
    checkBox13_5_3_2_5_2->setGeometry( QRect( 145, 5, 16, 20 ) );
    PortCDirGroupBox->insert( checkBox13_5_3_2_5_2, 0 );

    PortCDirBox = new QLineEdit( PortCDirGroupBox, "PortCDirBox" );
    PortCDirBox->setGeometry( QRect( 165, 5, 45, 22 ) );
    PortCDirBox->setReadOnly( FALSE );

    checkBox13_5_2_5_2 = new QCheckBox( PortCDirGroupBox, "checkBox13_5_2_5_2" );
    checkBox13_5_2_5_2->setGeometry( QRect( 105, 5, 16, 20 ) );
    PortCDirGroupBox->insert( checkBox13_5_2_5_2, 2 );

    PortCValueGroupBox = new QButtonGroup( groupBox11_4, "PortCValueGroupBox" );
    PortCValueGroupBox->setGeometry( QRect( 70, 50, 215, 25 ) );
    PortCValueGroupBox->setPaletteBackgroundColor( QColor( 220, 220, 220 ) );
    PortCValueGroupBox->setFrameShape( QButtonGroup::NoFrame );
    PortCValueGroupBox->setFlat( FALSE );

    checkBox13_5_3_7_2 = new QCheckBox( PortCValueGroupBox, "checkBox13_5_3_7_2" );
    checkBox13_5_3_7_2->setGeometry( QRect( 125, 5, 16, 20 ) );
    PortCValueGroupBox->insert( checkBox13_5_3_7_2, 1 );

    checkBox13_5_2_6_2 = new QCheckBox( PortCValueGroupBox, "checkBox13_5_2_6_2" );
    checkBox13_5_2_6_2->setGeometry( QRect( 105, 5, 16, 20 ) );
    PortCValueGroupBox->insert( checkBox13_5_2_6_2, 2 );

    checkBox13_5_8_2 = new QCheckBox( PortCValueGroupBox, "checkBox13_5_8_2" );
    checkBox13_5_8_2->setGeometry( QRect( 85, 5, 16, 20 ) );
    PortCValueGroupBox->insert( checkBox13_5_8_2, 3 );

    checkBox13_4_6_2 = new QCheckBox( PortCValueGroupBox, "checkBox13_4_6_2" );
    checkBox13_4_6_2->setGeometry( QRect( 65, 5, 16, 20 ) );
    PortCValueGroupBox->insert( checkBox13_4_6_2, 4 );

    checkBox13_3_6_2 = new QCheckBox( PortCValueGroupBox, "checkBox13_3_6_2" );
    checkBox13_3_6_2->setGeometry( QRect( 45, 5, 16, 20 ) );
    PortCValueGroupBox->insert( checkBox13_3_6_2, 5 );

    checkBox13_2_6_2 = new QCheckBox( PortCValueGroupBox, "checkBox13_2_6_2" );
    checkBox13_2_6_2->setGeometry( QRect( 25, 5, 16, 20 ) );
    checkBox13_2_6_2->setTristate( FALSE );
    PortCValueGroupBox->insert( checkBox13_2_6_2, 6 );

    checkBox13_10_2 = new QCheckBox( PortCValueGroupBox, "checkBox13_10_2" );
    checkBox13_10_2->setGeometry( QRect( 5, 5, 16, 20 ) );
    PortCValueGroupBox->insert( checkBox13_10_2, 7 );

    checkBox13_5_3_2_6_2 = new QCheckBox( PortCValueGroupBox, "checkBox13_5_3_2_6_2" );
    checkBox13_5_3_2_6_2->setGeometry( QRect( 145, 5, 16, 20 ) );
    PortCValueGroupBox->insert( checkBox13_5_3_2_6_2, 0 );

    PortCValueBox = new QLineEdit( PortCValueGroupBox, "PortCValueBox" );
    PortCValueBox->setGeometry( QRect( 165, 5, 45, 22 ) );
    PortCValueBox->setReadOnly( FALSE );
    PageControl->insertTab( TabPage, QString::fromLatin1("") );

    TabPage_2 = new QWidget( PageControl, "TabPage_2" );

    groupBox4 = new QGroupBox( TabPage_2, "groupBox4" );
    groupBox4->setGeometry( QRect( 10, 5, 410, 50 ) );

    textLabel2_2 = new QLabel( groupBox4, "textLabel2_2" );
    textLabel2_2->setGeometry( QRect( 15, 20, 25, 16 ) );

    RS232PortComboBox = new QComboBox( FALSE, groupBox4, "RS232PortComboBox" );
    RS232PortComboBox->setGeometry( QRect( 55, 15, 45, 22 ) );
    RS232PortComboBox->setSizeLimit( 2 );
    RS232PortComboBox->setInsertionPolicy( QComboBox::AtBottom );

    textLabel3 = new QLabel( groupBox4, "textLabel3" );
    textLabel3->setGeometry( QRect( 135, 20, 30, 16 ) );

    RS232FlushButton = new QPushButton( groupBox4, "RS232FlushButton" );
    RS232FlushButton->setGeometry( QRect( 330, 15, 65, 25 ) );

    RS232BaudRateComboBox = new QComboBox( FALSE, groupBox4, "RS232BaudRateComboBox" );
    RS232BaudRateComboBox->setGeometry( QRect( 180, 15, 85, 22 ) );
    RS232BaudRateComboBox->setEditable( TRUE );

    groupBox5 = new QGroupBox( TabPage_2, "groupBox5" );
    groupBox5->setGeometry( QRect( 10, 65, 410, 155 ) );

    GetNumTextBox = new QLineEdit( groupBox5, "GetNumTextBox" );
    GetNumTextBox->setGeometry( QRect( 110, 15, 45, 22 ) );
    GetNumTextBox->setReadOnly( TRUE );

    textLabel1_2 = new QLabel( groupBox5, "textLabel1_2" );
    textLabel1_2->setGeometry( QRect( 10, 20, 85, 16 ) );

    RS232ReadTextBox = new QTextEdit( groupBox5, "RS232ReadTextBox" );
    RS232ReadTextBox->setGeometry( QRect( 10, 50, 385, 95 ) );

    RS232ReadButton = new QPushButton( groupBox5, "RS232ReadButton" );
    RS232ReadButton->setGeometry( QRect( 330, 15, 65, 25 ) );

    groupBox6 = new QGroupBox( TabPage_2, "groupBox6" );
    groupBox6->setGeometry( QRect( 10, 230, 410, 155 ) );

    RS232WriteTextBox = new QTextEdit( groupBox6, "RS232WriteTextBox" );
    RS232WriteTextBox->setGeometry( QRect( 10, 50, 385, 95 ) );

    RS232WriteButton = new QPushButton( groupBox6, "RS232WriteButton" );
    RS232WriteButton->setGeometry( QRect( 330, 15, 65, 25 ) );
    PageControl->insertTab( TabPage_2, QString::fromLatin1("") );

    TabPage_3 = new QWidget( PageControl, "TabPage_3" );

    groupBox5_2_2 = new QGroupBox( TabPage_3, "groupBox5_2_2" );
    groupBox5_2_2->setGeometry( QRect( 10, 5, 410, 170 ) );

    I2cReadLengthTextBox = new QLineEdit( groupBox5_2_2, "I2cReadLengthTextBox" );
    I2cReadLengthTextBox->setGeometry( QRect( 180, 15, 45, 22 ) );
    I2cReadLengthTextBox->setReadOnly( FALSE );

    I2cReadButton = new QPushButton( groupBox5_2_2, "I2cReadButton" );
    I2cReadButton->setGeometry( QRect( 330, 15, 65, 25 ) );

    I2cReadTextBox = new QTextEdit( groupBox5_2_2, "I2cReadTextBox" );
    I2cReadTextBox->setGeometry( QRect( 10, 50, 385, 105 ) );

    textLabel1_2_3_3 = new QLabel( groupBox5_2_2, "textLabel1_2_3_3" );
    textLabel1_2_3_3->setGeometry( QRect( 135, 18, 40, 16 ) );

    I2cReadAddressTextBox = new QLineEdit( groupBox5_2_2, "I2cReadAddressTextBox" );
    I2cReadAddressTextBox->setGeometry( QRect( 70, 15, 45, 22 ) );
    I2cReadAddressTextBox->setReadOnly( FALSE );

    textLabel1_2_3_2_4_2 = new QLabel( groupBox5_2_2, "textLabel1_2_3_2_4_2" );
    textLabel1_2_3_2_4_2->setGeometry( QRect( 15, 20, 50, 16 ) );

    groupBox6_2_2 = new QGroupBox( TabPage_3, "groupBox6_2_2" );
    groupBox6_2_2->setGeometry( QRect( 10, 185, 410, 175 ) );

    I2cWriteTextBox = new QTextEdit( groupBox6_2_2, "I2cWriteTextBox" );
    I2cWriteTextBox->setGeometry( QRect( 10, 55, 385, 105 ) );

    I2cWriteButton = new QPushButton( groupBox6_2_2, "I2cWriteButton" );
    I2cWriteButton->setGeometry( QRect( 330, 20, 65, 25 ) );

    I2cWriteAddressTextBox = new QLineEdit( groupBox6_2_2, "I2cWriteAddressTextBox" );
    I2cWriteAddressTextBox->setGeometry( QRect( 70, 20, 45, 22 ) );
    I2cWriteAddressTextBox->setReadOnly( FALSE );

    textLabel1_2_3_2_4 = new QLabel( groupBox6_2_2, "textLabel1_2_3_2_4" );
    textLabel1_2_3_2_4->setGeometry( QRect( 15, 25, 50, 16 ) );
    PageControl->insertTab( TabPage_3, QString::fromLatin1("") );

    TabPage_4 = new QWidget( PageControl, "TabPage_4" );

    groupBox5_2 = new QGroupBox( TabPage_4, "groupBox5_2" );
    groupBox5_2->setGeometry( QRect( 10, 5, 410, 170 ) );

    textLabel1_2_3_2_2_2 = new QLabel( groupBox5_2, "textLabel1_2_3_2_2_2" );
    textLabel1_2_3_2_2_2->setGeometry( QRect( 15, 20, 25, 16 ) );

    SpiReadLengthTextBox = new QLineEdit( groupBox5_2, "SpiReadLengthTextBox" );
    SpiReadLengthTextBox->setGeometry( QRect( 180, 15, 45, 22 ) );
    SpiReadLengthTextBox->setReadOnly( FALSE );

    SpiReadPortNumComboBox = new QComboBox( FALSE, groupBox5_2, "SpiReadPortNumComboBox" );
    SpiReadPortNumComboBox->setGeometry( QRect( 60, 15, 45, 22 ) );

    SpiReadButton = new QPushButton( groupBox5_2, "SpiReadButton" );
    SpiReadButton->setGeometry( QRect( 330, 15, 65, 25 ) );

    SpiReadTextBox = new QTextEdit( groupBox5_2, "SpiReadTextBox" );
    SpiReadTextBox->setGeometry( QRect( 10, 50, 385, 105 ) );

    textLabel1_2_3 = new QLabel( groupBox5_2, "textLabel1_2_3" );
    textLabel1_2_3->setGeometry( QRect( 135, 18, 40, 16 ) );

    groupBox6_2 = new QGroupBox( TabPage_4, "groupBox6_2" );
    groupBox6_2->setGeometry( QRect( 10, 185, 410, 175 ) );

    SpiWriteButton = new QPushButton( groupBox6_2, "SpiWriteButton" );
    SpiWriteButton->setGeometry( QRect( 330, 20, 65, 25 ) );

    SpiWriteReadButton = new QPushButton( groupBox6_2, "SpiWriteReadButton" );
    SpiWriteReadButton->setGeometry( QRect( 250, 20, 65, 25 ) );

    SpiWriteTextBox = new QTextEdit( groupBox6_2, "SpiWriteTextBox" );
    SpiWriteTextBox->setGeometry( QRect( 10, 55, 385, 105 ) );

    textLabel1_2_3_2_2_2_2 = new QLabel( groupBox6_2, "textLabel1_2_3_2_2_2_2" );
    textLabel1_2_3_2_2_2_2->setGeometry( QRect( 15, 25, 25, 16 ) );

    SpiWritePortNumComboBox = new QComboBox( FALSE, groupBox6_2, "SpiWritePortNumComboBox" );
    SpiWritePortNumComboBox->setGeometry( QRect( 60, 20, 45, 22 ) );
    PageControl->insertTab( TabPage_4, QString::fromLatin1("") );

    // actions
    fileExitAction = new QAction( this, "fileExitAction" );
    helpAboutAction = new QAction( this, "helpAboutAction" );


    // toolbars


    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );

    MenuBar->setGeometry( QRect( 0, 0, 445, 24 ) );
    MenuBar->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, MenuBar->sizePolicy().hasHeightForWidth() ) );
    MenuBar->setMaximumSize( QSize( 450, 500 ) );

    fileMenu = new QPopupMenu( this );
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 4 );

    helpMenu = new QPopupMenu( this );
    helpAboutAction->addTo( helpMenu );
    MenuBar->insertItem( QString(""), helpMenu, 5 );

    languageChange();
    resize( QSize(445, 520).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( browseButton, SIGNAL( clicked() ), this, SLOT( browseButton_clicked() ) );
    connect( CommandLengthTextBox, SIGNAL( textChanged(const QString&) ), this, SLOT( CommandLengthTextBox_textChanged(const QString&) ) );
    connect( CommandReadButton, SIGNAL( clicked() ), this, SLOT( CommandReadButton_clicked() ) );
    connect( CommandWriteButton, SIGNAL( clicked() ), this, SLOT( CommandWriteButton_clicked() ) );
    connect( DataCountButton, SIGNAL( clicked() ), this, SLOT( DataCountButton_clicked() ) );
    connect( DataReadButton, SIGNAL( clicked() ), this, SLOT( DataReadButton_clicked() ) );
    connect( DataWriteButton, SIGNAL( clicked() ), this, SLOT( DataWriteButton_clicked() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( FileExit_activated() ) );
    connect( fpgaUploadButton, SIGNAL( clicked() ), this, SLOT( fpgaUploadButton_clicked() ) );
    connect( helpAboutAction, SIGNAL( activated() ), this, SLOT( HelpAbout_activated() ) );
    connect( I2cReadButton, SIGNAL( clicked() ), this, SLOT( I2cReadButton_clicked() ) );
    connect( I2cWriteButton, SIGNAL( clicked() ), this, SLOT( I2cWriteButton_clicked() ) );
    connect( intervalTextBox, SIGNAL( textChanged(const QString&) ), this, SLOT( intervalTextBox_textChanged(const QString&) ) );
    connect( moduleComboBox, SIGNAL( activated(const QString&) ), this, SLOT( moduleComboBox_activated(const QString&) ) );
    connect( PageControl, SIGNAL( currentChanged(QWidget*) ), this, SLOT( PageControl_currentChanged(QWidget*) ) );
    connect( PortADirBox, SIGNAL( returnPressed() ), this, SLOT( PortADirBox_returnPressed() ) );
    connect( PortADirGroupBox, SIGNAL( clicked(int) ), this, SLOT( PortADirGroupBox_clicked(int) ) );
    connect( PortAReadButton, SIGNAL( clicked() ), this, SLOT( PortAReadButton_clicked() ) );
    connect( PortAValueBox, SIGNAL( returnPressed() ), this, SLOT( PortAValueBox_returnPressed() ) );
    connect( PortAValueGroupBox, SIGNAL( clicked(int) ), this, SLOT( PortAValueGroupBox_clicked(int) ) );
    connect( PortCDirBox, SIGNAL( returnPressed() ), this, SLOT( PortCDirBox_returnPressed() ) );
    connect( PortCDirGroupBox, SIGNAL( clicked(int) ), this, SLOT( PortCDirGroupBox_clicked(int) ) );
    connect( PortCReadButton, SIGNAL( clicked() ), this, SLOT( PortCReadButton_clicked() ) );
    connect( PortCValueBox, SIGNAL( returnPressed() ), this, SLOT( PortCValueBox_returnPressed() ) );
    connect( PortCValueGroupBox, SIGNAL( clicked(int) ), this, SLOT( PortCValueGroupBox_clicked(int) ) );
    connect( PortEDirBox, SIGNAL( returnPressed() ), this, SLOT( PortEDirBox_returnPressed() ) );
    connect( PortEDirGroupBox, SIGNAL( clicked(int) ), this, SLOT( PortEDirGroupBox_clicked(int) ) );
    connect( PortEReadButton, SIGNAL( clicked() ), this, SLOT( PortEReadButton_clicked() ) );
    connect( PortEValueBox, SIGNAL( returnPressed() ), this, SLOT( PortEValueBox_returnPressed() ) );
    connect( PortEValueGroupBox, SIGNAL( clicked(int) ), this, SLOT( PortEValueGroupBox_clicked(int) ) );
    connect( repeatLastButtonCheckBox, SIGNAL( clicked() ), this, SLOT( repeatLastButtonCheckBox_clicked() ) );
    connect( RS232BaudRateComboBox, SIGNAL( activated(const QString&) ), this, SLOT( RS232BaudRateComboBox_activated(const QString&) ) );
    connect( RS232FlushButton, SIGNAL( clicked() ), this, SLOT( RS232FlushButton_clicked() ) );
    connect( RS232PortComboBox, SIGNAL( activated(int) ), this, SLOT( RS232PortComboBox_activated(int) ) );
    connect( RS232PortComboBox, SIGNAL( activated(int) ), this, SLOT( RS232PortComboBox_activated(int) ) );
    connect( RS232ReadButton, SIGNAL( clicked() ), this, SLOT( RS232ReadButton_clicked() ) );
    connect( RS232WriteButton, SIGNAL( clicked() ), this, SLOT( RS232WriteButton_clicked() ) );
    connect( SpiReadButton, SIGNAL( clicked() ), this, SLOT( SpiReadButton_clicked() ) );
    connect( SpiWriteButton, SIGNAL( clicked() ), this, SLOT( SpiWriteButton_clicked() ) );
    connect( SpiWriteReadButton, SIGNAL( clicked() ), this, SLOT( SpiWriteReadButton_clicked() ) );
    connect( WordWideCheckBox, SIGNAL( clicked() ), this, SLOT( WordWideCheckBox_clicked() ) );
    connect( asyncCheckBox, SIGNAL( stateChanged(int) ), this, SLOT( asyncCheckBox_stateChanged(int) ) );
    connect( ReadFileBrowseButton, SIGNAL( clicked() ), this, SLOT( ReadFileBrowseButton_clicked() ) );
    connect( WriteFileBrowseButton, SIGNAL( clicked() ), this, SLOT( WriteFileBrowseButton_clicked() ) );

    // tab order
    setTabOrder( PageControl, moduleComboBox );
    setTabOrder( moduleComboBox, stringDescriptorTextBox );
    setTabOrder( stringDescriptorTextBox, fpgaUploadButton );
    setTabOrder( fpgaUploadButton, browseButton );
    setTabOrder( browseButton, UploadFilenameTextBox );
    setTabOrder( UploadFilenameTextBox, CommandAddressTextBox );
    setTabOrder( CommandAddressTextBox, CommandLengthTextBox );
    setTabOrder( CommandLengthTextBox, CommandReadButton );
    setTabOrder( CommandReadButton, CommandWriteButton );
    setTabOrder( CommandWriteButton, CommandAddressCheckBox );
    setTabOrder( CommandAddressCheckBox, CommandFlexGrid );
    setTabOrder( CommandFlexGrid, DataAddressTextBox );
    setTabOrder( DataAddressTextBox, DataReadButton );
    setTabOrder( DataReadButton, DataLengthTextBox );
    setTabOrder( DataLengthTextBox, DataWriteButton );
    setTabOrder( DataWriteButton, DataAddressCheckBox );
    setTabOrder( DataAddressCheckBox, DataCountButton );
    setTabOrder( DataCountButton, WordWideCheckBox );
    setTabOrder( WordWideCheckBox, checkBox13 );
    setTabOrder( checkBox13, checkBox13_2 );
    setTabOrder( checkBox13_2, checkBox13_3 );
    setTabOrder( checkBox13_3, checkBox13_4 );
    setTabOrder( checkBox13_4, checkBox13_5 );
    setTabOrder( checkBox13_5, checkBox13_5_2 );
    setTabOrder( checkBox13_5_2, checkBox13_5_3 );
    setTabOrder( checkBox13_5_3, checkBox13_5_3_2 );
    setTabOrder( checkBox13_5_3_2, PortADirBox );
    setTabOrder( PortADirBox, portACheckBox7 );
    setTabOrder( portACheckBox7, portACheckBox6 );
    setTabOrder( portACheckBox6, portACheckBox5 );
    setTabOrder( portACheckBox5, portACheckBox4 );
    setTabOrder( portACheckBox4, checkBox13_5_4 );
    setTabOrder( checkBox13_5_4, checkBox13_5_2_2 );
    setTabOrder( checkBox13_5_2_2, checkBox13_5_3_3 );
    setTabOrder( checkBox13_5_3_3, checkBox13_5_3_2_2 );
    setTabOrder( checkBox13_5_3_2_2, PortAValueBox );
    setTabOrder( PortAValueBox, PortAReadButton );
    setTabOrder( PortAReadButton, checkBox13_9_2 );
    setTabOrder( checkBox13_9_2, checkBox13_2_5_2 );
    setTabOrder( checkBox13_2_5_2, checkBox13_3_5_2 );
    setTabOrder( checkBox13_3_5_2, checkBox13_4_5_2 );
    setTabOrder( checkBox13_4_5_2, checkBox13_5_7_2 );
    setTabOrder( checkBox13_5_7_2, checkBox13_5_2_5_2 );
    setTabOrder( checkBox13_5_2_5_2, checkBox13_5_3_6_2 );
    setTabOrder( checkBox13_5_3_6_2, checkBox13_5_3_2_5_2 );
    setTabOrder( checkBox13_5_3_2_5_2, PortCDirBox );
    setTabOrder( PortCDirBox, checkBox13_10_2 );
    setTabOrder( checkBox13_10_2, checkBox13_2_6_2 );
    setTabOrder( checkBox13_2_6_2, checkBox13_3_6_2 );
    setTabOrder( checkBox13_3_6_2, checkBox13_4_6_2 );
    setTabOrder( checkBox13_4_6_2, checkBox13_5_8_2 );
    setTabOrder( checkBox13_5_8_2, checkBox13_5_2_6_2 );
    setTabOrder( checkBox13_5_2_6_2, checkBox13_5_3_7_2 );
    setTabOrder( checkBox13_5_3_7_2, checkBox13_5_3_2_6_2 );
    setTabOrder( checkBox13_5_3_2_6_2, PortCValueBox );
    setTabOrder( PortCValueBox, PortCReadButton );
    setTabOrder( PortCReadButton, checkBox13_9 );
    setTabOrder( checkBox13_9, checkBox13_2_5 );
    setTabOrder( checkBox13_2_5, checkBox13_3_5 );
    setTabOrder( checkBox13_3_5, checkBox13_4_5 );
    setTabOrder( checkBox13_4_5, checkBox13_5_7 );
    setTabOrder( checkBox13_5_7, checkBox13_5_2_5 );
    setTabOrder( checkBox13_5_2_5, checkBox13_5_3_6 );
    setTabOrder( checkBox13_5_3_6, checkBox13_5_3_2_5 );
    setTabOrder( checkBox13_5_3_2_5, PortEDirBox );
    setTabOrder( PortEDirBox, checkBox13_10 );
    setTabOrder( checkBox13_10, checkBox13_2_6 );
    setTabOrder( checkBox13_2_6, checkBox13_3_6 );
    setTabOrder( checkBox13_3_6, checkBox13_4_6 );
    setTabOrder( checkBox13_4_6, checkBox13_5_8 );
    setTabOrder( checkBox13_5_8, checkBox13_5_2_6 );
    setTabOrder( checkBox13_5_2_6, checkBox13_5_3_7 );
    setTabOrder( checkBox13_5_3_7, checkBox13_5_3_2_6 );
    setTabOrder( checkBox13_5_3_2_6, PortEValueBox );
    setTabOrder( PortEValueBox, PortEReadButton );
    setTabOrder( PortEReadButton, RS232PortComboBox );
    setTabOrder( RS232PortComboBox, RS232BaudRateComboBox );
    setTabOrder( RS232BaudRateComboBox, RS232FlushButton );
    setTabOrder( RS232FlushButton, GetNumTextBox );
    setTabOrder( GetNumTextBox, RS232ReadButton );
    setTabOrder( RS232ReadButton, RS232ReadTextBox );
    setTabOrder( RS232ReadTextBox, RS232WriteButton );
    setTabOrder( RS232WriteButton, RS232WriteTextBox );
    setTabOrder( RS232WriteTextBox, SpiReadPortNumComboBox );
    setTabOrder( SpiReadPortNumComboBox, SpiReadLengthTextBox );
    setTabOrder( SpiReadLengthTextBox, SpiReadButton );
    setTabOrder( SpiReadButton, SpiReadTextBox );
    setTabOrder( SpiReadTextBox, SpiWritePortNumComboBox );
    setTabOrder( SpiWritePortNumComboBox, SpiWriteReadButton );
    setTabOrder( SpiWriteReadButton, SpiWriteButton );
    setTabOrder( SpiWriteButton, SpiWriteTextBox );
    setTabOrder( SpiWriteTextBox, I2cReadAddressTextBox );
    setTabOrder( I2cReadAddressTextBox, I2cReadLengthTextBox );
    setTabOrder( I2cReadLengthTextBox, I2cReadButton );
    setTabOrder( I2cReadButton, I2cReadTextBox );
    setTabOrder( I2cReadTextBox, I2cWriteAddressTextBox );
    setTabOrder( I2cWriteAddressTextBox, I2cWriteButton );
    setTabOrder( I2cWriteButton, I2cWriteTextBox );
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
QuickUsbDisplay::~QuickUsbDisplay()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QuickUsbDisplay::languageChange()
{
    setCaption( tr( "QuickUsb Diagnostic" ) );
    QToolTip::add( this, QString::null );
    textLabel1_5->setText( tr( "Period" ) );
    textLabel2_5->setText( tr( "ms" ) );
    intervalTextBox->setText( tr( "500" ) );
    QToolTip::add( intervalTextBox, tr( "Period in which last button will be repeated" ) );
    repeatLastButtonCheckBox->setText( tr( "Repe&at last button" ) );
    repeatLastButtonCheckBox->setAccel( QKeySequence( tr( "Alt+A" ) ) );
    QToolTip::add( repeatLastButtonCheckBox, tr( "Repeat the last button clicked at the specified period" ) );
    groupBox1->setTitle( QString::null );
    textLabel1->setText( tr( "Modules" ) );
    textLabel2->setText( tr( "Descriptor" ) );
    textLabel1_6->setText( tr( "Serial Num" ) );
    textLabel2_6->setText( tr( "USB Speed" ) );
    textLabel3_3->setText( tr( "HW Version" ) );
    QToolTip::add( moduleComboBox, tr( "Selects the active QuickUSB Module" ) );
    groupBox2->setTitle( QString::null );
    browseButton->setText( tr( "&Browse" ) );
    browseButton->setAccel( QKeySequence( tr( "Alt+B" ) ) );
    QToolTip::add( browseButton, tr( "Select an RBF file to upload to the evaluation board FPGA" ) );
    UploadFilenameTextBox->setText( tr( "Please select a file to upload" ) );
    FpgaTypeComboBox->clear();
    FpgaTypeComboBox->insertItem( tr( "Altera passive serial" ) );
    FpgaTypeComboBox->insertItem( tr( "Xilinx slave serial" ) );
    QToolTip::add( FpgaTypeComboBox, tr( "Select FPGA type" ) );
    fpgaUploadButton->setText( tr( "&Upload" ) );
    fpgaUploadButton->setAccel( QKeySequence( tr( "Alt+U" ) ) );
    QToolTip::add( fpgaUploadButton, tr( "Upload an RBF file to the evaluation board FPGA" ) );
    PageControl->changeTab( tab, tr( "&General" ) );
    groupBox14->setTitle( tr( "Command" ) );
    textLabel3_2->setText( tr( "Command Editor" ) );
    QToolTip::add( CommandFlexGrid, tr( "Command editor for command mode data" ) );
    textLabel1_4->setText( tr( "Address" ) );
    CommandAddressTextBox->setText( tr( "0" ) );
    QToolTip::add( CommandAddressTextBox, tr( "Starting GPIFADR value for the command mode transfer" ) );
    CommandLengthTextBox->setText( tr( "8" ) );
    QToolTip::add( CommandLengthTextBox, tr( "Number of items to transfer in command mode" ) );
    textLabel2_4->setText( tr( "Length" ) );
    CommandAddressCheckBox->setText( tr( "Inc Address" ) );
    QToolTip::add( CommandAddressCheckBox, tr( "Increment the address during command transfers if checked" ) );
    CommandWriteButton->setText( tr( "Write" ) );
    QToolTip::add( CommandWriteButton, tr( "Write 'length' items from the command editor to the QuickUSB module" ) );
    CommandReadButton->setText( tr( "Read" ) );
    QToolTip::add( CommandReadButton, tr( "Read 'length' items from the QuickUSB module into the command editor" ) );
    WordWideCheckBox->setText( tr( "Word Wide" ) );
    QToolTip::add( WordWideCheckBox, tr( "Checked=16 bit command and data transfers, unchecked=8-bit command and data transfers" ) );
    groupBox14_2->setTitle( tr( "Data" ) );
    DataAddressCheckBox->setText( tr( "Inc Address" ) );
    QToolTip::add( DataAddressCheckBox, tr( "Increment the address during data transfers if checked" ) );
    asyncCheckBox->setText( tr( "As&ync" ) );
    asyncCheckBox->setAccel( QKeySequence( tr( "Alt+Y" ) ) );
    textLabel1_4_2->setText( tr( "Address" ) );
    textLabel2_4_2->setText( tr( "Length" ) );
    bufferCountLabel->setText( tr( "Buf Cnt" ) );
    bufferSizeLabel->setText( tr( "Buf Size" ) );
    bufferSizeTextBox->setText( tr( "65536" ) );
    bufferCountTextBox->setText( tr( "4" ) );
    DataLengthTextBox->setText( tr( "512" ) );
    QToolTip::add( DataLengthTextBox, tr( "Number of items to transfer in data mode" ) );
    DataAddressTextBox->setText( tr( "0" ) );
    QToolTip::add( DataAddressTextBox, tr( "Starting GPIFADR address for the data mode transfer" ) );
    DataCountButton->setText( tr( "Co&unt" ) );
    DataCountButton->setAccel( QKeySequence( tr( "Alt+U" ) ) );
    QToolTip::add( DataCountButton, tr( "Write a count sequence to the QuickUSB module in data mode" ) );
    DataReadButton->setText( tr( "Read" ) );
    QToolTip::add( DataReadButton, tr( "Read 'length' items from the QuickUSB module in data mode" ) );
    ReadFileBrowseButton->setText( tr( "Browse" ) );
    ReadFileBrowseButton->setAccel( QKeySequence( QString::null ) );
    QToolTip::add( ReadFileBrowseButton, tr( "Select output disk file to place QuickUSB data" ) );
    WriteFileBrowseButton->setText( tr( "Browse" ) );
    WriteFileBrowseButton->setAccel( QKeySequence( QString::null ) );
    QToolTip::add( WriteFileBrowseButton, tr( "Select disk file to write to QuickUSB" ) );
    DataWriteButton->setText( tr( "Write" ) );
    QToolTip::add( DataWriteButton, tr( "Write contents of file to the QuickUSB module in data mode" ) );
    PageControl->changeTab( tabd, tr( "Comma&nd/Data" ) );
    groupBox10->setTitle( tr( "Port I/O" ) );
    groupBox11_3->setTitle( tr( "Port E" ) );
    textLabel1_3_3->setText( tr( "Dir" ) );
    textLabel2_3_3->setText( tr( "Value" ) );
    PortEReadButton->setText( tr( "Read" ) );
    PortEDirGroupBox->setTitle( QString::null );
    checkBox13_3_5->setText( QString::null );
    checkBox13_4_5->setText( QString::null );
    checkBox13_5_2_5->setText( QString::null );
    checkBox13_5_3_6->setText( QString::null );
    checkBox13_5_3_2_5->setText( QString::null );
    checkBox13_9->setText( QString::null );
    checkBox13_2_5->setText( QString::null );
    checkBox13_5_7->setText( QString::null );
    PortEValueGroupBox->setTitle( QString::null );
    checkBox13_3_6->setText( QString::null );
    checkBox13_4_6->setText( QString::null );
    checkBox13_5_2_6->setText( QString::null );
    checkBox13_5_3_7->setText( QString::null );
    checkBox13_2_6->setText( QString::null );
    checkBox13_10->setText( QString::null );
    checkBox13_5_3_2_6->setText( QString::null );
    checkBox13_5_8->setText( QString::null );
    groupBox11->setTitle( tr( "Port A" ) );
    textLabel2_3->setText( tr( "Value" ) );
    PortAReadButton->setText( tr( "Read" ) );
    textLabel1_3->setText( tr( "Dir" ) );
    PortADirGroupBox->setTitle( QString::null );
    checkBox13_5_3_2->setText( QString::null );
    checkBox13_2->setText( QString::null );
    checkBox13_3->setText( QString::null );
    checkBox13_4->setText( QString::null );
    checkBox13_5->setText( QString::null );
    checkBox13_5_2->setText( QString::null );
    checkBox13->setText( QString::null );
    checkBox13_5_3->setText( QString::null );
    PortAValueGroupBox->setTitle( QString::null );
    portACheckBox7->setText( QString::null );
    portACheckBox4->setText( QString::null );
    checkBox13_5_4->setText( QString::null );
    checkBox13_5_2_2->setText( QString::null );
    checkBox13_5_3_3->setText( QString::null );
    checkBox13_5_3_2_2->setText( QString::null );
    PortAValueBox->setText( QString::null );
    portACheckBox6->setText( QString::null );
    portACheckBox5->setText( QString::null );
    groupBox11_4->setTitle( tr( "Port C" ) );
    textLabel1_3_2->setText( tr( "Dir" ) );
    textLabel2_3_2->setText( tr( "Value" ) );
    PortCReadButton->setText( tr( "Read" ) );
    PortCDirGroupBox->setTitle( QString::null );
    checkBox13_9_2->setText( QString::null );
    checkBox13_2_5_2->setText( QString::null );
    checkBox13_3_5_2->setText( QString::null );
    checkBox13_4_5_2->setText( QString::null );
    checkBox13_5_7_2->setText( QString::null );
    checkBox13_5_3_6_2->setText( QString::null );
    checkBox13_5_3_2_5_2->setText( QString::null );
    checkBox13_5_2_5_2->setText( QString::null );
    PortCValueGroupBox->setTitle( QString::null );
    checkBox13_5_3_7_2->setText( QString::null );
    checkBox13_5_2_6_2->setText( QString::null );
    checkBox13_5_8_2->setText( QString::null );
    checkBox13_4_6_2->setText( QString::null );
    checkBox13_3_6_2->setText( QString::null );
    checkBox13_2_6_2->setText( QString::null );
    checkBox13_10_2->setText( QString::null );
    checkBox13_5_3_2_6_2->setText( QString::null );
    PageControl->changeTab( TabPage, tr( "Po&rt I/O" ) );
    groupBox4->setTitle( tr( "General" ) );
    textLabel2_2->setText( tr( "Port" ) );
    RS232PortComboBox->clear();
    RS232PortComboBox->insertItem( tr( "0" ) );
    RS232PortComboBox->insertItem( tr( "1" ) );
    RS232PortComboBox->setCurrentItem( 0 );
    QToolTip::add( RS232PortComboBox, tr( "Selects the active QuickUSB RS-232 port" ) );
    textLabel3->setText( tr( "Baud" ) );
    RS232FlushButton->setText( tr( "Fl&ush" ) );
    RS232BaudRateComboBox->clear();
    RS232BaudRateComboBox->insertItem( tr( "4800" ) );
    RS232BaudRateComboBox->insertItem( tr( "9600" ) );
    RS232BaudRateComboBox->insertItem( tr( "19200" ) );
    RS232BaudRateComboBox->insertItem( tr( "38400" ) );
    RS232BaudRateComboBox->insertItem( tr( "57600" ) );
    RS232BaudRateComboBox->insertItem( tr( "115200" ) );
    RS232BaudRateComboBox->insertItem( tr( "230400" ) );
    RS232BaudRateComboBox->setCurrentItem( 1 );
    QToolTip::add( RS232BaudRateComboBox, tr( "Selects the QuickUSB RS-232 port baud rate" ) );
    groupBox5->setTitle( tr( "Read" ) );
    GetNumTextBox->setText( tr( "0" ) );
    QToolTip::add( GetNumTextBox, tr( "Number of characters waiting in the receive buffer" ) );
    textLabel1_2->setText( tr( "Chars Waiting" ) );
    QToolTip::add( RS232ReadTextBox, tr( "Contains text read from the selected QuickUSB RS-232 port" ) );
    RS232ReadButton->setText( tr( "Read" ) );
    QToolTip::add( RS232ReadButton, tr( "Read the selected QuickUSB RS-232 port" ) );
    groupBox6->setTitle( tr( "Write" ) );
    QToolTip::add( RS232WriteTextBox, tr( "Contains text to be written to the selected QuickUSB RS-232 port" ) );
    RS232WriteButton->setText( tr( "Write" ) );
    QToolTip::add( RS232WriteButton, tr( "Write the text below to the selected QuickUSB RS-232 port" ) );
    PageControl->changeTab( TabPage_2, tr( "RS2&32" ) );
    groupBox5_2_2->setTitle( tr( "Read" ) );
    I2cReadLengthTextBox->setText( tr( "8" ) );
    QToolTip::add( I2cReadLengthTextBox, tr( "The number of bytes to read" ) );
    I2cReadButton->setText( tr( "Read" ) );
    textLabel1_2_3_3->setText( tr( "Length" ) );
    I2cReadAddressTextBox->setText( tr( "0" ) );
    QToolTip::add( I2cReadAddressTextBox, tr( "The I2C device address" ) );
    textLabel1_2_3_2_4_2->setText( tr( "Address" ) );
    groupBox6_2_2->setTitle( tr( "Write" ) );
    I2cWriteButton->setText( tr( "Write" ) );
    I2cWriteAddressTextBox->setText( tr( "0" ) );
    QToolTip::add( I2cWriteAddressTextBox, tr( "The I2C device address" ) );
    textLabel1_2_3_2_4->setText( tr( "Address" ) );
    PageControl->changeTab( TabPage_3, tr( "I&2C" ) );
    groupBox5_2->setTitle( tr( "Read" ) );
    textLabel1_2_3_2_2_2->setText( tr( "Port" ) );
    SpiReadLengthTextBox->setText( tr( "8" ) );
    SpiReadPortNumComboBox->clear();
    SpiReadPortNumComboBox->insertItem( tr( "0" ) );
    SpiReadPortNumComboBox->insertItem( tr( "1" ) );
    SpiReadPortNumComboBox->insertItem( tr( "2" ) );
    SpiReadPortNumComboBox->insertItem( tr( "3" ) );
    SpiReadPortNumComboBox->insertItem( tr( "4" ) );
    SpiReadPortNumComboBox->insertItem( tr( "5" ) );
    SpiReadPortNumComboBox->insertItem( tr( "6" ) );
    SpiReadPortNumComboBox->insertItem( tr( "7" ) );
    SpiReadPortNumComboBox->insertItem( tr( "8" ) );
    SpiReadPortNumComboBox->insertItem( tr( "9" ) );
    QToolTip::add( SpiReadPortNumComboBox, tr( "Selects the active QuickUSB SPI port" ) );
    SpiReadButton->setText( tr( "Read" ) );
    textLabel1_2_3->setText( tr( "Length" ) );
    groupBox6_2->setTitle( tr( "Write" ) );
    SpiWriteButton->setText( tr( "Write" ) );
    SpiWriteReadButton->setText( tr( "WriteRead" ) );
    textLabel1_2_3_2_2_2_2->setText( tr( "Port" ) );
    SpiWritePortNumComboBox->clear();
    SpiWritePortNumComboBox->insertItem( tr( "0" ) );
    SpiWritePortNumComboBox->insertItem( tr( "1" ) );
    SpiWritePortNumComboBox->insertItem( tr( "2" ) );
    SpiWritePortNumComboBox->insertItem( tr( "3" ) );
    SpiWritePortNumComboBox->insertItem( tr( "4" ) );
    SpiWritePortNumComboBox->insertItem( tr( "5" ) );
    SpiWritePortNumComboBox->insertItem( tr( "6" ) );
    SpiWritePortNumComboBox->insertItem( tr( "7" ) );
    SpiWritePortNumComboBox->insertItem( tr( "8" ) );
    SpiWritePortNumComboBox->insertItem( tr( "9" ) );
    PageControl->changeTab( TabPage_4, tr( "SPI" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "&Exit" ) );
    fileExitAction->setAccel( tr( "Ctrl+Q" ) );
    helpAboutAction->setText( tr( "About" ) );
    helpAboutAction->setMenuText( tr( "&About" ) );
    helpAboutAction->setAccel( tr( "Ctrl+A" ) );
    if (MenuBar->findItem(4))
        MenuBar->findItem(4)->setText( tr( "&File" ) );
    if (MenuBar->findItem(5))
        MenuBar->findItem(5)->setText( tr( "&Help" ) );
}

