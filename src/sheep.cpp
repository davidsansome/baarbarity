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

#include "sheep.h"
#include "worldobject.h"
#include "modelloader.h"
#include "gamestate.h"
#include "terrain.h"
#include "settings.h"
#include "scripting/scriptmanager.h"

#ifdef Q_OS_DARWIN
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <math.h>
#endif

#include <QDebug>
#include <QPainter>

ObjectType* Sheep::s_objectType = NULL;


Sheep::Sheep(GameState* gameState)
	: WorldObject(gameState),
	  m_upgrade(None)
{
	if (s_objectType == NULL)
		s_objectType = ScriptManager::objectType("Sheep");
	
	m_model = ModelLoader::loadModel(":/data/models/sheep.ms3d", 0.1f);
	loadPortrait(":/data/portraits/sheep.jpg");
	
	m_name = randomName();
}

QString Sheep::randomName() const
{
	if (Settings::instance()->facebookFriends().count() > 0)
	{
		int i = qrand() % Settings::instance()->facebookFriends().count();
		return Settings::instance()->facebookFriends()[i];
	}
	
	switch (qrand() % 31)
	{
		case 0: return "Dolly";
		case 1: return "Molly";
		case 2: return "Fluffy";
		case 3: return "Fluff";
		case 4: return "Baaaartholomew";
		case 5: return "Bob";
		case 6: return "Fred";
		case 7: return "Rachel";
		case 8: return "Merona";
		case 9: return "Polly";
		case 10: return "James";
		case 11: return "John";
		case 12: return "William";
		case 13: return "Dave";
		case 14: return "Paul";
		case 15: return "Kevin";
		case 16: return "George";
		case 17: return "Brian";
		case 18: return "Jason";
		case 19: return "Geoff";
		case 20: return "Mary";
		case 21: return "Becky";
		case 22: return "Betty";
		case 23: return "Laura";
		case 24: return "Carol";
		case 25: return "Elizabeth";
		case 26: return "Susan";
		case 27: return "Sharon";
		case 28: return "Nancy";
		case 29: return "Lisa";
		case 30: return "Sean";
	}
	return QString::null;
}

int Sheep::changeLoyalty(Player* player, int amount)
{
	if (!player)
		return 0;
	
	int oldAmount = loyalty(player);
	Player* oldPlayer = m_player;

	int newAmount = qBound(0, oldAmount + amount, 100);
	m_loyalty[player] = newAmount;

	if (oldPlayer == NULL && newAmount > 66)
		setPlayer(player);
	else if (oldPlayer != NULL && newAmount < 33)
		setPlayer(NULL);

	return newAmount - oldAmount;
}

int Sheep::loyalty(Player* player)
{
	if (!m_loyalty.contains(player))
		m_loyalty[player] = 50;

	return m_loyalty[player];
}

void Sheep::drawHudInfo(QPainter& painter, const QRect& rect)
{
	if (player() == NULL || player() == gameState()->humanPlayer())
	{
		QFont smallFont(painter.font());
		smallFont.setPixelSize(10);
		QFontMetrics smallMetrics(smallFont);
		
		QRect loyaltyRect(rect.x(), rect.bottom() - 15, rect.width(), 15);
		QRect skillsRect(rect.x(), loyaltyRect.top() - smallMetrics.height(), rect.width(), smallMetrics.height());
		int l = loyalty(gameState()->humanPlayer());
		int barWidth = int(float(l) / 100.0 * float(loyaltyRect.width() - 3));
		QRect loyaltyBarRect(loyaltyRect.x() + 2, loyaltyRect.y() + 1, barWidth, loyaltyRect.height() - 3);
		QLine line33(loyaltyBarRect.x() + int(0.33f * float(loyaltyRect.width() - 3)), loyaltyRect.y(),
		             loyaltyBarRect.x() + int(0.33f * float(loyaltyRect.width() - 3)), loyaltyRect.bottom());
		QLine line66(loyaltyBarRect.x() + int(0.66f * float(loyaltyRect.width() - 3)), loyaltyRect.y(),
		             loyaltyBarRect.x() + int(0.66f * float(loyaltyRect.width() - 3)), loyaltyRect.bottom());

		// Draw skills text
		QString skillsText;
		// TODO
		
		painter.setOpacity(1.0f);
		painter.setPen(Qt::white);
		painter.setFont(smallFont);
		painter.drawText(skillsRect, Qt::AlignRight | Qt::AlignVCenter, skillsText);

		// Draw frame
		painter.setOpacity(0.2f);
		painter.setBrush(Qt::black);
		painter.drawRect(loyaltyRect);

		// Draw bar
		painter.setOpacity(0.7f);
		painter.setPen(Qt::NoPen);
		painter.setBrush(gameState()->humanPlayer()->color());
		painter.drawRect(loyaltyBarRect);

		// Draw little marks
		painter.setOpacity(1.0f);
		if (player() == NULL)
		{
			painter.setPen(gameState()->humanPlayer()->color());
			painter.drawLine(line66);
		}
		else
		{
			painter.setPen(Qt::white);
			painter.drawLine(line33);
		}

		// Draw text
		painter.setPen(Qt::white);
		painter.setFont(smallFont);
		painter.drawText(loyaltyRect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(loyalty(gameState()->humanPlayer())) + " loyalty");
	}
}

void Sheep::draw() const
{
	QColor color = Qt::white;
	if (m_player == m_gameState->humanPlayer())
		color = m_player->color();
	
	glColor3f(float(color.red()) / 255.0f,
	          float(color.green()) / 255.0f,
	          float(color.blue()) / 255.0f);
	WorldObject::draw();
}

QDataStream& operator >>(QDataStream& stream, Sheep& sheep)
{
	sheep.load(stream);
	return stream;
}

void Sheep::save(QDataStream& stream) const
{
	/*WorldObject::save(stream);
	
	stream << m_name;

	stream << m_loyalty.count();
	foreach(Player* player, m_loyalty.keys())
		stream << player->uid() << m_loyalty[player];*/
}

void Sheep::load(QDataStream& stream)
{
	/*WorldObject::load(stream);
	
	stream >> m_name;

	int count;
	stream >> count;

	for (int i=0 ; i<count ; ++i)
	{
		int uid, loyalty;
		stream >> uid >> loyalty;

		m_loyalty[m_gameState->playerByUid(uid)] = loyalty;
	}*/
}


