#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H

#include <QObject>
#include <QtScript>
#include <QList>

#include "vector.h"

class Ability;
class ScriptManager;
class WorldObject;
class Action;

class ObjectType : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_PROPERTY(QScriptValue actionCompletedCallback READ actionCompletedCallback WRITE setActionCompletedCallback)
	Q_PROPERTY(QScriptValue contextActionCallback READ contextActionCallback WRITE setContextActionCallback)
	Q_PROPERTY(QList<Ability*> abilities READ abilities)

public:
	ObjectType(QObject* parent = NULL);
	
	QScriptValue actionCompletedCallback() const { return m_actionCompletedCallback; }
	void setActionCompletedCallback(const QScriptValue& value) { m_actionCompletedCallback = value; }
	
	QScriptValue contextActionCallback() const { return m_contextActionCallback; }
	void setContextActionCallback(const QScriptValue& value) { m_contextActionCallback = value; }
	
	QList<Ability*> abilities() const { return m_abilities; }
	void addAbility(Ability* ability) { m_abilities << ability; }
	
	void actionCompleted(WorldObject* thisObject, Action* lastAction);
	void startContextAction(WorldObject* thisObject, const vec2& target);
	void startContextAction(WorldObject* thisObject, WorldObject* target);

private:
	QScriptValue m_actionCompletedCallback;
	QScriptValue m_contextActionCallback;
	QList<Ability*> m_abilities;
};
Q_DECLARE_METATYPE(ObjectType*)

#endif
