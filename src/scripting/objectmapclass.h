#ifndef OBJECTMAPCLASS_H
#define OBJECTMAPCLASS_H

#include <QMap>
#include <QtScript>

#include "prototypestore.h"

template <class T>
class ObjectMapClass : public QScriptClass
{
public:
	typedef QMap<QString, T*> ObjectMap;
	
	ObjectMapClass(QScriptEngine* engine) : QScriptClass(engine)
	{
	}
	
	QString name() const
	{
		return "ObjectMap<" + QString(T::staticMetaObject.className()) + ">";
	}
	
	QueryFlags queryProperty(const QScriptValue& object, const QScriptString& name, QueryFlags flags, uint* id)
	{
		return flags & (HandlesReadAccess | HandlesWriteAccess);
	}
	
	QScriptValue::PropertyFlags propertyFlags(const QScriptValue& object, const QScriptString& name, uint id)
	{
		QScriptValue::PropertyFlags ret;
		ret = QScriptValue::Undeletable;
		
		ObjectMap* data = qscriptvalue_cast<ObjectMap*>(object.data());
		if (data->contains(name))
			ret |= QScriptValue::ReadOnly;
		
		return ret;
	}
	
	QScriptValue property(const QScriptValue& object, const QScriptString& name, uint id)
	{
		ObjectMap* data = qscriptvalue_cast<ObjectMap*>(object.data());
		if (data->contains(name))
		{
			T* o = (*data)[name];
			QScriptValue ret = engine()->newQObject(o, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject | QScriptEngine::AutoCreateDynamicProperties);
			ret.setPrototype(PrototypeStore::getPrototype(o->metaObject()));
			return ret;
		}
		
		return engine()->nullValue();
	}
	
	void setProperty(QScriptValue& object, const QScriptString& name, uint id, const QScriptValue& value)
	{
		ObjectMap* data = qscriptvalue_cast<ObjectMap*>(object.data());
		(*data)[name] = qscriptvalue_cast<T*>(value);
	}
};

#endif
