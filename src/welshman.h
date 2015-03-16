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

#ifndef WELSHMAN_H
#define WELSHMAN_H

#include "worldobject.h"
#include "scripting/objecttype.h"

/*class WelshIdleAction : public Action
{
public:
	WelshIdleAction(WorldObject* worldObject);
	
	QString name() const { return "Thinking about sheep"; }
	Type rtti() const { return Type_WelshIdle; }
	bool run(int timeDelta) { return true; }

	void save(QDataStream& stream) const {}
};

QDataStream& operator >>(QDataStream& stream, WelshIdleAction& a);*/

class Welshman : public WorldObject
{
	Q_OBJECT
public:
	Welshman(GameState* gameState);

	QString name() const { return m_name; }
	float sightRadius() const { return 7.0f; }
	ObjectType* type() const { return s_objectType; }
	
	void save(QDataStream& stream) const;
	void load(QDataStream& stream);

private:
	QString randomName() const;
	
private:
	QString m_name;
	static ObjectType* s_objectType;
};
Q_DECLARE_METATYPE(Welshman*)

QDataStream& operator >>(QDataStream& stream, Welshman& welshman);

#endif
