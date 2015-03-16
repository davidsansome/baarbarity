/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include "smoothvar.h"
#include "vector.h"

#include <QList>
#include <QObject>
#include <QPoint>
#include <QPixmap>

class WorldObject;
class Model;
class Terrain;
class GameState;
class Player;
class Ability;
class ObjectType;
class WorldObjectPrototype;
class Action;



class WorldObject : public QObject
{
	Q_OBJECT
	Q_PROPERTY(vec2 pos READ pos WRITE setPos)
	Q_PROPERTY(ObjectType* type READ type)
	Q_PROPERTY(Action* action READ currentAction WRITE setAction)
	Q_PROPERTY(Player* player READ player WRITE setPlayer)
	Q_PROPERTY(WorldObject* squadLeader READ squadLeader WRITE setSquadLeader)
	Q_PROPERTY(QList<WorldObject*> squadMembers READ squadMembers)
	Q_PROPERTY(float facing READ facing WRITE setFacing)
	Q_PROPERTY(QString name READ name)
	
public:
	virtual ~WorldObject();
	
	virtual QString name() const = 0;
	virtual float sightRadius() const = 0;
	virtual ObjectType* type() const = 0;
	
	virtual QPixmap portrait() const { return m_portrait; }
	virtual QList<Ability*> abilities() const;

	virtual void draw() const;
	virtual void drawBoundingBox() const;
	virtual float boundingSphereRadius() const;

	virtual Action* currentAction() { return m_action; }
	virtual void update(int timeDelta);

	virtual void drawHudInfo(QPainter& painter, const QRect& rect) {}

	virtual void save(QDataStream& stream) const;
	virtual void load(QDataStream& stream);
	
	virtual WorldObject* selectionProxy() const { return m_squadLeader; }

	vec2 pos() const { return m_pos; }
	void setPos(const vec2& pos) { m_pos = pos; }
	
	Player* player() const { return m_player; }
	void setPlayer(Player* player);
	
	QColor playerColor() const;
	QString playerName() const;

	GameState* gameState() { return m_gameState; }

	float facing() const { return m_facing; }
	void setFacing(float angle);
	
	void move(const vec2& delta);
	
	void startContextAction(const vec2& target);
	void startContextAction(WorldObject* target);

	bool setAction(Action* newAction);
	void forceClearAction();
	
	WorldObject* squadLeader() const { return m_squadLeader; }
	void setSquadLeader(WorldObject* leader);
	QList<WorldObject*> squadMembers() const { return m_squadMembers; }
	
	Q_INVOKABLE bool isSquadLeader() const;
	Q_INVOKABLE bool isMemberOfSquad() const;
	
	Q_INVOKABLE void finishedInitialisation();
	
	Q_INVOKABLE WorldObject* objectNear(float radius, ObjectType* type = NULL) const;
	Q_INVOKABLE QObjectList objectsNear(float radius, ObjectType* type = NULL, bool sorted = false) const;

protected:
	WorldObject(GameState* gameState);
	
	void rotateModel() const;
	void loadPortrait(const QString& portraitName);

signals:
	void actionChanged(Action* newAction);
	void actionNameChanged(QString newName);
	void playerChanged(Player* newPlayer);

protected:
	vec2 m_pos;
	Player* m_player;
	SmoothVar<float> m_facing;
	Action* m_action;
	Model* m_model;
	GameState* m_gameState;
	QPixmap m_portrait;
	QString m_portraitName;
	
	WorldObject* m_squadLeader;
	QList<WorldObject*> m_squadMembers;

private:
	static QMap<QString, QPixmap> m_portraitCache;
};
Q_DECLARE_METATYPE(WorldObject*);

QDataStream& operator <<(QDataStream& stream, const WorldObject& object);

#endif
