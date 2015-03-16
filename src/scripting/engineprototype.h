#ifndef ENGINEPROTOTYPE_H
#define ENGINEPROTOTYPE_H

#include <QtScript>

class GameState;
class GameEngine;

class EnginePrototype : public QObject, public QScriptable
{
	Q_OBJECT
	Q_PROPERTY(QScriptValue state READ state)
	Q_PROPERTY(QScriptValue initGameCallback READ initGameCallback WRITE setInitGameCallback)

public:
	EnginePrototype(GameEngine* engine);
	
	QScriptValue state() const;
	
	QScriptValue initGameCallback() const { return m_initGameCallback; }
	void setInitGameCallback(const QScriptValue& cb) { m_initGameCallback = cb; }
	void initGame();
	
	Q_INVOKABLE QString toString() const { return "GameEngine"; }

public slots:
	void stateChanged(GameState* state);

private:
	QScriptValue m_initGameCallback;
	GameEngine* m_engine;
};

#endif

