#ifndef MOVEACTION_H
#define MOVEACTION_H

#include "action.h"
#include "vector.h"

class MoveAction : public Action
{
public:
	virtual bool run(int timeDelta);

protected:
	MoveAction(WorldObject* parent);
	virtual ~MoveAction() {}

	void setSpeed(float speed) { m_speed = speed; }
	void moveTo(const vec2& pos);
	void moveBy(float angle, float distance);

	int m_moveTickCount;

private:
	vec2 m_delta;
	float m_speed;
};

#endif
