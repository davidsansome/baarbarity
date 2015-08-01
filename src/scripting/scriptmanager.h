#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include "objectmapclass.h"

#include <QtScript>
#include <QVariantMap>
#include <QMetaType>

class QString;
class QObject;
class ObjectType;
class PlayerType;
class BuildingType;
class GameEngine;
class GameHud;

typedef QMap<QString, ObjectType*> ObjectTypeMap;
typedef QMap<QString, PlayerType*> PlayerTypeMap;
typedef QMap<QString, BuildingType*> BuildingTypeMap;

Q_DECLARE_METATYPE(ObjectTypeMap*);
Q_DECLARE_METATYPE(PlayerTypeMap*);
Q_DECLARE_METATYPE(BuildingTypeMap*);

class ScriptManager
{
public:
	static ObjectType* objectType(const QString& name) { return s_objectTypes[name]; }
	static PlayerType* playerType(const QString& name) { return s_playerTypes[name]; }
	static BuildingType* buildingType(const QString& name) { return s_buildingTypes[name]; }
	
	static void init(GameEngine* engine);
	
	static void handleExceptions();
	static void loadAllScripts();
	static void loadScript(const QString& filename);
	
	static void setGameHud(GameHud* gameHud) { s_gameHud = gameHud; }
	
	static GameHud* gameHud() { return s_gameHud; }

private:
	ScriptManager();
	
	static void registerObjectType(const QString& name, ObjectType* value);
	static void registerPlayerType(const QString& name, PlayerType* value);
	
	template <typename T>
	static void addClass(QScriptEngine::FunctionSignature ctor);
	
	template <typename T>
	static void addPrototype(QObject* prototype, QScriptEngine::FunctionSignature constructor, const QString& name = QString::null);
	
	template <typename T>
	static void addObjectMap(const QString& name, ObjectMapClass<T>* clazz, QMap<QString, T*>* map);
	
	static QScriptEngine* s_engine;
	static ObjectMapClass<ObjectType>* s_objectTypesClass;
	static ObjectMapClass<PlayerType>* s_playerTypesClass;
	static ObjectMapClass<BuildingType>* s_buildingTypesClass;
	static ObjectTypeMap s_objectTypes;
	static PlayerTypeMap s_playerTypes;
	static BuildingTypeMap s_buildingTypes;
	
	static GameHud* s_gameHud;
};

#endif
