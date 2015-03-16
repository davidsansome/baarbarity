#include "vec2prototype.h"

#include <QtDebug>

vec2Prototype::vec2Prototype(QObject* parent)
	: QObject(parent)
{
}

float vec2Prototype::x() const
{
	vec2 value = qscriptvalue_cast<vec2>(thisObject());
	return value.x;
}

float vec2Prototype::y() const
{
	vec2 value = qscriptvalue_cast<vec2>(thisObject());
	return value.y;
}

float vec2Prototype::distanceTo(const vec2& other) const
{
	vec2 value = qscriptvalue_cast<vec2>(thisObject());
	return value.distanceTo(other);
}

QString vec2Prototype::toString() const
{
	return "vec2(" + QString::number(x()) + ", " + QString::number(y()) + ")";
}


QScriptValue vec2_ctor(QScriptContext* context, QScriptEngine* engine)
{
	float x = context->argument(0).toNumber();
	float y = context->argument(1).toNumber();
	
	return engine->toScriptValue(vec2(x, y));
}

