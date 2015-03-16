#ifndef PROTOTYPESTORE_H
#define PROTOTYPESTORE_H

#include <QMap>
#include <QMetaObject>
#include <QtScript>

class PrototypeStore
{
public:
	static void setNullValue(QScriptValue null) { s_null = null; }
	
	static QScriptValue getPrototype(const QMetaObject* meta);
	static QScriptValue getPrototype(const char* className) { return s_prototypes[className]; }
	static bool hasPrototype(const char* className) { return s_prototypes.contains(className); }
	static void addPrototype(const char* className, const QScriptValue& proto) { s_prototypes[className] = proto; }

private:
	static QMap<const char*, QScriptValue> s_prototypes;
	static QScriptValue s_null;
};

#endif

