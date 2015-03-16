#ifndef RESOURCEREQUIREMENT_H
#define RESOURCEREQUIREMENT_H

#include "requirement.h"
#include "ability.h"
#include "player.h"

class ResourceRequirement : public Requirement
{
	Q_OBJECT
	Q_PROPERTY(int value READ value WRITE setValue)
	
public:
	ResourceRequirement(Ability* parent = NULL) : Requirement(parent) {}
	
	int value() const { return m_value; }
	void setValue(int value) { m_value = value; }
	
	bool isMet(const GameState* state, const Player* player, const WorldObject* object) const
	{
		return player->devotion() >= m_value;
	}
	
	QString description() const
	{
		return "Requires " + QString::number(m_value) + " resource.";
	}
	
private:
	int m_value;
};
Q_SCRIPT_DECLARE_QMETAOBJECT(ResourceRequirement, Ability*)
Q_DECLARE_METATYPE(ResourceRequirement*)

#endif
