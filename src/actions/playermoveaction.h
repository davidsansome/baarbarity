#ifndef PLAYERMOVEACTION_H
#define PLAYERMOVEACTION_H

#include "moveaction.h"
#include "worldobject.h"

class PlayerMoveAction : public MoveAction
{
	Q_OBJECT
	Q_PROPERTY(vec2 dest READ dest WRITE setDest)
	Q_PROPERTY(WorldObject* followObject READ followObject WRITE setFollowObject)
	Q_PROPERTY(float followDistance READ followDistance WRITE setFollowDistance)
	Q_PROPERTY(vec2 followOffset READ followOffset WRITE setFollowOffset)
	
public:
	PlayerMoveAction(WorldObject* parent);
	
	QString name() const { return "Listening to the voices"; }
	bool run(int timeDelta);
	
	vec2 dest() const { return m_dest; }
	void setDest(const vec2& dest) { m_dest = dest; }
	
	WorldObject* followObject() const { return m_followObject; }
	void setFollowObject(WorldObject* o);
	
	float followDistance() const { return m_followDistance; }
	void setFollowDistance(float d) { m_followDistance = d; }
	
	vec2 followOffset() const { return m_followOffset; }
	void setFollowOffset(const vec2& followOffset) { m_followOffset = followOffset; }

protected:
	void attached();

private:
	vec2 m_dest;
	WorldObject* m_followObject;
	int m_followTimer;
	float m_followDistance;
	vec2 m_followOffset;
};
Q_DECLARE_METATYPE(PlayerMoveAction*)

#endif
