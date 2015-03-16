#include "prototypestore.h"

QMap<const char*, QScriptValue> PrototypeStore::s_prototypes;
QScriptValue PrototypeStore::s_null;

QScriptValue PrototypeStore::getPrototype(const QMetaObject* meta)
{
	while (meta)
	{
		if (s_prototypes.contains(meta->className()))
			return s_prototypes[meta->className()];
		meta = meta->superClass();
	}
	return s_null;
}
