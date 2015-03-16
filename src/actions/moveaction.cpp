#include "moveaction.h"
#include "worldobject.h"

MoveAction::MoveAction(WorldObject* worldObject)
	: Action(worldObject)
{
	m_speed = 0.001f;
}

void MoveAction::moveBy(float angle, float distance)
{
	m_delta = vec2(cos(angle) * m_speed,
	                 sin(angle) * m_speed);

	m_moveTickCount = int(distance/m_speed);
	
	m_worldObject->setFacing(angle / M_PI * 180.0f);
}

void MoveAction::moveTo(const vec2& pos)
{
	vec2 diff = pos - m_worldObject->pos();
	float distance = diff.len();

	float angle;
	if (diff.x == 0.0f) angle = diff.y > 0.0f ? 0.0f : M_PI;
	else angle = atan(diff.y / diff.x);
	
	if (diff.x < 0.0f)
		angle += M_PI;
	
	moveBy(angle, distance);
}

bool MoveAction::run(int timeDelta)
{
	m_worldObject->move(m_delta * timeDelta);

	m_moveTickCount -= timeDelta;
	return (m_moveTickCount > 0);
}
