/****************************************************************************
** GonioAdv meta object code from reading C++ file 'gonioadv.h'
**
** Created: Wed Oct 24 14:30:11 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "gonioadv.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *GonioAdv::className() const
{
    return "GonioAdv";
}

QMetaObject *GonioAdv::metaObj = 0;
static QMetaObjectCleanUp cleanUp_GonioAdv( "GonioAdv", &GonioAdv::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString GonioAdv::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "GonioAdv", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString GonioAdv::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "GonioAdv", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* GonioAdv::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
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
	"GonioAdv", parentObject,
	slot_tbl, 7,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_GonioAdv.setMetaObject( metaObj );
    return metaObj;
}

void* GonioAdv::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "GonioAdv" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool GonioAdv::qt_invoke( int _id, QUObject* _o )
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
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool GonioAdv::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool GonioAdv::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool GonioAdv::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
