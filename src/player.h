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

#ifndef PLAYER_H
#define PLAYER_H

#include "vector.h"
#include "scripting/ability.h"

#include <QColor>
#include <QString>
#include <QObject>

class GameState;
class Sheep;
class PlayerType;

class Player : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(int resource READ devotion)
	Q_PROPERTY(PlayerType* type READ type WRITE setType)
	Q_PROPERTY(vec2 homeLocation READ homeLocation WRITE setHomeLocation)
	Q_PROPERTY(QColor color READ color WRITE setColor)
	
public:
	Player(GameState* gameState);

	QColor color() const { return m_color; }
	void setColor(const QColor& color) { m_color = color; }
	
	QString name() const { return m_name; }
	void setName(const QString& name) { m_name = name; }
	
	PlayerType* type() const { return m_type; }
	void setType(PlayerType* type) { m_type = type; }

	vec2 homeLocation() { return m_homeLocation; }
	void setHomeLocation(const vec2& pos) { m_homeLocation = pos; }

	QList<Ability*> abilities() const;

	int devotion() const { return int(m_devotion); }
	void spendDevotion(int amount) { m_devotion -= float(amount); }
	void updateDevotion(int timeDelta);
	
	Q_INVOKABLE void spendResource(int amount) { spendDevotion(amount); }

	int sheepCount();
	void setSheepListDirty();

	void save(QDataStream& stream) const;
	void load(QDataStream& stream);
	
	static PlayerType* humanSheepType() { return s_humanSheepType; }
	static PlayerType* cpuWelshType() { return s_cpuWelshType; }

private:
	GameState* m_gameState;

	QColor m_color;
	QString m_name;
	PlayerType* m_type;

	vec2 m_homeLocation;

	static QList<Ability*> m_abilities;
	int m_devotion;

	bool m_sheepListDirty;
	QList<Sheep*> m_sheep;
	
	static PlayerType* s_humanSheepType;
	static PlayerType* s_cpuWelshType;
};
Q_DECLARE_METATYPE(Player*)

QDataStream& operator <<(QDataStream& stream, const Player& player);
QDataStream& operator >>(QDataStream& stream, Player& player);

#endif
