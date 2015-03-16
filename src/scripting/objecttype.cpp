#include "objecttype.h"
#include "scriptmanager.h"
#include "worldobject.h"
#include "actions/action.h"

#include <QtDebug>

ObjectType::ObjectType(QObject* parent)
	: QObject(parent)
{
}

void ObjectType::actionCompleted(WorldObject* thisObject, Action* lastAction)
{
	QScriptValue object = m_actionCompletedCallback.engine()->toScriptValue(thisObject);
	
	QScriptValue actionObject;
	if (lastAction)
		actionObject = m_actionCompletedCallback.engine()->toScriptValue(lastAction);
	
	m_actionCompletedCallback.call(object, QScriptValueList() << actionObject);
	ScriptManager::handleExceptions();
}

void ObjectType::startContextAction(WorldObject* thisObject, const vec2& target)
{
	QScriptValue object = m_contextActionCallback.engine()->toScriptValue(thisObject);
	QScriptValue targetObject = m_contextActionCallback.engine()->toScriptValue(target);
	
	m_contextActionCallback.call(object, QScriptValueList() << targetObject);
	ScriptManager::handleExceptions();
}

void ObjectType::startContextAction(WorldObject* thisObject, WorldObject* target)
{
	QScriptValue object = m_contextActionCallback.engine()->toScriptValue(thisObject);
	
	QScriptValue targetObject;
	if (target)
		targetObject = m_contextActionCallback.engine()->toScriptValue(target);
	
	m_contextActionCallback.call(object, QScriptValueList() << targetObject);
	ScriptManager::handleExceptions();
}
