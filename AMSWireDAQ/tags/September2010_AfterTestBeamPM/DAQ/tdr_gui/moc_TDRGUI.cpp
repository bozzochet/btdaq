/****************************************************************************
** Daq0 meta object code from reading C++ file 'TDRGUI.h'
**
** Created: Thu Oct 25 00:28:14 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "TDRGUI.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Daq0::className() const
{
    return "Daq0";
}

QMetaObject *Daq0::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Daq0( "Daq0", &Daq0::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Daq0::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Daq0", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Daq0::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Daq0", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Daq0::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_0 = {"pbGetLog_clicked", 1, param_slot_0 };
    static const QUMethod slot_1 = {"UpdateLog", 0, 0 };
    static const QUMethod slot_2 = {"pbClear_clicked", 0, 0 };
    static const QUMethod slot_3 = {"pbInit_clicked", 0, 0 };
    static const QUMethod slot_4 = {"pbStart_clicked", 0, 0 };
    static const QUMethod slot_5 = {"UpdateCount", 0, 0 };
    static const QUMethod slot_6 = {"pbStopLog_clicked", 0, 0 };
    static const QUMethod slot_7 = {"pbUpdate_clicked", 0, 0 };
    static const QUMethod slot_8 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "pbGetLog_clicked()", &slot_0, QMetaData::Public },
	{ "UpdateLog()", &slot_1, QMetaData::Public },
	{ "pbClear_clicked()", &slot_2, QMetaData::Public },
	{ "pbInit_clicked()", &slot_3, QMetaData::Public },
	{ "pbStart_clicked()", &slot_4, QMetaData::Public },
	{ "UpdateCount()", &slot_5, QMetaData::Public },
	{ "pbStopLog_clicked()", &slot_6, QMetaData::Public },
	{ "pbUpdate_clicked()", &slot_7, QMetaData::Public },
	{ "languageChange()", &slot_8, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"Daq0", parentObject,
	slot_tbl, 9,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Daq0.setMetaObject( metaObj );
    return metaObj;
}

void* Daq0::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Daq0" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool Daq0::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: static_QUType_bool.set(_o,pbGetLog_clicked()); break;
    case 1: UpdateLog(); break;
    case 2: pbClear_clicked(); break;
    case 3: pbInit_clicked(); break;
    case 4: pbStart_clicked(); break;
    case 5: UpdateCount(); break;
    case 6: pbStopLog_clicked(); break;
    case 7: pbUpdate_clicked(); break;
    case 8: languageChange(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool Daq0::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Daq0::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool Daq0::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
