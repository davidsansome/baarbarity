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

#include "worldobject.h"
#include "model.h"
#include "gamestate.h"
#include "terrain.h"
#include "settings.h"
#include "player.h"
#include "welshman.h"
#include "sheep.h"
#include "actions/action.h"
#include "opengl.h"

#include <QDebug>
#include <cmath>

QMap<QString, QPixmap> WorldObject::m_portraitCache;


WorldObject::~WorldObject()
{
	delete m_model;
}

WorldObject::WorldObject(GameState* gameState)
	: QObject(gameState),
	  m_player(NULL),
	  m_action(NULL),
	  m_model(NULL),
	  m_gameState(gameState),
	  m_squadLeader(this)
{
	m_facing = float(qrand() % 360);
	m_facing.setModulo(360.0f);
	m_facing.updateImmediately();
	
	gameState->addWorldObject(this);

	connect(this, SIGNAL(playerChanged(Player*)), m_gameState, SLOT(setSheepCountDirty()));
}

void WorldObject::loadPortrait(const QString& portraitName)
{
	m_portraitName = portraitName;
	if (m_portraitCache.contains(portraitName))
		m_portrait = m_portraitCache[portraitName];
	else
	{
		m_portrait = QPixmap(portraitName, "JPEG");
		m_portraitCache[portraitName] = m_portrait;
	}
}

void WorldObject::move(const vec2& delta)
{
	m_pos += delta;
	
	float r = boundingSphereRadius();
	m_pos.x = qBound(r, m_pos.x, m_gameState->terrain()->width() - r);
	m_pos.y = qBound(r, m_pos.y, m_gameState->terrain()->height() - r);
}

void WorldObject::setFacing(float facing)
{
	m_facing = facing;
	m_facing.finishedEditing();
}

void WorldObject::rotateModel() const
{
	vec3 normal = m_gameState->terrain()->normalAt(m_pos);

	glRotatef(normal.z * 90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(- normal.x * 90.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(- m_facing.currentValue(), 0.0f, 1.0f, 0.0f);
}

void WorldObject::draw() const
{
	rotateModel();
	m_model->draw();
}

void WorldObject::drawBoundingBox() const
{
	rotateModel();
	m_model->drawBoundingBox();
}

float WorldObject::boundingSphereRadius() const
{
	return m_model->boundingSphereRadius();
}

void WorldObject::update(int timeDelta)
{
	Action* action = m_action;
	if (action)
	{
		QString oldName = action->name();
		if (!action->run(timeDelta))
		{
			// If the action set a new action, don't try to start the default action again
			if (m_action != action)
				return;
			m_action = NULL;
			type()->actionCompleted(this, action);
			delete action;
		}
		else if (oldName != m_action->name())
			emit actionNameChanged(m_action->name());
	}
	
	m_facing.update(timeDelta);
}

QList<Ability*> WorldObject::abilities() const
{
	return type()->abilities();
}

bool WorldObject::setAction(Action* newAction)
{
	if (m_action && !m_action->interrupt(newAction))
	{
		newAction->deleteLater();
		return false;
	}

	delete m_action;
	m_action = newAction;
	m_action->attached();
	emit actionChanged(newAction);
	emit actionNameChanged(newAction ? newAction->name() : QString::null);

	return true;
}

void WorldObject::forceClearAction()
{
	delete m_action;
	m_action = NULL;
}

QColor WorldObject::playerColor() const
{
	if (m_player)
		return m_player->color();
	return Qt::white;
}

QString WorldObject::playerName() const
{
	if (m_player)
		return m_player->name();
	return "Neutral";
}

void WorldObject::setPlayer(Player* player)
{
	bool changed = player != m_player;
	
	m_player = player;
	
	if (changed)
		emit(playerChanged(player));
}

void WorldObject::startContextAction(const vec2& target)
{
	type()->startContextAction(this, target);
}

void WorldObject::startContextAction(WorldObject* target)
{
	type()->startContextAction(this, target);
}

void WorldObject::setSquadLeader(WorldObject* leader)
{
	// Remove ourself from any existing squad
	if (m_squadLeader && m_squadLeader != this)
		m_squadLeader->m_squadMembers.removeAll(this);
	
	if (leader == NULL || leader == this)
		m_squadLeader = this;
	else
	{
		m_squadLeader = leader;
		if (!m_squadLeader->m_squadMembers.contains(this))
			m_squadLeader->m_squadMembers << this;
	}
}

bool WorldObject::isSquadLeader() const
{
	return m_squadLeader == this &&
	       m_squadMembers.count() != 0;
}

bool WorldObject::isMemberOfSquad() const
{
	return m_squadLeader != this;
}

void WorldObject::finishedInitialisation()
{
	m_facing.updateImmediately();
	type()->actionCompleted(this, NULL);
}

QDataStream& operator <<(QDataStream& stream, const WorldObject& object)
{
	object.save(stream);
	return stream;
}

void WorldObject::save(QDataStream& stream) const
{
	/*int player = m_player ? m_player->uid() : -1;
	stream << m_uid;
	stream << m_pos << m_facing.targetValue() << player;
	stream << m_portraitName;
	stream << m_action->rtti() << *m_action;*/
}

void WorldObject::load(QDataStream& stream)
{
	/*vec2 pos;
	int playerUid, uid;
	QString portraitName;
	float facing;
	
	stream >> uid >> pos >> facing >> playerUid >> portraitName;
	initWorldObject(pos, m_gameState->playerByUid(playerUid), portraitName, facing, uid);

	int actionType;
	Action* action;
	stream >> actionType;
	switch(actionType)
	{
	case Action::Type_Ability:
		action = new AbilityAction(this);
		stream >> *((AbilityAction*)action);
		break;
	case Action::Type_Distracting:
		action = new DistractingAction(this);
		stream >> *((DistractingAction*)action);
		break;
	case Action::Type_WelshIdle:
		action = new WelshIdleAction(this);
		stream >> *((WelshIdleAction*)action);
		break;
	case Action::Type_PlayerMove:
		action = new PlayerMoveAction(this);
		stream >> *((PlayerMoveAction*)action);
		break;
	case Action::Type_Growing:
		action = new GrowingAction(this);
		stream >> *((GrowingAction*)action);
		break;
	case Action::Type_EatGrass:
		action = new EatGrassAction(this);
		stream >> *((EatGrassAction*)action);
		break;
	}

	m_action = action;*/
}

QObjectList WorldObject::objectsNear(float radius, ObjectType* type, bool sorted) const
{
	return m_gameState->objectsNear(m_pos, radius, type, sorted);
}

WorldObject* WorldObject::objectNear(float radius, ObjectType* type) const
{
	return m_gameState->objectNear(m_pos, radius, type);
}


