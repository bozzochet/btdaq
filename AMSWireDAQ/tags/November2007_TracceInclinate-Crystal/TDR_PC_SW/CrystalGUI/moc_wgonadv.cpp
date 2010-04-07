/****************************************************************************
** wGonAdv meta object code from reading C++ file 'wgonadv.h'
**
** Created: Sun Aug 26 19:28:57 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "wgonadv.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *wGonAdv::className() const
{
    return "wGonAdv";
}

QMetaObject *wGonAdv::metaObj = 0;
static QMetaObjectCleanUp cleanUp_wGonAdv( "wGonAdv", &wGonAdv::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString wGonAdv::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "wGonAdv", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString wGonAdv::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "wGonAdv", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* wGonAdv::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUMethod slot_0 = {"pbGonSetPos_clicked", 0, 0 };
    static const QUMethod slot_1 = {"pbGonHwCal_clicked", 0, 0 };
    static const QUMethod slot_2 = {"pbGonZero_clicked", 0, 0 };
    static const QUMethod slot_3 = {"pbGonAbort_clicked", 0, 0 };
    static const QUMethod slot_4 = {"pbGonSendRaw_clicked", 0, 0 };
    static const QUMethod slot_5 = {"pbGonSwCal_clicked", 0, 0 };
    static const QUMethod slot_6 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "pbGonSetPos_clicked()", &slot_0, QMetaData::Public },
	{ "pbGonHwCal_clicked()", &slot_1, QMetaData::Public },
	{ "pbGonZero_clicked()", &slot_2, QMetaData::Public },
	{ "pbGonAbort_clicked()", &slot_3, QMetaData::Public },
	{ "pbGonSendRaw_clicked()", &slot_4, QMetaData::Public },
	{ "pbGonSwCal_clicked()", &slot_5, QMetaData::Public },
	{ "languageChange()", &slot_6, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"wGonAdv", parentObject,
	slot_tbl, 7,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_wGonAdv.setMetaObject( metaObj );
    return metaObj;
}

void* wGonAdv::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "wGonAdv" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool wGonAdv::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: pbGonSetPos_clicked(); break;
    case 1: pbGonHwCal_clicked(); break;
    case 2: pbGonZero_clicked(); break;
    case 3: pbGonAbort_clicked(); break;
    case 4: pbGonSendRaw_clicked(); break;
    case 5: pbGonSwCal_clicked(); break;
    case 6: languageChange(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool wGonAdv::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool wGonAdv::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool wGonAdv::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
