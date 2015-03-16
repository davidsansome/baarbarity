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

#include "gamestate.h"
#include "terrain.h"
#include "sheep.h"
#include "welshman.h"
#include "settings.h"
#include "scripting/billboard.h"

#include <limits>
#include <math.h>
#include <QFile>

GameState::GameState()
	: m_terrain(NULL),
	  m_humanPlayer(NULL)
{
}

GameState::~GameState()
{
	delete m_terrain;
}

void GameState::newGame(int width, int height, int iterations)
{
	m_terrain = new Terrain(width, height);

	// Generate the terrain
	m_terrain->generate(iterations, 4, 15, 1.75f);
}

vec2 GameState::randomPositionNear(const vec2& pos, float maxRadius)
{
	float angle = float(qrand()) / float(RAND_MAX) * M_PI * 2;
	float radius = float(qrand()) / float(RAND_MAX) * maxRadius;

	return vec2(qBound(0.0f, pos.x + radius * cosf(angle), float(m_terrain->width())),
	              qBound(0.0f, pos.y + radius * sinf(angle), float(m_terrain->height())));
}

void GameState::setSheepCountDirty()
{
	foreach (Player* player, m_players)
	{
		player->setSheepListDirty();
	}
}

void GameState::increaseLoyalty(Sheep* sheep, Player* player, int amount)
{
	int clampedAmount = sheep->changeLoyalty(player, amount);
	
	if (clampedAmount != 0)
	{
		emit(loyaltyChanged(sheep, clampedAmount));
	}
}

void GameState::updateValues(int timeDelta)
{
	foreach (WorldObject* object, m_worldObjects)
		object->update(timeDelta);

	foreach (Player* player, m_players)
		player->updateDevotion(timeDelta);
}

void GameState::removeWorldObject(WorldObject* object)
{
	delete object;
	m_worldObjects.removeAll(object);
	emit(objectRemoved(object));
}

void GameState::addBillboard(Billboard* billboard)
{
	connect(billboard, SIGNAL(destroyed(QObject*)), SLOT(billboardDestroyed(QObject*)));
	m_billboards << billboard;
}

void GameState::billboardDestroyed(QObject* o)
{
	m_billboards.removeAll(static_cast<Billboard*>(o));
}

QDataStream& operator <<(QDataStream& stream, const GameState& state)
{
	state.save(stream);
	return stream;
}

QDataStream& operator >>(QDataStream& stream, GameState& state)
{
	state.load(stream);
	return stream;
}

void GameState::save(QDataStream& stream) const
{
	/*stream << *m_terrain;

	stream << m_players.count();
	foreach(Player* p, m_players)
		stream << *p;

	stream << m_humanPlayer->uid();

	stream << m_worldObjects.count();*/
	/*foreach(WorldObject* o, m_worldObjects)
	{
		stream << o->rtti();
		stream << *o;
	}*/
}

void GameState::load(QDataStream& stream)
{
	/*m_terrain = new Terrain();
	stream >> *m_terrain;

	int count;
	stream >> count;
	for(int i=0 ; i<count ; ++i)
	{
		Player* p = new Player(this);
		stream >> *p;
		m_players << p;
	}

	int humanPlayer;
	stream >> humanPlayer;
	m_humanPlayer = playerByUid(humanPlayer);

	stream >> count;*/
/*	for(int i=0 ; i<count ; ++i)
	{
		int rtti;
		stream >> rtti;
		WorldObject* o;
		
		switch(rtti)
		{
		case WorldObject::Type_Distraction:
			o = new Distraction(this);
			stream >> *((Distraction*)o);
			break;
		case WorldObject::Type_Sheep:
			o = new Sheep(this);
			stream >> *((Sheep*)o);
			break;
		case WorldObject::Type_Welshman:
			o = new Welshman(this);
			stream >> *((Welshman*)o);
			break;
		}

		m_worldObjects << o;
	}*/

	/*setSheepCountDirty();*/
}

void GameState::save(const QString& fileName) const
{
	QFile file(fileName);
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);
	
	save(stream);
}

void GameState::load(const QString& fileName)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	QDataStream stream(&file);

	load(stream);
}

QObjectList GameState::objectsNear(const vec2& pos, float radius, ObjectType* type, bool sorted) const
{
	typedef QPair<float, WorldObject*> DistObject;
	
	QList<DistObject> objects;
	
	foreach(WorldObject* o, m_worldObjects)
	{
		float dist = o->pos().distanceTo(pos);
		if (dist < radius && (type == NULL || type == o->type()))
			objects << DistObject(dist, o);
	}
	qSort(objects);
	
	QObjectList ret;
	foreach(DistObject o, objects)
		ret << o.second;
	
	return ret;
}

WorldObject* GameState::objectNear(const vec2& pos, float radius, ObjectType* type) const
{
	float nearestDist = std::numeric_limits<float>::max();
	WorldObject* nearestObject = NULL;
	
	foreach(WorldObject* o, m_worldObjects)
	{
		float dist = o->pos().distanceTo(pos);
		if (dist < nearestDist && (type == NULL || type == o->type()))
		{
			nearestDist = dist;
			nearestObject = o;
		}
	}
	
	return nearestObject;
}
