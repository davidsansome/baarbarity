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

#include "optionsdialog.h"
#include "gameengine.h"
#include "settings.h"
#include "gamestate.h"
#include "terrain.h"

OptionsDialog::OptionsDialog(GameEngine* gameEngine)
	: GLUIWidget(gameEngine),
	  m_gameEngine(gameEngine)
{
	m_ui.setupUi(this);

	m_ui.shadersBox->setChecked(Settings::instance()->terrainShaders());
	m_ui.waterShadersBox->setChecked(Settings::instance()->waterShaders());
	m_ui.maxFpsBox->setText(QString::number(Settings::instance()->targetFps()));
	m_ui.modelsBox->setChecked(Settings::instance()->drawModels());
	m_ui.wireframeBox->setChecked(Settings::instance()->drawWireframe());
	m_ui.originCubeBox->setChecked(Settings::instance()->drawOriginCube());
	m_ui.drawDistance->setValue(int(Settings::instance()->farClippingDistance()));
	m_ui.fogBox->setChecked(Settings::instance()->isFogEnabled());

	connect(m_ui.shadersBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setTerrainShaders(bool)));
	connect(m_ui.waterShadersBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setWaterShaders(bool)));
	connect(m_ui.modelsBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setDrawModels(bool)));
	connect(m_ui.wireframeBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setDrawWireframe(bool)));
	connect(m_ui.originCubeBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setDrawOriginCube(bool)));
	connect(m_ui.fogBox, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setFogEnabled(bool)));
	connect(m_ui.drawDistance, SIGNAL(valueChanged(int)), Settings::instance(), SLOT(setFarClippingDistance(int)));

	connect(m_ui.closeButton, SIGNAL(clicked()), SLOT(updateTargetFps()));
	connect(m_ui.closeButton, SIGNAL(clicked()), SLOT(hideGLUI()));
}


OptionsDialog::~OptionsDialog()
{
}

void OptionsDialog::updateTargetFps()
{
	bool ok = false;
	int fps = m_ui.maxFpsBox->text().toInt(&ok);

	if (ok && fps>0)
		Settings::instance()->setTargetFps(fps);
	else
		m_ui.maxFpsBox->setText(QString::number(Settings::instance()->targetFps()));
}


