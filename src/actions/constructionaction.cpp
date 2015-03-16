#include "constructionaction.h"

ConstructionAction::ConstructionAction(WorldObject* parent)
	: Action(parent),
	  m_duration(0.0),
	  m_billboard(NULL)
{
}

bool ConstructionAction::run(int timeDelta)
{
	m_elapsed += float(timeDelta) / 1000.0;
	
	return m_elapsed < m_duration;
}

float ConstructionAction::progress() const
{
	return m_elapsed / m_duration;
}

bool ConstructionAction::interrupt(Action* interruptingAction)
{
	if (qobject_cast<ConstructionAction*>(interruptingAction))
		return false;
	return true;
}
