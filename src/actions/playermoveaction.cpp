#include "playermoveaction.h"

PlayerMoveAction::PlayerMoveAction(WorldObject* parent)
	: MoveAction(parent),
	  m_followObject(NULL),
	  m_followTimer(0),
	  m_followDistance(0.0)
{
}

void PlayerMoveAction::setFollowObject(WorldObject* o)
{
	m_followObject = o;
	m_dest = o->pos();
}

void PlayerMoveAction::attached()
{
	moveTo(m_dest);
	
	static const float arcRadius = 2.0;
	static const float arcLength = 0.8 * M_PI;
	static const float arcPerMember = arcLength / 4.0;
	
	const int count = m_worldObject->squadMembers().count();
	const vec2 travelVector(m_dest - m_worldObject->pos());
	float theta =  - float(count - 1.0) / 2.0 * arcPerMember - travelVector.angle() - M_PI_2;
	
	for (int i=0 ; i<5 && i<count ; ++i)
	{
		vec2 offset(arcRadius * sin(theta), arcRadius * cos(theta));
		theta += arcPerMember;
		
		WorldObject* squaddie = m_worldObject->squadMembers()[i];
		
		PlayerMoveAction* action = new PlayerMoveAction(squaddie);
		if (m_followObject)
		{
			action->setFollowObject(m_followObject);
			action->setFollowDistance(m_followDistance);
			action->setFollowOffset(offset);
		}
		else
		{
			action->setDest(m_dest + offset);
		}
		squaddie->setAction(action);
	}
}

bool PlayerMoveAction::run(int timeDelta)
{
	if (m_followObject)
	{
		m_dest = m_followObject->pos() + m_followOffset;
		
		if (m_worldObject->pos().distanceTo(m_dest) < m_followDistance)
			return false;
		
		m_followTimer += timeDelta;
		if (m_followTimer >= 500)
		{
			moveTo(m_dest);
			m_followTimer = 0;
		}
	}
	
	return MoveAction::run(timeDelta);
}
