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

#include "settings.h"

#include <QSettings>

Settings* Settings::m_instance = NULL;

Settings::Settings()
{
	m_backend = new QSettings("Baarbarity", "Baarbarity", this);
	
	m_textureCompression = false;
	m_terrainShaders = m_backend->value("TerrainShaders", true).toBool();
	m_waterShaders = m_backend->value("WaterShaders", true).toBool();
	m_drawModels = m_backend->value("DrawModels", true).toBool();
	m_drawWireframe = m_backend->value("DrawWireframe", false).toBool();
	m_drawOriginCube = m_backend->value("DrawOriginCube", false).toBool();
	m_targetFps = m_backend->value("TargetFps", 60).toInt();
	m_targetFrameTime = (1000/m_targetFps);
	m_farClippingDistance = m_backend->value("FarClippingDistance", 150.0).toDouble();
	m_fogStart = m_farClippingDistance - 20.0f;
	m_fog = m_backend->value("Fog", true).toBool();
	m_playerName = m_backend->value("PlayerName", "Player").toString();

	m_facebookSessionKey = m_backend->value("Facebook/SessionKey").toString();
	m_facebookSessionSecret = m_backend->value("Facebook/SessionSecret").toString();
	m_facebookUid = m_backend->value("Facebook/Uid").toString();
	m_facebookName = m_backend->value("Facebook/Name").toString();
	m_facebookFriends = m_backend->value("Facebook/Friends").toStringList();
}

Settings* Settings::instance()
{
	if (!m_instance)
		m_instance = new Settings();
	return m_instance;
}

bool Settings::textureCompression()
{
	return m_textureCompression;
}

void Settings::setTextureCompression(bool c)
{
	m_textureCompression = c;
}

bool Settings::terrainShaders() { return m_terrainShaders; }
void Settings::setTerrainShaders(bool t)
{
	bool old = m_terrainShaders;
	m_terrainShaders = t;
	m_backend->setValue("TerrainShaders", t);

	if (m_terrainShaders != old)
		emit(terrainShadersChanged(t));
}

bool Settings::waterShaders() { return m_waterShaders; }
void Settings::setWaterShaders(bool t)
{
	bool old = m_waterShaders;
	m_waterShaders = t;
	m_backend->setValue("WaterShaders", t);

	if (m_waterShaders != old)
		emit(waterShadersChanged(t));
}

bool Settings::drawModels() { return m_drawModels; }
void Settings::setDrawModels(bool t)
{
	m_drawModels = t;
	m_backend->setValue("DrawModels", t);
}

bool Settings::drawWireframe() { return m_drawWireframe; }
void Settings::setDrawWireframe(bool t)
{
	m_drawWireframe = t;
	m_backend->setValue("DrawWireframe", t);
}

bool Settings::drawOriginCube() { return m_drawOriginCube; }
void Settings::setDrawOriginCube(bool t)
{
	m_drawOriginCube = t;
	m_backend->setValue("DrawOriginCube", t);
}

int Settings::targetFps() { return m_targetFps; }
int Settings::targetFrameTime() { return m_targetFrameTime; }
void Settings::setTargetFps(int fps)
{
	m_targetFps = fps;
	m_targetFrameTime = (1000/fps);
	m_backend->setValue("TargetFps", fps);
}

void Settings::setFarClippingDistance(int distance)
{
	float old = m_farClippingDistance;
	m_farClippingDistance = float(distance);
	m_fogStart = m_farClippingDistance - 20.0f;
	m_backend->setValue("FarClippingDistance", m_farClippingDistance);

	if (m_farClippingDistance != old)
		emit(farClippingDistanceChanged(m_fogStart, m_farClippingDistance));
}
float Settings::farClippingDistance() { return m_farClippingDistance; }
float Settings::fogStart() { return m_fogStart; }

void Settings::setFogEnabled(bool fog)
{
	m_fog = fog;
	m_backend->setValue("Fog", fog);
}
bool Settings::isFogEnabled() { return m_fog; }


float Settings::rotationSensitivityY()
{
	return 0.005f;
}

float Settings::rotationSensitivityX()
{
	return 0.005f;
}

float Settings::zoomSensitivity()
{
	return 0.05f;
}

float Settings::movementSpeed()
{
	return 0.001f;
}

float Settings::gameToGl()
{
	// Don't change me - shaders depend on this being 1
	return 1.0f;
}

float Settings::maxHeight()
{
	return 15.0f;
}

bool Settings::cheats()
{
	return true;
}

QString Settings::playerName() { return m_playerName; }
void Settings::setPlayerName(const QString& name)
{
	m_playerName = name;
	m_backend->setValue("PlayerName", name);
}

QString Settings::facebookApiKey() { return "652092e43ac3c1253d16a33366408351"; }
QString Settings::facebookSecret() { return "31318311dc9f46bee6b44115bb33baa9"; }

QString Settings::facebookSessionKey() { return m_facebookSessionKey; }
QString Settings::facebookSessionSecret() { return m_facebookSessionSecret; }
QString Settings::facebookUid() { return m_facebookUid; }
QString Settings::facebookName() { return m_facebookName; }
QStringList Settings::facebookFriends() { return m_facebookFriends; }

void Settings::clearFacebookSession()
{
	setFacebookSession(QString::null, QString::null, QString::null);
	setFacebookName(QString::null);
	setFacebookFriends(QStringList());
}

void Settings::setFacebookSession(const QString& sessionKey, const QString& sessionSecret, const QString& uid)
{
	m_facebookSessionKey = sessionKey;
	m_facebookSessionSecret = sessionSecret;
	m_facebookUid = uid;

	m_backend->setValue("Facebook/SessionKey", sessionKey);
	m_backend->setValue("Facebook/SessionSecret", sessionSecret);
	m_backend->setValue("Facebook/Uid", uid);
}

void Settings::setFacebookName(const QString& name)
{
	m_facebookName = name;

	m_backend->setValue("Facebook/Name", name);
}

void Settings::setFacebookFriends(const QStringList& friends)
{
	m_facebookFriends = friends;

	m_backend->setValue("Facebook/Friends", friends);
}
