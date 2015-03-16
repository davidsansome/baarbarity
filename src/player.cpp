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

#include "player.h"
#include "sheep.h"
#include "gamestate.h"
#include "settings.h"
#include "scripting/scriptmanager.h"
#include "scripting/playertype.h"

#include <QDebug>

PlayerType* Player::s_humanSheepType = NULL;
PlayerType* Player::s_cpuWelshType = NULL;


Player::Player(GameState* gameState)
	: QObject(gameState),
	  m_gameState(gameState),
	  m_type(NULL),
	  m_devotion(25.0),
	  m_sheepListDirty(true)
{
	if (s_humanSheepType == NULL)
	{
		s_humanSheepType = ScriptManager::playerType("HumanSheep");
		s_cpuWelshType = ScriptManager::playerType("CPUWelsh");
	}
	
	gameState->addPlayer(this);
}


int Player::sheepCount()
{
	if (!m_sheepListDirty)
		return m_sheep.count();

	m_sheep.clear();
	foreach (WorldObject* object, m_gameState->worldObjects())
	{
		Sheep* sheep = qobject_cast<Sheep*>(object);
		if (sheep && object->player() == this)
			m_sheep.append((Sheep*)object);
	}

	m_sheepListDirty = false;
	return m_sheep.count();
}

void Player::setSheepListDirty()
{
	m_sheepListDirty = true;
}

void Player::updateDevotion(int timeDelta)
{
	if (m_sheepListDirty)
		sheepCount();

	int totalLoyalty = 0;
	foreach (Sheep* sheep, m_sheep)
		totalLoyalty += sheep->loyalty(this);

	float modifier = Settings::instance()->cheats() ? 50000.0f : 500000.0f;
	m_devotion += float(totalLoyalty*timeDelta) / modifier;
}

QList<Ability*> Player::abilities() const
{
	return m_type->abilities();
}

QDataStream& operator <<(QDataStream& stream, const Player& player)
{
	player.save(stream);
	return stream;
}

QDataStream& operator >>(QDataStream& stream, Player& player)
{
	player.load(stream);
	return stream;
}

void Player::save(QDataStream& stream) const
{
	/*stream << m_uid << m_name << m_color;
	stream << m_homeLocation << m_devotion;*/
}

void Player::load(QDataStream& stream)
{
	/*int uid;
	QColor color;
	QString name;

	stream >> uid >> name >> color;
	stream >> m_homeLocation >> m_devotion;
*/
	//init(, color, name, uid);
}


