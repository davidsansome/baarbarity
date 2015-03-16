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

#include "gamemenudialog.h"
#include "gameengine.h"
#include "gamestate.h"

#include <QFileDialog>
#include <QDir>
#include <QFile>

GameMenuDialog::GameMenuDialog(GameEngine* gameEngine)
	: GLUIWidget(gameEngine),
	  m_gameEngine(gameEngine)
{
	m_ui.setupUi(this);

	connect(m_ui.optionsButton, SIGNAL(clicked()), SLOT(showOptionsDialog()));
	connect(m_ui.shaderEditorButton, SIGNAL(clicked()), gameEngine, SLOT(showShaderEditor()));
	//connect(m_ui.gameEditorButton, SIGNAL(clicked()), gameEngine, SLOT(showGameEditor()));
	connect(m_ui.saveButton, SIGNAL(clicked()), SLOT(save()));
	connect(m_ui.loadButton, SIGNAL(clicked()), SLOT(load()));
	connect(m_ui.returnButton, SIGNAL(clicked()), SLOT(hideGLUI()));
	connect(m_ui.exitButton, SIGNAL(clicked()), QCoreApplication::instance(), SLOT(quit()));
}

GameMenuDialog::~GameMenuDialog()
{
}

void GameMenuDialog::showOptionsDialog()
{
	hideGLUI();
	m_gameEngine->showOptionsDialog();
}

void GameMenuDialog::save()
{
	QString fileName = QFileDialog::getSaveFileName(NULL, "Save game", QDir::homePath(), "Baarbarity state (*.baa)");

	if (fileName.isEmpty())
		return;
	if (!fileName.endsWith(".baa"))
		fileName += ".baa";

	m_gameEngine->gameState()->save(fileName);
}

void GameMenuDialog::load()
{
	QString fileName = QFileDialog::getOpenFileName(NULL, "Load game", QDir::homePath(), "Baarbarity state (*.baa)");

	if (fileName.isEmpty())
		return;

	GameState* state = new GameState();
	state->load(fileName);
	m_gameEngine->setState(state, false);
}


