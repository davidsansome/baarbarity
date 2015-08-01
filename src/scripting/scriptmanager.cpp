#include "scriptmanager.h"
#include "ability.h"
#include "resourcerequirement.h"
#include "objecttype.h"
#include "playertype.h"
#include "buildingtype.h"
#include "worldobject.h"
#include "vec2prototype.h"
#include "actions/playermoveaction.h"
#include "actions/eatgrassaction.h"
#include "actions/constructionaction.h"
#include "sheep.h"
#include "welshman.h"
#include "gamestate.h"
#include "gameengine.h"
#include "colorprototype.h"
#include "engineprototype.h"
#include "terrain.h"
#include "building.h"
#include "billboard.h"
#include "timer.h"
#include "gui/gamehud.h"

#include <QtScript>
#include <QString>
#include <iostream>


QScriptEngine* ScriptManager::s_engine;
ObjectMapClass<ObjectType>* ScriptManager::s_objectTypesClass;
ObjectMapClass<PlayerType>* ScriptManager::s_playerTypesClass;
ObjectMapClass<BuildingType>* ScriptManager::s_buildingTypesClass;
ObjectTypeMap ScriptManager::s_objectTypes;
PlayerTypeMap ScriptManager::s_playerTypes;
BuildingTypeMap ScriptManager::s_buildingTypes;
GameHud* ScriptManager::s_gameHud = NULL;


QScriptValue scriptDebug(QScriptContext *context, QScriptEngine *engine)
{
	QString result;
	for (int i = 0; i < context->argumentCount(); ++i)
	{
		if (i > 0)
			result.append(" ");
		result.append(context->argument(i).toString());
	}
	
	if (!ScriptManager::gameHud())
		std::cerr << result.toAscii().data() << std::endl;
	else
		ScriptManager::gameHud()->appendChatLine("<font color=\"red\">[debug]</font> " + result);
	
	return engine->undefinedValue();
}


template <typename T>
QScriptValue qobjToScriptValue(QScriptEngine* engine, const T& o)
{
	if (o == NULL)
		return engine->nullValue();
	
	QScriptValue ret = engine->newQObject(o, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject | QScriptEngine::AutoCreateDynamicProperties);
	ret.setPrototype(PrototypeStore::getPrototype(o->metaObject()));
	
	return ret;
}

template <typename T>
void qobjFromScriptValue(const QScriptValue& v, T& o)
{
	o = qobject_cast<T>(v.toQObject());
}

template <typename T>
QScriptValue wrapQObject(QScriptEngine* engine, T* o)
{
	QScriptValue ret = engine->newQObject(o, QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject | QScriptEngine::AutoCreateDynamicProperties);
	ret.setPrototype(PrototypeStore::getPrototype(&T::staticMetaObject));
	return ret;
}

template <typename T, typename P>
QScriptValue qobjCtor(QScriptContext* context, QScriptEngine* engine)
{
	P parent = qscriptvalue_cast<P>(context->argument(0));
	return wrapQObject(engine, new T(parent));
}

QScriptValue abstractCtor(QScriptContext* context, QScriptEngine* engine)
{
	context->throwError("You cannot construct an instance of an abstract class.  Construct one of the subclasses instead.");
	return QScriptValue();
}

QScriptValue dummyCtor(QScriptContext* context, QScriptEngine* engine)
{
	context->throwError("You cannot construct an instance of this class.");
	return QScriptValue();
}

template <typename T>
void ScriptManager::addClass(QScriptEngine::FunctionSignature ctor)
{
	typedef T* Tp;
	const QMetaObject& metaObj = T::staticMetaObject;
	const char* className = metaObj.className();
	const char* superClassName = metaObj.superClass() ? metaObj.superClass()->className() : 0;
	
	// Create a constructor function
	QScriptValue ctorValue = s_engine->newFunction(ctor);
	
	// Create a prototype
	QScriptValue prototype = s_engine->newObject();
	
	// If this class inherits something, set the parent prototype
	if (superClassName && PrototypeStore::hasPrototype(superClassName))
		prototype.setPrototype(PrototypeStore::getPrototype(superClassName));
	
	// Store stuff
	PrototypeStore::addPrototype(className, prototype);
	ctorValue.setProperty("prototype", prototype);
	s_engine->globalObject().setProperty(className, ctorValue);
	
	// Register converter functions
	qScriptRegisterMetaType(s_engine, qobjToScriptValue<Tp>, qobjFromScriptValue<Tp>);
}

template <typename T>
void ScriptManager::addPrototype(QObject* prototype, QScriptEngine::FunctionSignature constructor, const QString& name)
{
	QScriptValue proto(s_engine->newQObject(prototype));
	s_engine->setDefaultPrototype(qMetaTypeId<T>(), proto);
	
	QScriptValue ctor(s_engine->newFunction(constructor));
	ctor.setProperty("prototype", proto);
	
	QString typeName(name.isNull() ? QMetaType::typeName(qMetaTypeId<T>()) : name);
	if (typeName.endsWith("*"))
		typeName.chop(1);
	
	s_engine->globalObject().setProperty(typeName, ctor);
}


template <typename T>
void ScriptManager::addObjectMap(const QString& name, ObjectMapClass<T>* clazz, QMap<QString, T*>* map)
{
	QScriptValue value = s_engine->newObject(clazz, s_engine->toScriptValue(map));
	s_engine->globalObject().setProperty(name, value, QScriptValue::ReadOnly | QScriptValue::Undeletable);
}


QScriptValue randBetween(QScriptContext* context, QScriptEngine* engine)
{
	if (context->argumentCount() != 2 ||
	    !context->argument(0).isNumber() ||
	    !context->argument(1).isNumber())
		context->throwError("Invalid arguments to randBetween(min, max)");
	
	qsreal min = context->argument(0).toNumber();
	qsreal max = context->argument(1).toNumber();
	
	return QScriptValue(engine, min + (float(qrand()) / RAND_MAX) * (max-min));
}

void ScriptManager::init(GameEngine* engine)
{
	s_engine = new QScriptEngine;
	s_objectTypesClass = new ObjectMapClass<ObjectType>(s_engine);
	s_playerTypesClass = new ObjectMapClass<PlayerType>(s_engine);
	s_buildingTypesClass = new ObjectMapClass<BuildingType>(s_engine);

	PrototypeStore::setNullValue(s_engine->nullValue());
	
	qRegisterMetaType<Action*>("Action*");
	qRegisterMetaType<GameState*>("GameState*");
	qRegisterMetaType<Terrain*>("Terrain*");
	
	// Abstract classes
	addClass<Requirement>(abstractCtor);
	addClass<Action>(abstractCtor);
	addClass<WorldObject>(abstractCtor);
	
	// Real classes that cannot be instantiated from the scripting environment
	addClass<GameState>(dummyCtor);
	addClass<Terrain>(dummyCtor);
	
	// Real classes that can be instantiated from the scripting environment
	addClass<ObjectType>(qobjCtor<ObjectType, QObject*>);
	addClass<PlayerType>(qobjCtor<PlayerType, QObject*>);
	addClass<BuildingType>(qobjCtor<BuildingType, QObject*>);
	addClass<Ability>(qobjCtor<Ability, QObject*>);
	addClass<ResourceRequirement>(qobjCtor<ResourceRequirement, Ability*>);
	addClass<EatGrassAction>(qobjCtor<EatGrassAction, WorldObject*>);
	addClass<PlayerMoveAction>(qobjCtor<PlayerMoveAction, WorldObject*>);
	addClass<ConstructionAction>(qobjCtor<ConstructionAction, WorldObject*>);
	addClass<Sheep>(qobjCtor<Sheep, GameState*>);
	addClass<Player>(qobjCtor<Player, GameState*>);
	addClass<Welshman>(qobjCtor<Welshman, GameState*>);
	addClass<Building>(qobjCtor<Building, GameState*>);
	addClass<Billboard>(qobjCtor<Billboard, WorldObject*>);
	addClass<Timer>(qobjCtor<Timer, WorldObject*>);
	
	addPrototype<vec2>(new vec2Prototype(), vec2_ctor);
	addPrototype<QColor>(new ColorPrototype(), Color_ctor);
	addPrototype<GameEngine*>(new EnginePrototype(engine), dummyCtor);
	
	addObjectMap("ObjectTypes", s_objectTypesClass, &s_objectTypes);
	addObjectMap("PlayerTypes", s_playerTypesClass, &s_playerTypes);
	addObjectMap("BuildingTypes", s_buildingTypesClass, &s_buildingTypes);
	
	s_engine->globalObject().setProperty("print", s_engine->newFunction(scriptDebug));
	s_engine->globalObject().setProperty("randBetween", s_engine->newFunction(randBetween));
	
	s_engine->globalObject().setProperty("Engine", s_engine->newQObject(engine));
}

void ScriptManager::loadAllScripts()
{
	QDir scriptsDir(":/data/scripts/");
	QStringList scripts(scriptsDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name));
	
	foreach (const QString& script, scripts)
		loadScript(scriptsDir.filePath(script));
}

void ScriptManager::loadScript(const QString& filename)
{
	QFile scriptFile(filename);
	if (!scriptFile.open(QIODevice::ReadOnly))
		qFatal("Could not open script: %s\n", filename.toAscii().data());
	
	QTextStream stream(&scriptFile);
	QString contents = stream.readAll();
	scriptFile.close();
	s_engine->evaluate(contents, filename);
	handleExceptions();
}

void ScriptManager::handleExceptions()
{
	if (s_engine->hasUncaughtException())
	{
		QStringList backtrace(s_engine->uncaughtExceptionBacktrace());
		
		qDebug() << "QtScript Exception:" << s_engine->uncaughtException().toString();
		foreach (const QString& line, backtrace)
			qDebug() << "    " << line;
		s_engine->clearExceptions();
	}
}
