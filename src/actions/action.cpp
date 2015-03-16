#include "action.h"
#include "worldobject.h"

Action::Action(WorldObject* parent)
	: QObject(parent)
{
	if (!parent)
		qDebug() << "Warning: Action created without a parent.  Probably going to crash soon.";
	else
	{
		m_worldObject = parent;
		m_gameState = parent->gameState();
	}
}
