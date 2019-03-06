/****************************************************************************
** AboutForm meta object code from reading C++ file 'AboutForm.h'
**
** Created: Tue Feb 22 18:29:55 2011
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/AboutForm.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *AboutForm::className() const
{
    return "AboutForm";
}

QMetaObject *AboutForm::metaObj = 0;
static QMetaObjectCleanUp cleanUp_AboutForm( "AboutForm", &AboutForm::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString AboutForm::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "AboutForm", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString AboutForm::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "AboutForm", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* AboutForm::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "x", &static_QUType_int, 0, QUParameter::In },
	{ "y", &static_QUType_int, 0, QUParameter::In },
	{ "ptr", &static_QUType_ptr, "AboutForm*", QUParameter::InOut }
    };
    static const QUMethod slot_0 = {"showForm", 3, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "event", &static_QUType_ptr, "QCloseEvent", QUParameter::In }
    };
    static const QUMethod slot_1 = {"closeEvent", 1, param_slot_1 };
    static const QUMethod slot_2 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "showForm(int,int,AboutForm**)", &slot_0, QMetaData::Public },
	{ "closeEvent(QCloseEvent*)", &slot_1, QMetaData::Public },
	{ "languageChange()", &slot_2, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"AboutForm", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_AboutForm.setMetaObject( metaObj );
    return metaObj;
}

void* AboutForm::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "AboutForm" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool AboutForm::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: showForm((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2),(AboutForm**)static_QUType_ptr.get(_o+3)); break;
    case 1: closeEvent((QCloseEvent*)static_QUType_ptr.get(_o+1)); break;
    case 2: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool AboutForm::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool AboutForm::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool AboutForm::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
