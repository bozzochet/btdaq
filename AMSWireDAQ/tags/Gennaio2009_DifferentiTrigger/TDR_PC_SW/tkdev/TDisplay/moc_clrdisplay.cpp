/****************************************************************************
** ClRDisplay meta object code from reading C++ file 'clrdisplay.h'
**
** Created: Tue Dec 23 11:26:56 2008
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.3   edited Aug 5 16:40 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "clrdisplay.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ClRDisplay::className() const
{
    return "ClRDisplay";
}

QMetaObject *ClRDisplay::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ClRDisplay( "ClRDisplay", &ClRDisplay::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ClRDisplay::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ClRDisplay", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ClRDisplay::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ClRDisplay", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ClRDisplay::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUMethod slot_0 = {"UpdateS", 0, 0 };
    static const QUMethod slot_1 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "UpdateS()", &slot_0, QMetaData::Public },
	{ "languageChange()", &slot_1, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"ClRDisplay", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ClRDisplay.setMetaObject( metaObj );
    return metaObj;
}

void* ClRDisplay::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ClRDisplay" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool ClRDisplay::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: UpdateS(); break;
    case 1: languageChange(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool ClRDisplay::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ClRDisplay::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool ClRDisplay::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
