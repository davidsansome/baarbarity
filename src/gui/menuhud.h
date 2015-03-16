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

#ifndef MENUHUD_H
#define MENUHUD_H

#include "ui_menuhud.h"
#include "gluimanager.h"

#include <QWidget>

class GameEngine;
class PlayerOptionsDialog;

class MenuHud : public GLUIWidget
{
	Q_OBJECT
public:
	MenuHud(GameEngine* parent);
	~MenuHud();

private slots:
	void singlePlayerPressed();
	void showPlayerOptionsDialog();

private:
	Ui_MenuHud m_ui;
	GameEngine* m_gameEngine;
	PlayerOptionsDialog* m_playerOptionsDialog;
};

#endif
