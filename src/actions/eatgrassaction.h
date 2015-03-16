#ifndef EATGRASSACTION_H
#define EATGRASSACTION_H

#include "moveaction.h"
#include "worldobject.h"

class EatGrassAction : public MoveAction
{
	Q_OBJECT
	Q_PROPERTY(vec2 dest READ dest WRITE setDest)
	Q_PROPERTY(int thinkingDelay READ thinkingDelay WRITE setThinkingDelay)
	Q_PROPERTY(int eatingDelay READ eatingDelay WRITE setEatingDelay)
	
public:
	EatGrassAction(WorldObject* parent);
	
	QString name() const;
	bool run(int timeDelta);
	
	vec2 dest() const { return m_dest; }
	void setDest(const vec2& dest) { m_dest = dest; }
	
	int thinkingDelay() const { return m_thinkingDelay; }
	void setThinkingDelay(int thinkingDelay) { m_thinkingDelay = thinkingDelay; }
	
	int eatingDelay() const { return m_eatingDelay; }
	void setEatingDelay(int eatingDelay) { m_eatingDelay = eatingDelay; }

protected:
	void attached();

private:
	enum State
	{
		State_Thinking = 0,
		State_Moving = 1,
		State_Eating = 2
	} m_state;

	vec2 m_dest;
	int m_thinkingDelay;
	int m_eatingDelay;
	int m_tickCount;
};
Q_DECLARE_METATYPE(EatGrassAction*)

#endif
