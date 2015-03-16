#include "engineprototype.h"
#include "gameengine.h"
#include "scriptmanager.h"
#include "gamestate.h"

#include <QtDebug>

EnginePrototype::EnginePrototype(GameEngine* engine)
	: QObject(NULL),
	  m_engine(engine)
{
	connect(m_engine, SIGNAL(stateChanged(GameState*)), SLOT(stateChanged(GameState*)));
}

QScriptValue EnginePrototype::state() const
{
	return engine()->newQObject(m_engine->gameState(), QScriptEngine::QtOwnership, QScriptEngine::PreferExistingWrapperObject | QScriptEngine::AutoCreateDynamicProperties);
}

void EnginePrototype::stateChanged(GameState* state)
{
	initGame();
}

void EnginePrototype::initGame()
{
	m_initGameCallback.call();
	ScriptManager::handleExceptions();
}

