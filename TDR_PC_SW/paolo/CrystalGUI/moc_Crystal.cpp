/****************************************************************************
** CrystalGUI meta object code from reading C++ file 'Crystal.h'
**
** Created: Wed Oct 24 14:30:05 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "Crystal.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *CrystalGUI::className() const
{
    return "CrystalGUI";
}

QMetaObject *CrystalGUI::metaObj = 0;
static QMetaObjectCleanUp cleanUp_CrystalGUI( "CrystalGUI", &CrystalGUI::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString CrystalGUI::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CrystalGUI", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString CrystalGUI::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CrystalGUI", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* CrystalGUI::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"filePrint", 0, 0 };
    static const QUMethod slot_1 = {"fileExit", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ 0, &static_QUType_ptr, "QDockWindow", QUParameter::In }
    };
    static const QUMethod slot_2 = {"CrystalGUI_dockWindowPositionChanged", 1, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_3 = {"CrystalGUI_usesTextLabelChanged", 1, param_slot_3 };
    static const QUMethod slot_4 = {"pbGonGet_clicked", 0, 0 };
    static const QUMethod slot_5 = {"Init", 0, 0 };
    static const QUMethod slot_6 = {"UpdateGonDisplay", 0, 0 };
    static const QUMethod slot_7 = {"SetGonPos", 0, 0 };
    static const QUMethod slot_8 = {"toolsGoniometer_AdvancedAction_activated", 0, 0 };
    static const QUParameter param_slot_9[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_9 = {"rbGonMurad_toggled", 1, param_slot_9 };
    static const QUParameter param_slot_10[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_10 = {"rbGonRad_toggled", 1, param_slot_10 };
    static const QUParameter param_slot_11[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_11 = {"rbGonGrad_toggled", 1, param_slot_11 };
    static const QUMethod slot_12 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "filePrint()", &slot_0, QMetaData::Public },
	{ "fileExit()", &slot_1, QMetaData::Public },
	{ "CrystalGUI_dockWindowPositionChanged(QDockWindow*)", &slot_2, QMetaData::Public },
	{ "CrystalGUI_usesTextLabelChanged(bool)", &slot_3, QMetaData::Public },
	{ "pbGonGet_clicked()", &slot_4, QMetaData::Public },
	{ "Init()", &slot_5, QMetaData::Public },
	{ "UpdateGonDisplay()", &slot_6, QMetaData::Public },
	{ "SetGonPos()", &slot_7, QMetaData::Public },
	{ "toolsGoniometer_AdvancedAction_activated()", &slot_8, QMetaData::Public },
	{ "rbGonMurad_toggled(bool)", &slot_9, QMetaData::Public },
	{ "rbGonRad_toggled(bool)", &slot_10, QMetaData::Public },
	{ "rbGonGrad_toggled(bool)", &slot_11, QMetaData::Public },
	{ "languageChange()", &slot_12, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"CrystalGUI", parentObject,
	slot_tbl, 13,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_CrystalGUI.setMetaObject( metaObj );
    return metaObj;
}

void* CrystalGUI::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "CrystalGUI" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool CrystalGUI::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: filePrint(); break;
    case 1: fileExit(); break;
    case 2: CrystalGUI_dockWindowPositionChanged((QDockWindow*)static_QUType_ptr.get(_o+1)); break;
    case 3: CrystalGUI_usesTextLabelChanged((bool)static_QUType_bool.get(_o+1)); break;
    case 4: pbGonGet_clicked(); break;
    case 5: Init(); break;
    case 6: UpdateGonDisplay(); break;
    case 7: SetGonPos(); break;
    case 8: toolsGoniometer_AdvancedAction_activated(); break;
    case 9: rbGonMurad_toggled((bool)static_QUType_bool.get(_o+1)); break;
    case 10: rbGonRad_toggled((bool)static_QUType_bool.get(_o+1)); break;
    case 11: rbGonGrad_toggled((bool)static_QUType_bool.get(_o+1)); break;
    case 12: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool CrystalGUI::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool CrystalGUI::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool CrystalGUI::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
