#ifndef BUILDINGTYPE_H
#define BUILDINGTYPE_H

#include <QObject>
#include <QMetaType>

class Ability;

class BuildingType : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(QList<Ability*> abilities READ abilities)

public:
	BuildingType(QObject* parent = NULL);
	
	QString name() const { return m_name; }
	void setName(const QString& name) { m_name = name; }
	
	QString description() const { return m_description; }
	void setDescription(const QString& description) { m_description = description; }
	
	QList<Ability*> abilities() const { return m_abilities; }
	void addAbility(Ability* ability) { m_abilities << ability; }

private:
	QString m_name;
	QString m_description;
	QList<Ability*> m_abilities;
};
Q_DECLARE_METATYPE(BuildingType*)

#endif

