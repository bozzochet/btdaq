/****************************************************************************
** ErrorDialog meta object code from reading C++ file 'errordialog.h'
**
** Created: Thu Oct 25 00:28:06 2007
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "errordialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ErrorDialog::className() const
{
    return "ErrorDialog";
}

QMetaObject *ErrorDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ErrorDialog( "ErrorDialog", &ErrorDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ErrorDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ErrorDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ErrorDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ErrorDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ErrorDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "aa", &static_QUType_charstar, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"setError", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "aa", &static_QUType_QString, 0, QUParameter::InOut }
    };
    static const QUMethod slot_1 = {"setError", 1, param_slot_1 };
    static const QUMethod slot_2 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "setError(char*)", &slot_0, QMetaData::Public },
	{ "setError(QString&)", &slot_1, QMetaData::Public },
	{ "languageChange()", &slot_2, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"ErrorDialog", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ErrorDialog.setMetaObject( metaObj );
    return metaObj;
}

void* ErrorDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ErrorDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool ErrorDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setError((char*)static_QUType_charstar.get(_o+1)); break;
    case 1: setError((QString&)static_QUType_QString.get(_o+1)); break;
    case 2: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool ErrorDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ErrorDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool ErrorDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
