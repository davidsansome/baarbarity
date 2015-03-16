#include "colorprototype.h"

ColorPrototype::ColorPrototype(QObject* parent)
	: QObject(parent)
{
}

float ColorPrototype::r() const
{
	QColor value = qscriptvalue_cast<QColor>(thisObject());
	return value.red();
}

float ColorPrototype::g() const
{
	QColor value = qscriptvalue_cast<QColor>(thisObject());
	return value.green();
}

float ColorPrototype::b() const
{
	QColor value = qscriptvalue_cast<QColor>(thisObject());
	return value.blue();
}

QScriptValue Color_ctor(QScriptContext* context, QScriptEngine* engine)
{
	float r = context->argument(0).toNumber();
	float g = context->argument(1).toNumber();
	float b = context->argument(2).toNumber();
	return engine->toScriptValue(QColor(r, g, b));
}

