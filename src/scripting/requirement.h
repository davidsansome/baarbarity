#ifndef REQUIREMENT_H
#define REQUIREMENT_H

#include <QObject>
#include <QString>
#include <QtScript>

class GameState;
class Player;
class WorldObject;
class Ability;

class Requirement : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_PROPERTY(QString description READ description)
	
public:
	Requirement(Ability* parent);
	
	Q_INVOKABLE virtual bool isMet(const GameState* state, const Player* player, const WorldObject* object) const = 0;
	virtual QString description() const = 0;
};
Q_DECLARE_METATYPE(Requirement*)

#endif
