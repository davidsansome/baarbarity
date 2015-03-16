#include "building.h"
#include "scripting/buildingtype.h"
#include "scripting/scriptmanager.h"
#include "modelloader.h"

ObjectType* Building::s_objectType = NULL;

Building::Building(GameState* gameState)
	: WorldObject(gameState)
{
	m_buildingType = NULL;
	if (s_objectType == NULL)
		s_objectType = ScriptManager::objectType("Building");
	
	m_model = ModelLoader::loadModel("TestModel", 1.0);
	loadPortrait(":/data/portraits/sheep.jpg");
}

QString Building::name() const
{
	return m_buildingType->name();
}

QList<Ability*> Building::abilities() const
{
	return m_buildingType->abilities();
}

