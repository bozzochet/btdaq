/****************************************************************************
** MQDisplay meta object code from reading C++ file 'mqdisplay.h'
**
** Created: Tue Dec 23 11:26:49 2008
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "mqdisplay.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MQDisplay::className() const
{
    return "MQDisplay";
}

QMetaObject *MQDisplay::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MQDisplay( "MQDisplay", &MQDisplay::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MQDisplay::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MQDisplay", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MQDisplay::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MQDisplay", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MQDisplay::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"fileNew", 0, 0 };
    static const QUMethod slot_1 = {"fileOpen", 0, 0 };
    static const QUMethod slot_2 = {"fileSave", 0, 0 };
    static const QUMethod slot_3 = {"fileSaveAs", 0, 0 };
    static const QUMethod slot_4 = {"filePrint", 0, 0 };
    static const QUMethod slot_5 = {"fileExit", 0, 0 };
    static const QUMethod slot_6 = {"editUndo", 0, 0 };
    static const QUMethod slot_7 = {"editRedo", 0, 0 };
    static const QUMethod slot_8 = {"editCut", 0, 0 };
    static const QUMethod slot_9 = {"editCopy", 0, 0 };
    static const QUMethod slot_10 = {"editPaste", 0, 0 };
    static const QUMethod slot_11 = {"editFind", 0, 0 };
    static const QUMethod slot_12 = {"pbPREV_clicked", 0, 0 };
    static const QUMethod slot_13 = {"pbNEXT_clicked", 0, 0 };
    static const QUMethod slot_14 = {"pbOPEN_clicked", 0, 0 };
    static const QUMethod slot_15 = {"pbDraw_clicked", 0, 0 };
    static const QUMethod slot_16 = {"pbAnalize_clicked", 0, 0 };
    static const QUMethod slot_17 = {"pbCLD_clicked", 0, 0 };
    static const QUMethod slot_18 = {"pbRCLD_clicked", 0, 0 };
    static const QUMethod slot_19 = {"pb3DD_clicked", 0, 0 };
    static const QUParameter param_slot_20[] = {
	{ "entry", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_20 = {"SetEntry", 1, param_slot_20 };
    static const QUMethod slot_21 = {"drawEventConn", 0, 0 };
    static const QUParameter param_slot_22[] = {
	{ "entry", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_22 = {"setEntryConn", 1, param_slot_22 };
    static const QUParameter param_slot_23[] = {
	{ "max", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_23 = {"setMaxConn", 1, param_slot_23 };
    static const QUMethod slot_24 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "fileNew()", &slot_0, QMetaData::Public },
	{ "fileOpen()", &slot_1, QMetaData::Public },
	{ "fileSave()", &slot_2, QMetaData::Public },
	{ "fileSaveAs()", &slot_3, QMetaData::Public },
	{ "filePrint()", &slot_4, QMetaData::Public },
	{ "fileExit()", &slot_5, QMetaData::Public },
	{ "editUndo()", &slot_6, QMetaData::Public },
	{ "editRedo()", &slot_7, QMetaData::Public },
	{ "editCut()", &slot_8, QMetaData::Public },
	{ "editCopy()", &slot_9, QMetaData::Public },
	{ "editPaste()", &slot_10, QMetaData::Public },
	{ "editFind()", &slot_11, QMetaData::Public },
	{ "pbPREV_clicked()", &slot_12, QMetaData::Public },
	{ "pbNEXT_clicked()", &slot_13, QMetaData::Public },
	{ "pbOPEN_clicked()", &slot_14, QMetaData::Public },
	{ "pbDraw_clicked()", &slot_15, QMetaData::Public },
	{ "pbAnalize_clicked()", &slot_16, QMetaData::Public },
	{ "pbCLD_clicked()", &slot_17, QMetaData::Public },
	{ "pbRCLD_clicked()", &slot_18, QMetaData::Public },
	{ "pb3DD_clicked()", &slot_19, QMetaData::Public },
	{ "SetEntry(int)", &slot_20, QMetaData::Public },
	{ "drawEventConn()", &slot_21, QMetaData::Public },
	{ "setEntryConn(int)", &slot_22, QMetaData::Public },
	{ "setMaxConn(int)", &slot_23, QMetaData::Public },
	{ "languageChange()", &slot_24, QMetaData::Protected }
    };
    static const QUMethod signal_0 = {"drawEvent", 0, 0 };
    static const QUMethod signal_1 = {"quitApp", 0, 0 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod signal_2 = {"setEntry", 1, param_signal_2 };
    static const QMetaData signal_tbl[] = {
	{ "drawEvent()", &signal_0, QMetaData::Public },
	{ "quitApp()", &signal_1, QMetaData::Public },
	{ "setEntry(int)", &signal_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"MQDisplay", parentObject,
	slot_tbl, 25,
	signal_tbl, 3,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MQDisplay.setMetaObject( metaObj );
    return metaObj;
}

void* MQDisplay::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MQDisplay" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

// SIGNAL drawEvent
void MQDisplay::drawEvent()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

// SIGNAL quitApp
void MQDisplay::quitApp()
{
    activate_signal( staticMetaObject()->signalOffset() + 1 );
}

// SIGNAL setEntry
void MQDisplay::setEntry( int t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 2, t0 );
}

bool MQDisplay::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: fileNew(); break;
    case 1: fileOpen(); break;
    case 2: fileSave(); break;
    case 3: fileSaveAs(); break;
    case 4: filePrint(); break;
    case 5: fileExit(); break;
    case 6: editUndo(); break;
    case 7: editRedo(); break;
    case 8: editCut(); break;
    case 9: editCopy(); break;
    case 10: editPaste(); break;
    case 11: editFind(); break;
    case 12: pbPREV_clicked(); break;
    case 13: pbNEXT_clicked(); break;
    case 14: pbOPEN_clicked(); break;
    case 15: pbDraw_clicked(); break;
    case 16: pbAnalize_clicked(); break;
    case 17: pbCLD_clicked(); break;
    case 18: pbRCLD_clicked(); break;
    case 19: pb3DD_clicked(); break;
    case 20: SetEntry((int)static_QUType_int.get(_o+1)); break;
    case 21: drawEventConn(); break;
    case 22: setEntryConn((int)static_QUType_int.get(_o+1)); break;
    case 23: setMaxConn((int)static_QUType_int.get(_o+1)); break;
    case 24: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MQDisplay::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: drawEvent(); break;
    case 1: quitApp(); break;
    case 2: setEntry((int)static_QUType_int.get(_o+1)); break;
    default:
	return QMainWindow::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool MQDisplay::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool MQDisplay::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
