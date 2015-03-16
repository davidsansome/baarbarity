#include "ability.h"
#include "scriptmanager.h"
#include "requirement.h"
#include "objecttype.h"
#include "playertype.h"
#include "buildingtype.h"
#include "resourcerequirement.h"
#include "worldobject.h"

Ability::Ability(QObject* parent)
	: QObject(parent),
	  m_cost(0)
{
	ObjectType* objectType = qobject_cast<ObjectType*>(parent);
	PlayerType* playerType = qobject_cast<PlayerType*>(parent);
	BuildingType* buildingType = qobject_cast<BuildingType*>(parent);
	
	if (objectType)
		objectType->addAbility(this);
	else if (playerType)
		playerType->addAbility(this);
	else if (buildingType)
		buildingType->addAbility(this);
	else
		qDebug() << "Warning: Ability created without a parent, or parent wasn't an ObjectType, PlayerType or BuildingType.";
}

bool Ability::areRequirementsMet(const GameState* state, const Player* player, const WorldObject* object) const
{
	foreach (Requirement* req, m_requirements)
		if (!req->isMet(state, player, object))
			return false;
	return true;
}

void Ability::invoke(WorldObject* thisObject)
{
	QScriptValue object = m_callback.engine()->toScriptValue(thisObject);
	
	m_callback.call(object);
	ScriptManager::handleExceptions();
}

int Ability::cost() const
{
	foreach (Requirement* req, m_requirements)
	{
		ResourceRequirement* res = qobject_cast<ResourceRequirement*>(req);
		if (res)
			return res->value();
	}
	return 0;
}

