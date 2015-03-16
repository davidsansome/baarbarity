#ifndef CONSTRUCTIONACTION_H
#define CONSTRUCTIONACTION_H

#include <QString>

#include "action.h"

class Billboard;
class WorldObject;

class ConstructionAction : public Action
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(float duration READ duration WRITE setDuration)
	Q_PROPERTY(Billboard* billboard READ billboard WRITE setBillboard)

public:
	ConstructionAction(WorldObject* parent);
	
	QString name() const { return m_name; }
	void setName(const QString& name) { m_name = name; }
	
	float duration() const { return m_duration; }
	void setDuration(float d) { m_duration = d; }
	
	Billboard* billboard() const { return m_billboard; }
	void setBillboard(Billboard* b) { m_billboard = b; }
	
	bool hasProgressBar() const { return true; }
	float progress() const;

	bool run(int timeDelta);

	bool interrupt(Action* interruptingAction);

protected:
	QString m_name;
	float m_duration;
	Billboard* m_billboard;
	
	float m_elapsed;
};
Q_DECLARE_METATYPE(ConstructionAction*);

#endif
