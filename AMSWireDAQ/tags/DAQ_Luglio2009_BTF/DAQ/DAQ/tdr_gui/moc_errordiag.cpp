/****************************************************************************
** ErrorDiag meta object code from reading C++ file 'errordiag.h'
**
** Created: Thu Oct 25 00:28:02 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "errordiag.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ErrorDiag::className() const
{
    return "ErrorDiag";
}

QMetaObject *ErrorDiag::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ErrorDiag( "ErrorDiag", &ErrorDiag::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ErrorDiag::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ErrorDiag", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ErrorDiag::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ErrorDiag", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ErrorDiag::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"pushButton68_clicked", 0, 0 };
    static const QUMethod slot_1 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "pushButton68_clicked()", &slot_0, QMetaData::Public },
	{ "languageChange()", &slot_1, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"ErrorDiag", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ErrorDiag.setMetaObject( metaObj );
    return metaObj;
}

void* ErrorDiag::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ErrorDiag" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool ErrorDiag::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: pushButton68_clicked(); break;
    case 1: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool ErrorDiag::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ErrorDiag::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool ErrorDiag::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
