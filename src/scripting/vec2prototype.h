#ifndef VEC2PROTOTYPE_H
#define VEC2PROTOTYPE_H

#include <QtScript>

#include "vector.h"

class vec2Prototype : public QObject, public QScriptable
{
	Q_OBJECT
	Q_PROPERTY(float x READ x)
	Q_PROPERTY(float y READ y)

public:
	vec2Prototype(QObject* parent = NULL);
	
	float x() const;
	float y() const;
	
	Q_INVOKABLE float distanceTo(const vec2& other) const;
	Q_INVOKABLE QString toString() const;
};

QScriptValue vec2_ctor(QScriptContext* context, QScriptEngine* engine);

#endif
