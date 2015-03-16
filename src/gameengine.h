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

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "smoothvar.h"
#include "frustum.h"
#include "vector.h"

#include <QGLWidget>
#include <QPainter>
#include <QPoint>
#include <QTime>
#include <QtScript>

class WorldObject;
class GameState;
class GLUIManager;
class GameMenuDialog;
class OptionsDialog;
class ProgressDialog;
class ShaderEditor;
class MenuHud;
class GameHud;
class Ability;
class Sheep;
class Pool;
class Texture;
class Shader;
class PostProcessing;
class Billboard;

class QTimer;
class QMouseEvent;
class QPaintEvent;
class QGLFramebufferObject;

class FloatingText
{
public:
	FloatingText(const QString& t, const QColor& c, const vec3& p);

	bool update(int timeDelta);
	float opacity() const;
	
	QString text;
	QColor color;
	float height;
	vec3 pos;
};

class GameEngine : public QGLWidget
{
	Q_OBJECT
	
public:
	GameEngine(QWidget* parent);
	~GameEngine();

	void setState(GameState* state, bool menuMode);
	GameState* gameState() const { return m_gameState; }
	GLUIManager* uiManager() const { return m_uiManager; }

public slots:
	void showOptionsDialog();
	void showShaderEditor();

signals:
	void stateChanged(GameState* state);

private:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

	void resizeGL(int width, int height);
	void initializeGL();

	void updateCameraRotation();
	void updateCameraPosition(int timeDelta);
	void updateLightPosition();

	void draw3D();
	void draw3DReflections();
	void draw3DRefractions();
	
	void draw2D(QPainter& painter);
	void drawFloatingText(QPainter& painter, double* model, double* proj, int* view);
	
	void updateValues(int timeDelta);

	void setupViewport();
	void setupGL();
	void setupCamera();
	void calculateCameraPosition();

	void renderTerrain(bool useFrustum = true);
	void renderWater();
	void renderWorldObjects();
	void renderBillboards();
	void renderTestCube();
	void renderDropCircle();
	void renderSky(bool upsideDown);
	void renderBaseLayer();
	void renderBaseLayer(const vec2& pos, float width, float height);

	vec2 glToGameCoord(const vec3& gl);
	vec3 gameCoordToGl(const vec2& gameCoord, bool adjustToTerrainHeight = false);
	void translateToGameCoord(const vec2& gameCoord, bool adjustToTerrainHeight = false);
	
	QObject* objectAtPoint(const QPoint& pos, const QList<QObject*>* fineSelection = NULL);
	vec3 screenCoordToGl(const QPoint& pos);

	void drawCircle(const vec2& center, float radius, int resolution=35);
	bool isBillboardTargetValid() const;

private slots:
	void updateFps();
	void farClippingDistanceChanged();
	void sheepLoyaltyChanged(Sheep*, int amount);
	void objectRemoved(WorldObject* object);

private:
	GameState* m_gameState;
	QList<FloatingText*> m_floatingText;
	
	QTimer* m_redrawTimer;
	QTimer* m_fpsTimer;

	int m_fps;
	int m_frameCount;

	Frustum m_frustum;

	SmoothVar<float> m_cameraOffset[3];
	vec3 m_rotationOffset;
	SmoothVar<float> m_vertAngle, m_horizAngle;
	SmoothVar<float> m_zoomDistance;

	vec3 m_lookAtPos;
	vec3 m_eyePos;

	bool m_rotateDragging;
	Billboard* m_billboardDragging;
	vec2 m_billboardDraggingPos;
	QObject* m_dragStartObject;
	QPoint m_dragStartPoint;
	float m_startVertAngle, m_startHorizAngle;

	bool m_moveKeyPressed[4];

	float m_lightPosition[4];
	Texture* m_skyBox[5];
	Texture* m_waterDisplacementMaps;
	Texture* m_reflection;
	Texture* m_refraction;
	QGLFramebufferObject* m_reflectionFbo;
	QGLFramebufferObject* m_refractionFbo;

	float m_t;
	
	QObject* m_hoverObject;
	WorldObject* m_selectedObject;
	QPoint m_mouseCursorPos;
	QTime m_hoverTimer;

	MenuHud* m_menuHud;
	GameHud* m_gameHud;
	GameMenuDialog* m_gameMenuDialog;
	OptionsDialog* m_optionsDialog;
	ProgressDialog* m_progressDialog;
	ShaderEditor* m_shaderEditor;
	GLUIManager* m_uiManager;

	bool m_menuMode;
	
	static Shader* s_baseLayerShader;
	static Shader* s_worldObjectShader;
	
	PostProcessing* m_postProcessing;
};
Q_DECLARE_METATYPE(GameEngine*)

#endif

