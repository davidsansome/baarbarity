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

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "worldobject.h"
#include "player.h"
#include "vector.h"
#include "scripting/ability.h"

#include <QList>

class Terrain;
class Sheep;
class Billboard;

class GameState : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QList<WorldObject*> worldObjects READ worldObjects)
	Q_PROPERTY(Terrain* terrain READ terrain)
	Q_PROPERTY(Player* humanPlayer READ humanPlayer WRITE setHumanPlayer)
	
	friend class WorldObject;
	friend class Player;
	friend class Billboard;
	
public:
	GameState();
	~GameState();

	void newGame(int width = 50, int height = 50, int iterations = 600);

	QList<WorldObject*> worldObjects() const { return m_worldObjects; }
	Terrain* terrain() const { return m_terrain; }
	QList<Player*> players() const { return m_players; }
	QList<Billboard*> billboards() const { return m_billboards; }
	
	Player* humanPlayer() const { return m_humanPlayer; }
	void setHumanPlayer(Player* player) { m_humanPlayer = player; }

	void increaseLoyalty(Sheep* sheep, Player* player, int amount);

	void updateValues(int timeDelta);

	void save(QDataStream& stream) const;
	void load(QDataStream& stream);

	void save(const QString& fileName) const;
	void load(const QString& fileName);
	
	Q_INVOKABLE vec2 randomPositionNear(const vec2& pos, float maxRadius);
	
	Q_INVOKABLE WorldObject* objectNear(const vec2& pos, float radius, ObjectType* type = NULL) const;
	Q_INVOKABLE QObjectList objectsNear(const vec2& pos, float radius, ObjectType* type = NULL, bool sorted = false) const;;

public slots:
	void setSheepCountDirty();

signals:
	void loyaltyChanged(Sheep* sheep, int amount);
	void objectRemoved(WorldObject* object);

private slots:
	void billboardDestroyed(QObject* o);

private:
	void addWorldObject(WorldObject* object) { m_worldObjects << object; }
	void removeWorldObject(WorldObject* object);
	
	void addPlayer(Player* player) { m_players << player; }
	
	void addBillboard(Billboard* billboard);
	
	QList<WorldObject*> m_worldObjects;
	Terrain* m_terrain;
	QList<Player*> m_players;
	Player* m_humanPlayer;
	QList<Billboard*> m_billboards;
};
Q_DECLARE_METATYPE(GameState*)

QDataStream& operator <<(QDataStream& stream, const GameState& state);
QDataStream& operator >>(QDataStream& stream, GameState& state);

#endif
