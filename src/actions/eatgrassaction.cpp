#include "eatgrassaction.h"

EatGrassAction::EatGrassAction(WorldObject* parent)
	: MoveAction(parent),
	  m_state(State_Thinking)
{
}

void EatGrassAction::attached()
{
	m_tickCount = m_thinkingDelay + m_eatingDelay;
}

bool EatGrassAction::run(int timeDelta)
{
	m_tickCount -= timeDelta;

	if (m_tickCount <= 0)
		return false;
	
	switch (m_state)
	{
	case State_Thinking:
		if (m_tickCount <= m_eatingDelay)
		{
			m_state = State_Moving;
			moveTo(m_dest);
		}
		break;
	case State_Moving:
		if (!MoveAction::run(timeDelta))
		{
			m_state = State_Eating;
			m_tickCount = m_eatingDelay + m_moveTickCount;
		}
		break;
	case State_Eating:
		break;
	}
	
	return true;
}

QString EatGrassAction::name() const
{
	switch (m_state)
	{
	case State_Thinking: return "Thinking about grass";
	case State_Moving: return "Looking for grass to eat";
	case State_Eating: return "Eating grass";
	}
	
	return QString::null;
}
