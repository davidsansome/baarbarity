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

#include "menuhud.h"
#include "gameengine.h"
#include "gamestate.h"
#include "gluimanager.h"
#include "settings.h"
#include "progressdialog.h"
#include "playeroptionsdialog.h"

MenuHud::MenuHud(GameEngine* parent)
	: GLUIWidget(parent),
	  m_gameEngine(parent)
{
	m_ui.setupUi(this);

	m_playerOptionsDialog = new PlayerOptionsDialog(m_gameEngine);
	
	connect(m_ui.singlePlayerButton, SIGNAL(pressed()), SLOT(singlePlayerPressed()));
	connect(m_ui.optionsButton, SIGNAL(pressed()), m_gameEngine, SLOT(showOptionsDialog()));
	connect(m_ui.playerOptionsButton, SIGNAL(pressed()), SLOT(showPlayerOptionsDialog()));
	connect(m_ui.exitButton, SIGNAL(pressed()), QCoreApplication::instance(), SLOT(quit()));
}

MenuHud::~MenuHud()
{
}

void MenuHud::singlePlayerPressed()
{
	GameState* gameState = new GameState();
	gameState->newGame();
	
	m_gameEngine->setState(gameState, false);

	hideGLUI();
}

void MenuHud::showPlayerOptionsDialog()
{
	m_gameEngine->uiManager()->showDialog(m_playerOptionsDialog);
}



