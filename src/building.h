#ifndef BUILDING_H
#define BUILDING_H

#include "worldobject.h"

class BuildingType;

class Building : public WorldObject
{
	Q_OBJECT
	Q_PROPERTY(BuildingType* buildingType READ buildingType WRITE setBuildingType)
	
public:
	Building(GameState* gameState);
	
	QString name() const;
	float sightRadius() const { return 7.0f; }
	ObjectType* type() const { return s_objectType; }
	
	QList<Ability*> abilities() const;
	
	BuildingType* buildingType() const { return m_buildingType; }
	void setBuildingType(BuildingType* type) { m_buildingType = type; }

private:
	void init(GameState* state);
	
	static ObjectType* s_objectType;
	BuildingType* m_buildingType;
};
Q_DECLARE_METATYPE(Building*)

#endif

