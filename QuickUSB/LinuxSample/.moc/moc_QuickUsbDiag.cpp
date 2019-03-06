/****************************************************************************
** QuickUsbDisplay meta object code from reading C++ file 'QuickUsbDiag.h'
**
** Created: Tue Feb 22 18:29:55 2011
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/QuickUsbDiag.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QuickUsbDisplay::className() const
{
    return "QuickUsbDisplay";
}

QMetaObject *QuickUsbDisplay::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QuickUsbDisplay( "QuickUsbDisplay", &QuickUsbDisplay::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QuickUsbDisplay::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QuickUsbDisplay", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QuickUsbDisplay::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QuickUsbDisplay", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QuickUsbDisplay::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"UpdateModuleComboBox", 0, 0 };
    static const QUParameter param_slot_1[] = {
	{ 0, &static_QUType_ptr, "QWidget", QUParameter::In }
    };
    static const QUMethod slot_1 = {"PageControl_currentChanged", 1, param_slot_1 };
    static const QUMethod slot_2 = {"fpgaUploadButton_clicked", 0, 0 };
    static const QUParameter param_slot_3[] = {
	{ "str", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_3 = {"moduleComboBox_activated", 1, param_slot_3 };
    static const QUMethod slot_4 = {"browseButton_clicked", 0, 0 };
    static const QUMethod slot_5 = {"WordWideCheckBox_clicked", 0, 0 };
    static const QUMethod slot_6 = {"CommandReadButton_clicked", 0, 0 };
    static const QUMethod slot_7 = {"CommandWriteButton_clicked", 0, 0 };
    static const QUParameter param_slot_8[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_8 = {"CommandLengthTextBox_textChanged", 1, param_slot_8 };
    static const QUMethod slot_9 = {"AsyncDataRead", 0, 0 };
    static const QUMethod slot_10 = {"AsyncDataWrite", 0, 0 };
    static const QUMethod slot_11 = {"DataReadButton_clicked", 0, 0 };
    static const QUMethod slot_12 = {"DataWriteButton_clicked", 0, 0 };
    static const QUMethod slot_13 = {"DataCountButton_clicked", 0, 0 };
    static const QUMethod slot_14 = {"PortAReadButton_clicked", 0, 0 };
    static const QUMethod slot_15 = {"PortCReadButton_clicked", 0, 0 };
    static const QUMethod slot_16 = {"PortEReadButton_clicked", 0, 0 };
    static const QUMethod slot_17 = {"ReadPortADirBits", 0, 0 };
    static const QUMethod slot_18 = {"ReadPortCDirBits", 0, 0 };
    static const QUMethod slot_19 = {"ReadPortEDirBits", 0, 0 };
    static const QUParameter param_slot_20[] = {
	{ "buttonID", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_20 = {"PortADirGroupBox_clicked", 1, param_slot_20 };
    static const QUParameter param_slot_21[] = {
	{ "buttonID", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_21 = {"PortAValueGroupBox_clicked", 1, param_slot_21 };
    static const QUParameter param_slot_22[] = {
	{ "buttonID", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_22 = {"PortCDirGroupBox_clicked", 1, param_slot_22 };
    static const QUParameter param_slot_23[] = {
	{ "buttonID", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_23 = {"PortCValueGroupBox_clicked", 1, param_slot_23 };
    static const QUParameter param_slot_24[] = {
	{ "buttonID", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_24 = {"PortEDirGroupBox_clicked", 1, param_slot_24 };
    static const QUParameter param_slot_25[] = {
	{ "buttonID", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_25 = {"PortEValueGroupBox_clicked", 1, param_slot_25 };
    static const QUMethod slot_26 = {"PortAValueBox_returnPressed", 0, 0 };
    static const QUMethod slot_27 = {"PortCValueBox_returnPressed", 0, 0 };
    static const QUMethod slot_28 = {"PortEValueBox_returnPressed", 0, 0 };
    static const QUMethod slot_29 = {"PortADirBox_returnPressed", 0, 0 };
    static const QUMethod slot_30 = {"PortCDirBox_returnPressed", 0, 0 };
    static const QUMethod slot_31 = {"PortEDirBox_returnPressed", 0, 0 };
    static const QUMethod slot_32 = {"RS232WriteButton_clicked", 0, 0 };
    static const QUMethod slot_33 = {"RS232ReadButton_clicked", 0, 0 };
    static const QUParameter param_slot_34[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_34 = {"RS232PortComboBox_activated", 1, param_slot_34 };
    static const QUMethod slot_35 = {"RS232FlushButton_clicked", 0, 0 };
    static const QUParameter param_slot_36[] = {
	{ "str", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_36 = {"RS232BaudRateComboBox_activated", 1, param_slot_36 };
    static const QUMethod slot_37 = {"SpiReadButton_clicked", 0, 0 };
    static const QUMethod slot_38 = {"SpiWriteButton_clicked", 0, 0 };
    static const QUMethod slot_39 = {"SpiWriteReadButton_clicked", 0, 0 };
    static const QUMethod slot_40 = {"I2cReadButton_clicked", 0, 0 };
    static const QUMethod slot_41 = {"I2cWriteButton_clicked", 0, 0 };
    static const QUMethod slot_42 = {"cleanUp", 0, 0 };
    static const QUParameter param_slot_43[] = {
	{ "button", &static_QUType_ptr, "QPushButton", QUParameter::In }
    };
    static const QUMethod slot_43 = {"resetTimers", 1, param_slot_43 };
    static const QUMethod slot_44 = {"repeatLastButtonCheckBox_clicked", 0, 0 };
    static const QUParameter param_slot_45[] = {
	{ "str", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_45 = {"intervalTextBox_textChanged", 1, param_slot_45 };
    static const QUMethod slot_46 = {"pressButton", 0, 0 };
    static const QUMethod slot_47 = {"pause", 0, 0 };
    static const QUMethod slot_48 = {"HelpAbout_activated", 0, 0 };
    static const QUMethod slot_49 = {"aboutOnTop", 0, 0 };
    static const QUMethod slot_50 = {"FileExit_activated", 0, 0 };
    static const QUParameter param_slot_51[] = {
	{ "event", &static_QUType_ptr, "QCloseEvent", QUParameter::In }
    };
    static const QUMethod slot_51 = {"closeEvent", 1, param_slot_51 };
    static const QUParameter param_slot_52[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_52 = {"asyncCheckBox_stateChanged", 1, param_slot_52 };
    static const QUMethod slot_53 = {"ReadFileBrowseButton_clicked", 0, 0 };
    static const QUMethod slot_54 = {"WriteFileBrowseButton_clicked", 0, 0 };
    static const QUMethod slot_55 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "UpdateModuleComboBox()", &slot_0, QMetaData::Public },
	{ "PageControl_currentChanged(QWidget*)", &slot_1, QMetaData::Public },
	{ "fpgaUploadButton_clicked()", &slot_2, QMetaData::Public },
	{ "moduleComboBox_activated(const QString&)", &slot_3, QMetaData::Public },
	{ "browseButton_clicked()", &slot_4, QMetaData::Public },
	{ "WordWideCheckBox_clicked()", &slot_5, QMetaData::Public },
	{ "CommandReadButton_clicked()", &slot_6, QMetaData::Public },
	{ "CommandWriteButton_clicked()", &slot_7, QMetaData::Public },
	{ "CommandLengthTextBox_textChanged(const QString&)", &slot_8, QMetaData::Public },
	{ "AsyncDataRead()", &slot_9, QMetaData::Public },
	{ "AsyncDataWrite()", &slot_10, QMetaData::Public },
	{ "DataReadButton_clicked()", &slot_11, QMetaData::Public },
	{ "DataWriteButton_clicked()", &slot_12, QMetaData::Public },
	{ "DataCountButton_clicked()", &slot_13, QMetaData::Public },
	{ "PortAReadButton_clicked()", &slot_14, QMetaData::Public },
	{ "PortCReadButton_clicked()", &slot_15, QMetaData::Public },
	{ "PortEReadButton_clicked()", &slot_16, QMetaData::Public },
	{ "ReadPortADirBits()", &slot_17, QMetaData::Public },
	{ "ReadPortCDirBits()", &slot_18, QMetaData::Public },
	{ "ReadPortEDirBits()", &slot_19, QMetaData::Public },
	{ "PortADirGroupBox_clicked(int)", &slot_20, QMetaData::Public },
	{ "PortAValueGroupBox_clicked(int)", &slot_21, QMetaData::Public },
	{ "PortCDirGroupBox_clicked(int)", &slot_22, QMetaData::Public },
	{ "PortCValueGroupBox_clicked(int)", &slot_23, QMetaData::Public },
	{ "PortEDirGroupBox_clicked(int)", &slot_24, QMetaData::Public },
	{ "PortEValueGroupBox_clicked(int)", &slot_25, QMetaData::Public },
	{ "PortAValueBox_returnPressed()", &slot_26, QMetaData::Public },
	{ "PortCValueBox_returnPressed()", &slot_27, QMetaData::Public },
	{ "PortEValueBox_returnPressed()", &slot_28, QMetaData::Public },
	{ "PortADirBox_returnPressed()", &slot_29, QMetaData::Public },
	{ "PortCDirBox_returnPressed()", &slot_30, QMetaData::Public },
	{ "PortEDirBox_returnPressed()", &slot_31, QMetaData::Public },
	{ "RS232WriteButton_clicked()", &slot_32, QMetaData::Public },
	{ "RS232ReadButton_clicked()", &slot_33, QMetaData::Public },
	{ "RS232PortComboBox_activated(int)", &slot_34, QMetaData::Public },
	{ "RS232FlushButton_clicked()", &slot_35, QMetaData::Public },
	{ "RS232BaudRateComboBox_activated(const QString&)", &slot_36, QMetaData::Public },
	{ "SpiReadButton_clicked()", &slot_37, QMetaData::Public },
	{ "SpiWriteButton_clicked()", &slot_38, QMetaData::Public },
	{ "SpiWriteReadButton_clicked()", &slot_39, QMetaData::Public },
	{ "I2cReadButton_clicked()", &slot_40, QMetaData::Public },
	{ "I2cWriteButton_clicked()", &slot_41, QMetaData::Public },
	{ "cleanUp()", &slot_42, QMetaData::Public },
	{ "resetTimers(QPushButton*)", &slot_43, QMetaData::Public },
	{ "repeatLastButtonCheckBox_clicked()", &slot_44, QMetaData::Public },
	{ "intervalTextBox_textChanged(const QString&)", &slot_45, QMetaData::Public },
	{ "pressButton()", &slot_46, QMetaData::Public },
	{ "pause()", &slot_47, QMetaData::Public },
	{ "HelpAbout_activated()", &slot_48, QMetaData::Public },
	{ "aboutOnTop()", &slot_49, QMetaData::Public },
	{ "FileExit_activated()", &slot_50, QMetaData::Public },
	{ "closeEvent(QCloseEvent*)", &slot_51, QMetaData::Public },
	{ "asyncCheckBox_stateChanged(int)", &slot_52, QMetaData::Public },
	{ "ReadFileBrowseButton_clicked()", &slot_53, QMetaData::Public },
	{ "WriteFileBrowseButton_clicked()", &slot_54, QMetaData::Public },
	{ "languageChange()", &slot_55, QMetaData::Protected }
    };
    static const QUMethod signal_0 = {"clicked", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "clicked()", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"QuickUsbDisplay", parentObject,
	slot_tbl, 56,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QuickUsbDisplay.setMetaObject( metaObj );
    return metaObj;
}

void* QuickUsbDisplay::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QuickUsbDisplay" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

// SIGNAL clicked
void QuickUsbDisplay::clicked()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

bool QuickUsbDisplay::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: UpdateModuleComboBox(); break;
    case 1: PageControl_currentChanged((QWidget*)static_QUType_ptr.get(_o+1)); break;
    case 2: fpgaUploadButton_clicked(); break;
    case 3: moduleComboBox_activated((const QString&)static_QUType_QString.get(_o+1)); break;
    case 4: browseButton_clicked(); break;
    case 5: WordWideCheckBox_clicked(); break;
    case 6: CommandReadButton_clicked(); break;
    case 7: CommandWriteButton_clicked(); break;
    case 8: CommandLengthTextBox_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 9: AsyncDataRead(); break;
    case 10: AsyncDataWrite(); break;
    case 11: DataReadButton_clicked(); break;
    case 12: DataWriteButton_clicked(); break;
    case 13: DataCountButton_clicked(); break;
    case 14: PortAReadButton_clicked(); break;
    case 15: PortCReadButton_clicked(); break;
    case 16: PortEReadButton_clicked(); break;
    case 17: ReadPortADirBits(); break;
    case 18: ReadPortCDirBits(); break;
    case 19: ReadPortEDirBits(); break;
    case 20: PortADirGroupBox_clicked((int)static_QUType_int.get(_o+1)); break;
    case 21: PortAValueGroupBox_clicked((int)static_QUType_int.get(_o+1)); break;
    case 22: PortCDirGroupBox_clicked((int)static_QUType_int.get(_o+1)); break;
    case 23: PortCValueGroupBox_clicked((int)static_QUType_int.get(_o+1)); break;
    case 24: PortEDirGroupBox_clicked((int)static_QUType_int.get(_o+1)); break;
    case 25: PortEValueGroupBox_clicked((int)static_QUType_int.get(_o+1)); break;
    case 26: PortAValueBox_returnPressed(); break;
    case 27: PortCValueBox_returnPressed(); break;
    case 28: PortEValueBox_returnPressed(); break;
    case 29: PortADirBox_returnPressed(); break;
    case 30: PortCDirBox_returnPressed(); break;
    case 31: PortEDirBox_returnPressed(); break;
    case 32: RS232WriteButton_clicked(); break;
    case 33: RS232ReadButton_clicked(); break;
    case 34: RS232PortComboBox_activated((int)static_QUType_int.get(_o+1)); break;
    case 35: RS232FlushButton_clicked(); break;
    case 36: RS232BaudRateComboBox_activated((const QString&)static_QUType_QString.get(_o+1)); break;
    case 37: SpiReadButton_clicked(); break;
    case 38: SpiWriteButton_clicked(); break;
    case 39: SpiWriteReadButton_clicked(); break;
    case 40: I2cReadButton_clicked(); break;
    case 41: I2cWriteButton_clicked(); break;
    case 42: cleanUp(); break;
    case 43: resetTimers((QPushButton*)static_QUType_ptr.get(_o+1)); break;
    case 44: repeatLastButtonCheckBox_clicked(); break;
    case 45: intervalTextBox_textChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    case 46: pressButton(); break;
    case 47: pause(); break;
    case 48: HelpAbout_activated(); break;
    case 49: aboutOnTop(); break;
    case 50: FileExit_activated(); break;
    case 51: closeEvent((QCloseEvent*)static_QUType_ptr.get(_o+1)); break;
    case 52: asyncCheckBox_stateChanged((int)static_QUType_int.get(_o+1)); break;
    case 53: ReadFileBrowseButton_clicked(); break;
    case 54: WriteFileBrowseButton_clicked(); break;
    case 55: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QuickUsbDisplay::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: clicked(); break;
    default:
	return QMainWindow::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool QuickUsbDisplay::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool QuickUsbDisplay::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
