#ifndef ABILITY_H
#define ABILITY_H

#include <QList>
#include <QString>
#include <QMap>
#include <QtScript>
#include <QtDebug>

class Requirement;
class GameState;
class Player;
class WorldObject;

class Ability : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(int cost READ cost)
	Q_PROPERTY(QList<Requirement*> requirements READ requirements)
	Q_PROPERTY(QScriptValue callback READ callback WRITE setCallback)
	
public:
	Ability(QObject* parent);
	
	QString name() const { return m_name; }
	void setName(const QString& name) { m_name = name; }
	
	QString description() const { return m_description; }
	void setDescription(const QString& description) { m_description = description; }
	
	int cost() const;
	
	QList<Requirement*> requirements() const { return m_requirements; }
	void addRequirement(Requirement* requirement) { m_requirements << requirement; }
	Q_INVOKABLE bool areRequirementsMet(const GameState* state, const Player* player, const WorldObject* object) const;
	
	QScriptValue callback() const { return m_callback; }
	void setCallback(const QScriptValue& callback) { m_callback = callback; }

	void invoke(WorldObject* thisObject);

private:
	QString m_name;
	QString m_description;
	int m_cost;
	QList<Requirement*> m_requirements;
	QScriptValue m_callback;
};
Q_DECLARE_METATYPE(Ability*)


#endif
