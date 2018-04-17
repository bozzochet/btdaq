/****************************************************************************
** outputForm meta object code from reading C++ file 'outputForm.h'
**
** Created: Tue Feb 22 18:29:54 2011
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "../.ui/outputForm.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *outputForm::className() const
{
    return "outputForm";
}

QMetaObject *outputForm::metaObj = 0;
static QMetaObjectCleanUp cleanUp_outputForm( "outputForm", &outputForm::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString outputForm::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "outputForm", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString outputForm::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "outputForm", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* outputForm::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "x", &static_QUType_int, 0, QUParameter::In },
	{ "y", &static_QUType_int, 0, QUParameter::In },
	{ "height", &static_QUType_int, 0, QUParameter::In },
	{ "ptr", &static_QUType_ptr, "outputForm*", QUParameter::InOut }
    };
    static const QUMethod slot_0 = {"showForm", 4, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "str", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"setText", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "theData", &static_QUType_ptr, "unsigned char", QUParameter::In },
	{ "dataLength", &static_QUType_ptr, "unsigned long", QUParameter::In }
    };
    static const QUMethod slot_2 = {"showByteData", 2, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ "event", &static_QUType_ptr, "QCloseEvent", QUParameter::In }
    };
    static const QUMethod slot_3 = {"closeEvent", 1, param_slot_3 };
    static const QUMethod slot_4 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "showForm(int,int,int,outputForm**)", &slot_0, QMetaData::Public },
	{ "setText(QString)", &slot_1, QMetaData::Public },
	{ "showByteData(unsigned char*,unsigned long)", &slot_2, QMetaData::Public },
	{ "closeEvent(QCloseEvent*)", &slot_3, QMetaData::Public },
	{ "languageChange()", &slot_4, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"outputForm", parentObject,
	slot_tbl, 5,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_outputForm.setMetaObject( metaObj );
    return metaObj;
}

void* outputForm::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "outputForm" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool outputForm::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: showForm((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2),(int)static_QUType_int.get(_o+3),(outputForm**)static_QUType_ptr.get(_o+4)); break;
    case 1: setText((QString)static_QUType_QString.get(_o+1)); break;
    case 2: showByteData((unsigned char*)static_QUType_ptr.get(_o+1),(unsigned long)(*((unsigned long*)static_QUType_ptr.get(_o+2)))); break;
    case 3: closeEvent((QCloseEvent*)static_QUType_ptr.get(_o+1)); break;
    case 4: languageChange(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool outputForm::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool outputForm::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool outputForm::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
