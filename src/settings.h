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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QStringList>

class QSettings;

class Settings : public QObject
{
	Q_OBJECT

public:
	static Settings* instance();
	
	bool textureCompression();
	bool terrainShaders();
	bool waterShaders();
	bool drawModels();
	bool drawWireframe();
	bool drawOriginCube();
	int targetFps();
	int targetFrameTime();
	float farClippingDistance();
	float fogStart();
	bool isFogEnabled();
	
	float rotationSensitivityY();
	float rotationSensitivityX();
	float zoomSensitivity();
	float movementSpeed();
	float gameToGl();
	float maxHeight();
	bool cheats();

	QString playerName();

	QString facebookApiKey();
	QString facebookSecret();

	QString facebookSessionKey();
	QString facebookSessionSecret();
	QString facebookUid();
	QString facebookName();
	QStringList facebookFriends();

	void clearFacebookSession();
	void setFacebookSession(const QString& sessionKey, const QString& sessionSecret, const QString& uid);
	void setFacebookName(const QString& name);
	void setFacebookFriends(const QStringList& friends);
	

public slots:
	void setTextureCompression(bool c);
	void setTerrainShaders(bool t);
	void setWaterShaders(bool t);
	void setDrawModels(bool t);
	void setDrawWireframe(bool t);
	void setDrawOriginCube(bool t);
	void setTargetFps(int fps);
	void setFarClippingDistance(int distance);
	void setFogEnabled(bool fog);
	void setPlayerName(const QString& name);

signals:
	void terrainShadersChanged(bool shaders);
	void waterShadersChanged(bool shaders);
	void farClippingDistanceChanged(float fogStart, float farClippingDistance);

private:
	Settings();
	static Settings* m_instance;
	
	bool m_textureCompression;
	bool m_terrainShaders;
	bool m_waterShaders;
	bool m_drawModels;
	bool m_drawWireframe;
	bool m_drawOriginCube;
	int m_targetFps;
	int m_targetFrameTime;
	QString m_playerName;
	float m_farClippingDistance;
	float m_fogStart;
	bool m_fog;

	QString m_facebookSessionKey;
	QString m_facebookSessionSecret;
	QString m_facebookUid;
	QString m_facebookName;
	QStringList m_facebookFriends;

	QSettings* m_backend;
};

#endif

