/****************************************************************************
** ClDisplay meta object code from reading C++ file 'cldisplay.h'
**
** Created: Tue Dec 23 11:26:48 2008
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "cldisplay.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ClDisplay::className() const
{
    return "ClDisplay";
}

QMetaObject *ClDisplay::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ClDisplay( "ClDisplay", &ClDisplay::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ClDisplay::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ClDisplay", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ClDisplay::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ClDisplay", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ClDisplay::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUMethod slot_0 = {"init", 0, 0 };
    static const QUMethod slot_1 = {"UpdateS", 0, 0 };
    static const QUMethod slot_2 = {"UpdateEvent", 0, 0 };
    static const QUMethod slot_3 = {"UpdateDigits", 0, 0 };
    static const QUMethod slot_4 = {"UpdateLayer", 0, 0 };
    static const QUMethod slot_5 = {"UpdateTkIdRange", 0, 0 };
    static const QUMethod slot_6 = {"UpdateTkId", 0, 0 };
    static const QUMethod slot_7 = {"UpdateNP", 0, 0 };
    static const QUMethod slot_8 = {"UpdateIclsRange", 0, 0 };
    static const QUMethod slot_9 = {"UpdateIdx", 0, 0 };
    static const QUMethod slot_10 = {"Exit", 0, 0 };
    static const QUParameter param_slot_11[] = {
	{ "sw", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_11 = {"rbPside_toggled", 1, param_slot_11 };
    static const QUParameter param_slot_12[] = {
	{ "sw", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_12 = {"rbNside_toggled", 1, param_slot_12 };
    static const QUMethod slot_13 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "init()", &slot_0, QMetaData::Public },
	{ "UpdateS()", &slot_1, QMetaData::Public },
	{ "UpdateEvent()", &slot_2, QMetaData::Public },
	{ "UpdateDigits()", &slot_3, QMetaData::Public },
	{ "UpdateLayer()", &slot_4, QMetaData::Public },
	{ "UpdateTkIdRange()", &slot_5, QMetaData::Public },
	{ "UpdateTkId()", &slot_6, QMetaData::Public },
	{ "UpdateNP()", &slot_7, QMetaData::Public },
	{ "UpdateIclsRange()", &slot_8, QMetaData::Public },
	{ "UpdateIdx()", &slot_9, QMetaData::Public },
	{ "Exit()", &slot_10, QMetaData::Public },
	{ "rbPside_toggled(bool)", &slot_11, QMetaData::Public },
	{ "rbNside_toggled(bool)", &slot_12, QMetaData::Public },
	{ "languageChange()", &slot_13, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"ClDisplay", parentObject,
	slot_tbl, 14,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ClDisplay.setMetaObject( metaObj );
    return metaObj;
}

void* ClDisplay::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ClDisplay" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool ClDisplay::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: init(); break;
    case 1: UpdateS(); break;
    case 2: UpdateEvent(); break;
    case 3: UpdateDigits(); break;
    case 4: UpdateLayer(); break;
    case 5: UpdateTkIdRange(); break;
    case 6: UpdateTkId(); break;
    case 7: UpdateNP(); break;
    case 8: UpdateIclsRange(); break;
    case 9: UpdateIdx(); break;
    case 10: Exit(); break;
    case 11: rbPside_toggled((bool)static_QUType_bool.get(_o+1)); break;
    case 12: rbNside_toggled((bool)static_QUType_bool.get(_o+1)); break;
    case 13: languageChange(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool ClDisplay::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ClDisplay::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool ClDisplay::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
