#ifndef ACTION_H
#define ACTION_H

#include <QObject>
#include <QtScript>

class WorldObject;
class GameState;

class Action : public QObject
{
	Q_OBJECT

public:
	Action(WorldObject* parent);
	virtual ~Action() {}
	
	// Called when the action is attached to an object
	virtual void attached() {}
	
	virtual QString name() const { return QString::null; }
	
	virtual bool hasProgressBar() const { return false; }
	virtual float progress() const { return 0.0; }

	// Return true if the action wants to keep running
	virtual bool run(int timeDelta) = 0;

	// Return true if it's ok to be interrupted
	virtual bool interrupt(Action* interruptingAction) { return true; }

protected:
	
	WorldObject* m_worldObject;
	GameState* m_gameState;
};
Q_DECLARE_METATYPE(Action*);

#endif
