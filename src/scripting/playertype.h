#ifndef PLAYERTYPE_H
#define PLAYERTYPE_H

#include <QObject>
#include <QScriptable>
#include <QList>
#include <QMetaType>

class Ability;

class PlayerType : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_PROPERTY(bool welsh READ isWelsh WRITE setWelsh)
	Q_PROPERTY(bool human READ isHuman WRITE setHuman)
	Q_PROPERTY(QList<Ability*> abilities READ abilities)
	
public:
	PlayerType(QObject* parent = NULL);
	
	void setWelsh(bool welsh) { m_welsh = welsh; }
	bool isWelsh() const { return m_welsh; }
	
	void setHuman(bool human) { m_human = human; }
	bool isHuman() const { return m_human; }
	
	QList<Ability*> abilities() const { return m_abilities; }
	void addAbility(Ability* ability) { m_abilities << ability; }

private:
	QString m_name;
	bool m_welsh;
	bool m_human;
	QList<Ability*> m_abilities;
};
Q_DECLARE_METATYPE(PlayerType*)

#endif

