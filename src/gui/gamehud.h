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

#ifndef GAMEHUD_H
#define GAMEHUD_H

#include "ui_gamehud.h"
#include "gluimanager.h"

#include <QList>
#include <QPair>
#include <QTextLayout>

class GameEngine;
class WorldObject;
class Action;
class QPushButton;

class GameHud : public GLUIWidget
{
	Q_OBJECT
public:
	GameHud(GameEngine* parent);
	~GameHud();
	
	void setSelectedObject(WorldObject* object);
	void drawWidget(QPainter& painter, QWidget* widget);
	bool hoverEvent(QWidget* widget, const QPoint& pos);
	bool widgetAcceptsClicks(QWidget* widget);

public slots:
	void appendChatLine(const QString& line);
	void recheckAbilityRequirements();

private slots:
	void abilityClicked();
	void chatReturnPressed();

private:
	QList<Ability*> abilityList();
	void drawObjectInfo(QPainter& painter);
	void drawPlayerInfo(QPainter& painter);
	void drawChatBox(QPainter& painter);
	
	Ui_GameHud m_ui;
	GameEngine* m_gameEngine;
	WorldObject* m_selectedObject;

	QList<QPushButton*> m_abilityButtons;
	QList<QPair<QTextLayout*, bool> > m_chat;
};

#endif
