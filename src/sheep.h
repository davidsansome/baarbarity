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

#ifndef SHEEP_H
#define SHEEP_H

#include "worldobject.h"
#include "scripting/ability.h"
#include "scripting/objecttype.h"
#include "vector.h"

#include <QMap>

class Player;

class Sheep : public WorldObject
{
	Q_OBJECT
public:
	enum Upgrade
	{
		None,
		Lasers1,
		Lasers2,
		Lasers3,
		Grenades,
		Rockets,
		Explosives
	};
	
	Sheep(GameState* gameState);

	Upgrade upgrade() const { return m_upgrade; }
	void setUpgrade(Upgrade upgrade) { m_upgrade = upgrade; }

	void draw() const;

	QString name() const { return m_name; }
	float sightRadius() const { return 7.0f; }
	ObjectType* type() const { return s_objectType; }
	
	void drawHudInfo(QPainter& painter, const QRect& rect);

	Q_INVOKABLE int changeLoyalty(Player* player, int amount);
	Q_INVOKABLE int loyalty(Player* player);

	void save(QDataStream& stream) const;
	void load(QDataStream& stream);

private:
	QString randomName() const;
	
private:
	QString m_name;
	Upgrade m_upgrade;
	QMap<Player*, int> m_loyalty;

	static ObjectType* s_objectType;
};
Q_DECLARE_METATYPE(Sheep*);

QDataStream& operator >>(QDataStream& stream, Sheep& sheep);


#endif
