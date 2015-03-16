#ifndef COLORPROTOTYPE_H
#define COLORPROTOTYPE_H

#include <QtScript>
#include <QColor>

class ColorPrototype : public QObject, public QScriptable
{
	Q_OBJECT
	Q_PROPERTY(float r READ r)
	Q_PROPERTY(float g READ g)
	Q_PROPERTY(float b READ b)

public:
	ColorPrototype(QObject* parent = NULL);
	
	float r() const;
	float g() const;
	float b() const;
};

QScriptValue Color_ctor(QScriptContext* context, QScriptEngine* engine);

#endif
