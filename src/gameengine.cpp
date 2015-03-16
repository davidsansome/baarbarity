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

#include "gameengine.h"
#include "settings.h"
#include "gamestate.h"
#include "terrain.h"
#include "gluimanager.h"
#include "gui/gamemenudialog.h"
#include "gui/optionsdialog.h"
#include "gui/progressdialog.h"
#include "gui/shadereditor.h"
#include "gui/menuhud.h"
#include "gui/gamehud.h"
#include "sheep.h"
#include "texture.h"
#include "opengl.h"
#include "pools.h"
#include "shader.h"
#include "postprocessing.h"
#include "actions/action.h"
#include "scripting/scriptmanager.h"
#include "scripting/billboard.h"
#include "scripting/timer.h"

#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResource>
#include <QDebug>
#include <QKeyEvent>
#include <QGLFramebufferObject>

#include <cmath>

#define NAME_WORLDOBJECT 10000
#define NAME_BILLBOARD   10001

Shader* GameEngine::s_baseLayerShader = NULL;
Shader* GameEngine::s_worldObjectShader = NULL;


FloatingText::FloatingText(const QString& t, const QColor& c, const vec3& p)
{
	text = t;
	color = c;
	pos = p;
	height = 0.0f;
}

bool FloatingText::update(int timeDelta)
{
	height += 0.0005f * timeDelta;
	return height <= 1.0f;
}

float FloatingText::opacity() const
{
	return 1.0f - (height*height);
}

GameEngine::GameEngine(QWidget* parent)
	: QGLWidget(parent),
	  m_gameState(NULL),
	  m_fps(0),
	  m_frameCount(0),
	  m_rotateDragging(false),
	  m_billboardDragging(NULL),
	  m_reflection(NULL),
	  m_refraction(NULL),
	  m_reflectionFbo(NULL),
	  m_refractionFbo(NULL),
	  m_t(0.0f),
	  m_hoverObject(NULL),
	  m_selectedObject(NULL),
	  m_shaderEditor(NULL),
	  m_menuMode(true)
{
	m_moveKeyPressed[0] = false;
	m_moveKeyPressed[1] = false;
	m_moveKeyPressed[2] = false;
	m_moveKeyPressed[3] = false;
	
	m_vertAngle = M_PI_2*0.30f;
	m_horizAngle = 0.0f;
	m_horizAngle.setModulo(M_PI*2);
	m_horizAngle.setLimit(0.001f);
	m_vertAngle.setLimit(0.001f);
	m_vertAngle.updateImmediately();
	m_horizAngle.updateImmediately();
	updateCameraRotation();
	
	m_zoomDistance = 40.0f;
	m_zoomDistance.updateImmediately();
	
	// Setup the redraw timer
	m_redrawTimer = new QTimer(this);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(update()));
	m_redrawTimer->setSingleShot(true);
	
	m_fpsTimer = new QTimer(this);
	connect(m_fpsTimer, SIGNAL(timeout()), SLOT(updateFps()));
	m_fpsTimer->start(1000);

	connect(Settings::instance(), SIGNAL(farClippingDistanceChanged(float, float)), SLOT(farClippingDistanceChanged()));
	
	setMouseTracking(true);
	m_hoverTimer.start();
	
	setAttribute(Qt::WA_NoSystemBackground);
	
	m_postProcessing = new PostProcessing();

	m_uiManager = new GLUIManager(this);

	m_menuHud = new MenuHud(this);
	m_gameHud = new GameHud(this);
	m_optionsDialog = new OptionsDialog(this);
	m_gameMenuDialog = new GameMenuDialog(this);
	m_progressDialog = new ProgressDialog(this);
	
	ScriptManager::init(this);
	ScriptManager::loadAllScripts();
	
	m_uiManager->setHud(m_menuHud);
	// Don't put things after setHud
}

GameEngine::~GameEngine()
{
	delete m_skyBox[0];
	delete m_skyBox[1];
	delete m_skyBox[2];
	delete m_skyBox[3];
	delete m_skyBox[4];
	delete m_postProcessing;
	delete m_reflectionFbo;
	delete m_refractionFbo;
	delete m_reflection;
	delete m_refraction;
}

void GameEngine::setState(GameState* gameState, bool menuMode)
{
	delete m_gameState;
	m_gameState = gameState;
	connect(m_gameState, SIGNAL(loyaltyChanged(Sheep*, int)), SLOT(sheepLoyaltyChanged(Sheep*, int)));
	connect(m_gameState, SIGNAL(objectRemoved(WorldObject*)), SLOT(objectRemoved(WorldObject*)));
	emit stateChanged(gameState);
	
	ScriptManager::setGameHud(m_gameHud);

	m_menuMode = menuMode;
	
	if (!menuMode)
	{
		m_uiManager->setHud(m_gameHud);
		m_gameHud->setSelectedObject(NULL);
		m_gameHud->appendChatLine("Baaaaaa");

		// Set the camera location to the location of our human player
		vec3 home = gameCoordToGl(m_gameState->humanPlayer()->homeLocation());
		m_cameraOffset[0] = home.x;
		m_cameraOffset[2] = home.z;
		m_cameraOffset[0].updateImmediately();
		m_cameraOffset[2].updateImmediately();

		// Set the camera rotation so it looks towards the center of the terrain
		if (home.z == 0.0f)
			m_horizAngle = home.x > 0.0f ? M_PI_2 : -M_PI_2;
		else
			m_horizAngle = atanf(home.x/home.z);
		if (home.z < 0.0f)
			m_horizAngle += m_horizAngle < 0.0 ? -M_PI_2 : M_PI_2;
		
		m_horizAngle.updateImmediately();
	}
	
	m_selectedObject = NULL;

	updateCameraRotation();
	updateLightPosition();
}

void GameEngine::initializeGL()
{
	setupWinGLFunctions();

	// Check OpenGL extensions
	QStringList extensions = QString((const char*)glGetString(GL_EXTENSIONS)).split(' ');
	if (extensions.contains("GL_EXT_texture_compression_s3tc"))
		Settings::instance()->setTextureCompression(true);

	m_skyBox[0] = new Texture(":/data/textures/sky-back.png", Texture::Mirrored | Texture::Mipmaps);
	m_skyBox[1] = new Texture(":/data/textures/sky-front.png", Texture::Mirrored | Texture::Mipmaps);
	m_skyBox[2] = new Texture(":/data/textures/sky-left.png", Texture::Mirrored | Texture::Mipmaps);
	m_skyBox[3] = new Texture(":/data/textures/sky-right.png", Texture::Mirrored | Texture::Mipmaps);
	m_skyBox[4] = new Texture(":/data/textures/sky-top.png", Texture::Mirrored | Texture::Mipmaps);

	s_baseLayerShader = new Shader(":/data/shaders/baseLayerVert.glsl", ":/data/shaders/baseLayerFrag.glsl");
	s_worldObjectShader = new Shader(":/data/shaders/worldObjectVert.glsl", ":/data/shaders/worldObjectFrag.glsl");

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*GameState* state = new GameState();
	state->load(":/data/state/menu.baa");
	setState(state, true);*/
	
	m_postProcessing->init();
}

void GameEngine::resizeGL(int width, int height)
{
	glFogf(GL_FOG_START, Settings::instance()->fogStart());
	glFogf(GL_FOG_END, Settings::instance()->farClippingDistance());
	
	delete m_reflectionFbo;
	delete m_refractionFbo;
	delete m_refraction;
	delete m_reflection;
	// TODO: Make these power of 2
	m_reflectionFbo = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth);
	m_reflection = new Texture(m_reflectionFbo->texture());
	m_refractionFbo = new QGLFramebufferObject(width, height, QGLFramebufferObject::Depth);
	m_refraction = new Texture(m_refractionFbo->texture());
	
	m_postProcessing->windowResized(width, height);
}

void GameEngine::setupViewport()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)width() / (float)height(), 0.1f, Settings::instance()->farClippingDistance());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GameEngine::setupGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	float global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
	
	float ambientLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	float fogColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_START, Settings::instance()->fogStart());
	glFogf(GL_FOG_END, Settings::instance()->farClippingDistance());
}

void GameEngine::setupCamera()
{
	gluLookAt(m_eyePos.x, m_eyePos.y, m_eyePos.z,
	          m_lookAtPos.x, m_lookAtPos.y, m_lookAtPos.z,
		  0.0f, 1.0f, 0.0f);
	
	m_frustum.getPlaneEquations();
}

void GameEngine::calculateCameraPosition()
{
	m_lookAtPos = m_cameraOffset;
	m_eyePos = m_lookAtPos + m_rotationOffset * m_zoomDistance;
	
	// Adjust the camera position if it would be under the terrain
	if (m_gameState)
	{
		vec2 gameCoord = glToGameCoord(m_eyePos);
		float height = m_gameState->terrain()->heightAt(gameCoord);
		float yDiff = qMax(0.0f, height + 0.5f - m_eyePos.y);
		
		m_eyePos.y += yDiff;
		m_lookAtPos.y += yDiff;
	}
}

void GameEngine::draw3D()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setupViewport();

	calculateCameraPosition();

	if (m_gameState)
		renderSky(false);
	
	setupCamera();
	
	glLightfv(GL_LIGHT0, GL_POSITION, m_lightPosition);

	if (m_gameState)
	{
		renderBaseLayer();
		
		if (Settings::instance()->drawWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderTerrain();
		renderWater();
		if (Settings::instance()->drawWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (Settings::instance()->drawModels())
			renderWorldObjects();

		if (Settings::instance()->drawOriginCube())
			renderTestCube();
		
		if (m_billboardDragging)
			renderDropCircle();
		
		renderBillboards();
	}
}

void GameEngine::draw3DReflections()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setupViewport();

	calculateCameraPosition();

	if (m_gameState)
		renderSky(true);
	
	setupCamera();
	
	glScalef(1.0, -1.0, 1.0);
	double plane[4] = {0.0, 1.0, 0.0, -0.001};
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);
	
	glLightfv(GL_LIGHT0, GL_POSITION, m_lightPosition);

	if (m_gameState)
	{
		if (Settings::instance()->drawWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderTerrain();
		if (Settings::instance()->drawWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (Settings::instance()->drawModels())
			renderWorldObjects();
		
		renderBillboards();
	}
	
	glDisable(GL_CLIP_PLANE0);
}

void GameEngine::draw3DRefractions()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setupViewport();

	calculateCameraPosition();
	setupCamera();
	
	glLightfv(GL_LIGHT0, GL_POSITION, m_lightPosition);

	if (m_gameState)
	{
		if (Settings::instance()->drawWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		renderTerrain();
		if (Settings::instance()->drawWireframe())
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (Settings::instance()->drawModels())
			renderWorldObjects();
	}
}

void GameEngine::draw2D(QPainter& painter)
{
	painter.setPen(QPen(Qt::white));
	painter.drawText(10, 20, "FPS: " + QString::number(m_fps));
	
	if (m_hoverObject != NULL && !m_mouseCursorPos.isNull())
	{
		QString name;
		QString description;
		QColor color;
		
		if (qobject_cast<WorldObject*>(m_hoverObject))
		{
			WorldObject* o = qobject_cast<WorldObject*>(m_hoverObject);
			name = o->name();
			if (o->currentAction())
				description = o->currentAction()->name();
			color = o->playerColor();
		}
		else if (qobject_cast<Billboard*>(m_hoverObject))
		{
			Billboard* b = qobject_cast<Billboard*>(m_hoverObject);
			name = b->name();
			description = b->description();
			color = b->worldObject()->playerColor();
		}
		
		m_uiManager->setTooltip(name, description, m_mouseCursorPos, color);
	}

	if (m_gameHud && m_gameState)
		m_gameHud->recheckAbilityRequirements();
	
	m_uiManager->draw(painter);
}

void GameEngine::paintEvent(QPaintEvent *event)
{
	QTime frameTime;
	frameTime.start();

	// Get the object under the mouse cursor
	
	// 2D Initialisation
	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	
	// Save 2D GL state
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	// Initialize 3D GL state
	setupGL();
	
	// Draw the things to be reflected into a buffer
	m_reflectionFbo->bind();
	draw3DReflections();
	m_reflectionFbo->release();
	
	// Draw refractions into a buffer
	m_refractionFbo->bind();
	draw3DRefractions();
	m_refractionFbo->release();
	
	// Paint the 3D scene
	m_postProcessing->prepareBuffers();
	
	if (Settings::instance()->isFogEnabled())
		glEnable(GL_FOG);
	draw3D();
	if (Settings::instance()->isFogEnabled())
		glDisable(GL_FOG);
	
	m_postProcessing->run();
	
	m_hoverObject = objectAtPoint(m_mouseCursorPos);

	// Save the 3D matrices for use when drawing floating text
	double modelViewMatrix[16];
	double projectionMatrix[16];
	int viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
	glGetIntegerv(GL_VIEWPORT, (GLint*)viewport);
	
	// Restore the 2D GL state
	glPopAttrib();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	// Draw 2D scene
	draw2D(painter);

	// Draw floating text
	drawFloatingText(painter, modelViewMatrix, projectionMatrix, viewport);
	
	painter.end();

	// Update values
	updateValues(qMax(Settings::instance()->targetFrameTime(), frameTime.elapsed()));

	// Do FPS stuff
	m_redrawTimer->start(qMax(Settings::instance()->targetFrameTime() - frameTime.elapsed(), 0));
	m_frameCount++;
}

void GameEngine::updateValues(int timeDelta)
{
	m_zoomDistance.update(timeDelta);

	if (m_vertAngle.needsUpdating() || m_horizAngle.needsUpdating())
		updateCameraRotation();

	/*if (m_vertAngle.needsUpdating() || m_horizAngle.needsUpdating() ||
	    m_cameraOffset[0].needsUpdating() ||
	    m_cameraOffset[1].needsUpdating() ||
	    m_cameraOffset[2].needsUpdating())
		updateViewFrustum();*/

	m_vertAngle.update(timeDelta);
	m_horizAngle.update(timeDelta);

	updateCameraPosition(timeDelta);

	m_cameraOffset[0].update(timeDelta);
	m_cameraOffset[1].update(timeDelta);
	m_cameraOffset[2].update(timeDelta);
	
	if (m_gameState)
		m_gameState->updateValues(timeDelta);

	foreach (FloatingText* text, m_floatingText)
	{
		if (!text->update(timeDelta))
		{
			m_floatingText.removeAll(text);
			delete text;
		}
	}

	m_uiManager->updateValues(timeDelta);
	
	Timer::tick(timeDelta);

	m_t += float(timeDelta) / 5000.0f;
}

void GameEngine::updateCameraPosition(int timeDelta)
{
	if (!m_gameState)
		return;
	
	float vector[] = {0.0f, 0.0f};

	if (!m_menuMode)
	{
		if (m_moveKeyPressed[0]) vector[1] -= 0.06f * timeDelta;
		if (m_moveKeyPressed[1]) vector[0] += 0.06f * timeDelta;
		if (m_moveKeyPressed[2]) vector[1] += 0.06f * timeDelta;
		if (m_moveKeyPressed[3]) vector[0] -= 0.06f * timeDelta;
	}

	vec3 origin(gameCoordToGl(vec2(0.0f, 0.0f)));
	vec3 extent(gameCoordToGl(vec2(m_gameState->terrain()->width(), m_gameState->terrain()->height())));

	float xDelta = vector[0] * cosf(-m_horizAngle) - vector[1] * sinf(-m_horizAngle);
	float yDelta = vector[0] * sinf(-m_horizAngle) + vector[1] * cosf(-m_horizAngle);

	float zoomMod = m_zoomDistance / 40.0f;

	m_cameraOffset[0] = qBound(origin.x, m_cameraOffset[0].targetValue() + xDelta*zoomMod, extent.x);
	m_cameraOffset[2] = qBound(origin.z, m_cameraOffset[2].targetValue() + yDelta*zoomMod, extent.z);
}

void GameEngine::updateCameraRotation()
{
	m_rotationOffset.x = sinf(m_horizAngle) * cosf(m_vertAngle);
	m_rotationOffset.y = sinf(m_vertAngle);
	m_rotationOffset.z = cosf(m_horizAngle) * cosf(m_vertAngle);
}

void GameEngine::updateLightPosition()
{
	m_lightPosition[0] = m_gameState->terrain()->width()/2 * Settings::instance()->gameToGl();
	m_lightPosition[1] = 30.0f;
	m_lightPosition[2] = 0.0f;
	m_lightPosition[3] = 0.0f;
}

void GameEngine::updateFps()
{
	m_fps = m_frameCount;
	m_frameCount = 0;
}

void GameEngine::renderTestCube()
{
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);

		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
	glEnd();
}

void GameEngine::renderTerrain(bool useFrustum)
{
	Terrain* terrain = m_gameState->terrain();

	glPushMatrix();
		translateToGameCoord(vec2(0.0f, 0.0f));
		
		if (Settings::instance()->terrainShaders())
			terrain->draw();
		else
			terrain->drawFF();
	glPopMatrix();
}

void GameEngine::renderWater()
{
	Terrain* terrain = m_gameState->terrain();

	glPushMatrix();
		translateToGameCoord(vec2(0.0f, 0.0f));
		
		if (Settings::instance()->terrainShaders())
			terrain->drawWater(m_reflection, m_refraction, m_t,
			                   vec3(m_eyePos.x + (m_gameState->terrain()->width()-1) / 2.0f,
                                                m_eyePos.y,
                                                m_eyePos.z + (m_gameState->terrain()->height()-1) / 2.0f));
		else
			terrain->drawWaterFF();
	glPopMatrix();
}

void GameEngine::renderWorldObjects()
{
	if (Settings::instance()->terrainShaders())
	{
		s_worldObjectShader->bind();
	}
	else
	{
		glEnable(GL_LIGHTING);
	}
	
	foreach (WorldObject* object, m_gameState->worldObjects())
	{
		//if (m_frustum.sphereInFrustum());
		glPushMatrix();
			translateToGameCoord(object->pos(), true);
			object->draw();
		glPopMatrix();
	}

	if (Settings::instance()->terrainShaders())
		s_worldObjectShader->release();
	else
	{
		glDisable(GL_LIGHTING);
	}
}

vec2 GameEngine::glToGameCoord(const vec3& gl)
{
	return vec2(gl.x / Settings::instance()->gameToGl() + (m_gameState->terrain()->width()-1) / 2.0f,
	              gl.z / Settings::instance()->gameToGl() + (m_gameState->terrain()->height()-1) / 2.0f);
}

vec3 GameEngine::gameCoordToGl(const vec2& gameCoord, bool adjustToTerrainHeight)
{
	float height = 0.0f;
	if (adjustToTerrainHeight)
		height = m_gameState->terrain()->heightAt(gameCoord);
	
	return vec3((gameCoord.x - float(m_gameState->terrain()->width()-1) / 2.0f) * Settings::instance()->gameToGl(),
	              height,
	              (gameCoord.y - float(m_gameState->terrain()->height()-1) / 2.0f) * Settings::instance()->gameToGl());
}

void GameEngine::translateToGameCoord(const vec2& gameCoord, bool adjustToTerrainHeight)
{
	vec3 c = gameCoordToGl(gameCoord, adjustToTerrainHeight);
	glTranslatef(c.x, c.y, c.z);
}

void GameEngine::mousePressEvent(QMouseEvent* event)
{
	m_dragStartPoint = event->pos();
	m_dragStartObject = m_hoverObject;
}

void GameEngine::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_rotateDragging && !m_menuMode)
	{
		m_rotateDragging = false;
		m_vertAngle.finishedEditing();
		m_horizAngle.finishedEditing();
	}
	else if (m_billboardDragging && !m_menuMode)
	{
		if (isBillboardTargetValid())
		{
			WorldObject* o = qobject_cast<WorldObject*>(m_hoverObject);
			if (o)
				m_billboardDragging->dropped(o);
			else
				m_billboardDragging->dropped(m_billboardDraggingPos);
			delete m_billboardDragging;
		}
		
		m_billboardDragging = NULL;
	}
	else if (event->button() == Qt::RightButton && !m_menuMode)
	{
		if (m_selectedObject && m_gameState && m_selectedObject->player() == m_gameState->humanPlayer())
		{
			vec3 glCoord = screenCoordToGl(event->pos());
			vec2 gameCoord = glToGameCoord(glCoord);
			QObject* object = objectAtPoint(event->pos());
			
			if (qobject_cast<WorldObject*>(object))
				m_selectedObject->startContextAction(qobject_cast<WorldObject*>(object));
			else if (gameCoord.x>0.0f && gameCoord.y>0.0f &&
			         gameCoord.x<m_gameState->terrain()->width() &&
			         gameCoord.y<m_gameState->terrain()->height() &&
			         glCoord.y>-1.0f && glCoord.y<Settings::instance()->maxHeight() + 1.0f)
			{
				m_selectedObject->startContextAction(gameCoord);
			}
		}
	}
	
	if (event->button() == Qt::LeftButton && !m_menuMode)
	{
		QObject* o = objectAtPoint(event->pos());
		WorldObject* object = qobject_cast<WorldObject*>(o);
		
		m_selectedObject = (object == NULL) ? NULL : object->selectionProxy();
		if (m_gameState)
			m_gameHud->setSelectedObject(m_selectedObject);
	}
}

void GameEngine::mouseMoveEvent(QMouseEvent* event)
{
	if (m_rotateDragging && !m_menuMode)
	{
		m_vertAngle = m_startVertAngle + (event->pos().y() - m_dragStartPoint.y()) * Settings::instance()->rotationSensitivityY();
		m_horizAngle = m_startHorizAngle + (m_dragStartPoint.x() - event->pos().x()) * Settings::instance()->rotationSensitivityX();

		m_vertAngle = qBound(0.0f, m_vertAngle.targetValue(), float(M_PI_2) - 0.001f);
		
		updateCameraRotation();
	}
	else if (m_billboardDragging && !m_menuMode)
	{
		vec3 glCoord(screenCoordToGl(event->pos()));
		vec2 gameCoord(glToGameCoord(glCoord));
		
		if (gameCoord.x>0.0f && gameCoord.y>0.0f &&
		    gameCoord.x<m_gameState->terrain()->width() &&
		    gameCoord.y<m_gameState->terrain()->height() &&
		    glCoord.y>-1.0f && glCoord.y<Settings::instance()->maxHeight() + 1.0f)
		{
			m_billboardDraggingPos = gameCoord;
		}
	}
	else if (event->buttons() != 0 && !m_menuMode)
	{
		if ((event->pos() - m_dragStartPoint).manhattanLength() > 3)
		{
			//m_hoverObject = NULL;
			if (event->buttons() & Qt::RightButton)
			{
				m_startVertAngle = m_vertAngle.targetValue();
				m_startHorizAngle = m_horizAngle.targetValue();
				m_rotateDragging = true;
			}
			else if (event->buttons() & Qt::LeftButton && qobject_cast<Billboard*>(m_dragStartObject))
			{
				Billboard* b = qobject_cast<Billboard*>(m_dragStartObject);
				
				if (b->isEnabled())
					m_billboardDragging = b;
			}
		}
	}
	
	m_mouseCursorPos = event->pos();
}

void GameEngine::wheelEvent(QWheelEvent* event)
{
	if (m_menuMode)
		return;
	
	m_zoomDistance = m_zoomDistance.targetValue() - event->delta() * Settings::instance()->zoomSensitivity();
	m_zoomDistance = qBound(5.0f, m_zoomDistance.targetValue(), 40.0f);
}

QObject* GameEngine::objectAtPoint(const QPoint& pos, const QList<QObject*>* fineSelection)
{
	if (!m_gameState)
		return NULL;
	
	// Use picking to find the object at a certain point (eg, under the mouse cursor)
	// If fineSelection is NULL, only the bounding boxes of objects will be drawn.  If there
	// are two or more bounding boxes at the point, the function will call itself again
	// with fineSelection set to a list of those objects.  On this second pass, only these
	// objects will be drawn, but at full detail - meaning the one that is actually under the
	// cursor will be returned.
	
	GLint viewport[4];
	uint selectBuf[512];
	
	const int border = m_postProcessing->borderSize();
	const QPoint adjustedPos(pos.x() + border, pos.y() + border);

	// Enter selection mode
	glSelectBuffer(512, (GLuint*)selectBuf);
	glRenderMode(GL_SELECT);
	
	glViewport(0, 0, width() + border*2, height() + border*2);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// If the cursor is near (within 20 pixels of) a single object, that one will be returned.
	// However if there is more than one object near the cursor, fineSelection will be non-null,
	// and we should look specifically for the object under the cursor.
	int size = fineSelection ? 1 : 20;

	glGetIntegerv(GL_VIEWPORT,viewport);
	gluPickMatrix(adjustedPos.x(), viewport[3]-adjustedPos.y(), size, size, viewport);
	gluPerspective(45.0f, float(width())/height(), 0.1f, Settings::instance()->farClippingDistance());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glInitNames();
	
	setupGL();
	setupCamera();
	
	QList<WorldObject*> objects(m_gameState->worldObjects());
	QList<Billboard*> billboards(m_gameState->billboards());
	
	glColorMask(false, false, false, false);
	
	// Draw all the objects in the world
	for (int i=0 ; i<objects.count() ; ++i)
	{
		WorldObject* object = objects[i];
		
		if (fineSelection && !fineSelection->contains(object))
			continue;
		
		glPushMatrix();
		glPushName(NAME_WORLDOBJECT);
		glPushName(i);
			translateToGameCoord(object->pos(), true);
			if (fineSelection)
				object->draw();
			else
				object->drawBoundingBox();
		glPopName();
		glPopName();
		glPopMatrix();
	}
	
	// Draw all the billboards
	if (billboards.count() > 0)
	{
		Billboard::bind();
		for (int i=0 ; i<billboards.count() ; ++i)
		{
			Billboard* b = billboards[i];
			
			if (fineSelection && !fineSelection->contains(b) || b == m_billboardDragging)
				continue;
			
			glPushMatrix();
			glPushName(NAME_BILLBOARD);
			glPushName(i);
				translateToGameCoord(b->worldObject()->pos(), true);
				b->draw();
			glPopName();
			glPopName();
			glPopMatrix();
		}
		Billboard::release();
	}
	
	glColorMask(true, true, true, true);
	
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	
	// Come out of selection mode
	int hits = glRenderMode(GL_RENDER);

	QList<QObject*> hitList;
	
	// Go through the hit list looking for the object that was closest to the camera
	int bufI = 0;
	QObject* object = NULL;
	uint distance = UINT_MAX;
	for (int i=0 ; i<hits ; ++i)
	{
		uint names = selectBuf[bufI++];
		uint minDistance = selectBuf[bufI+=2];
		
		if (names >= 2 && selectBuf[bufI] == NAME_WORLDOBJECT)
		{
			// It's an object
			hitList << objects[selectBuf[bufI+1]];
			if (minDistance < distance)
			{
				// It's closer than any we've previously found
				distance = minDistance;
				object = objects[selectBuf[bufI+1]];
			}
		}
		else if (names >= 2 && selectBuf[bufI] == NAME_BILLBOARD)
		{
			// It's a billboard
			hitList << billboards[selectBuf[bufI+1]];
			if (minDistance < distance)
			{
				// It's closer than any we've previously found
				distance = minDistance;
				object = billboards[selectBuf[bufI+1]];
			}
		}
		
		bufI += names;
	}
	
	if (!object) // Zero objects were matched
		return NULL;
	else if (hitList.count() > 1 && fineSelection == NULL)
	{
		// More than one object was matched - call the function again
		// with a list of these objects, and ask it to draw their exact shapes
		// rather than bounding boxes.
		QObject* fineObject = objectAtPoint(pos, &hitList);
		if (fineObject)
			return fineObject;
	}

	return object;
}

void GameEngine::keyPressEvent(QKeyEvent* event)
{
	switch(event->key())
	{
	case Qt::Key_Escape:
		if (m_selectedObject)
		{
			m_selectedObject = NULL;
			m_gameHud->setSelectedObject(NULL);
		}
		else
			m_uiManager->showDialog(m_gameMenuDialog);
		break;
	
	case Qt::Key_W: case Qt::Key_Up:    m_moveKeyPressed[0] = true; break;
	case Qt::Key_D: case Qt::Key_Right: m_moveKeyPressed[1] = true; break;
	case Qt::Key_S: case Qt::Key_Down:  m_moveKeyPressed[2] = true; break;
	case Qt::Key_A: case Qt::Key_Left:  m_moveKeyPressed[3] = true; break;
	}
}

void GameEngine::keyReleaseEvent(QKeyEvent* event)
{
	switch(event->key())
	{
	case Qt::Key_W: case Qt::Key_Up:    m_moveKeyPressed[0] = false; break;
	case Qt::Key_D: case Qt::Key_Right: m_moveKeyPressed[1] = false; break;
	case Qt::Key_S: case Qt::Key_Down:  m_moveKeyPressed[2] = false; break;
	case Qt::Key_A: case Qt::Key_Left:  m_moveKeyPressed[3] = false; break;
	}
}

void GameEngine::showOptionsDialog()
{
	m_uiManager->showDialog(m_optionsDialog);
}

void GameEngine::showShaderEditor()
{
	if (!m_shaderEditor)
		m_shaderEditor = new ShaderEditor(NULL);
	m_shaderEditor->show();
}

vec3 GameEngine::screenCoordToGl(const QPoint& pos)
{
	glViewport(0, 0, width(), height());
	setupViewport();
	setupCamera();
	
	float x = pos.x();
	float y = pos.y();
	double modelview[16], projection[16], glPos[3];
	GLint viewport[4];
	float z;

	// Trying to read a pixel that's outside the viewport crashes
	// some systems (such as Thinkpads with Intel graphics chips).
	if (x<0) x=0;
	else if (x>=width()) x=width()-1;
	if (y<0) y=0;
	else if (y>=height()) y=height()-1;

	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetIntegerv( GL_VIEWPORT, viewport );

	m_postProcessing->sceneReadPixels(x, viewport[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);

	gluUnProject( x, viewport[3]-y, z, modelview,
			projection, viewport, &glPos[0], &glPos[1], &glPos[2] );

	return vec3(glPos[0], glPos[1], glPos[2]);
}

void GameEngine::drawCircle(const vec2& center, float radius, int resolution)
{
	const float radius2 = radius + 0.5;
	const float pInc = 2.0*M_PI / float(resolution);
	int i=0;

	glBegin(GL_QUAD_STRIP);
	for (float p=0.0 ; i<=resolution ; ++i, p+=pInc)
	{
		vec2 lowPoint(center.x + radius * cosf(p),
		              center.y + radius * sinf(p));
		vec2 highPoint(center.x + radius2 * cosf(p),
		               center.y + radius2 * sinf(p));

		glVertex3f(lowPoint.x, gameState()->terrain()->heightAt(lowPoint), lowPoint.y);
		glVertex3f(highPoint.x, gameState()->terrain()->heightAt(highPoint) + 1.0, highPoint.y);
	}
	glEnd();
}

void GameEngine::renderDropCircle()
{
	vec2 center(m_billboardDragging->worldObject()->pos());
	QColor color(m_billboardDragging->worldObject()->playerColor());

	glEnable(GL_BLEND);
	glPushMatrix();
		translateToGameCoord(vec2(0.0f, 0.0f));
		glTranslatef(0.0f, 0.1f, 0.0f);
		glColor4f(float(color.red())/255.0, float(color.green())/255.0, float(color.blue())/255.0, 0.75);
		drawCircle(center, m_billboardDragging->distance(), 50);
	glPopMatrix();
	glDisable(GL_BLEND);
}

void GameEngine::sheepLoyaltyChanged(Sheep* sheep, int amount)
{
	QString text = QString::number(amount);
	QColor color = Qt::red;
	if (amount > 0)
	{
		text = "+" + text;
		color = Qt::green;
	}

	vec3 pos = gameCoordToGl(sheep->pos(), true);
	pos.y += sheep->boundingSphereRadius();
	
	m_floatingText.append(new FloatingText(text, color, pos));
}

void GameEngine::objectRemoved(WorldObject* object)
{
	if (object == m_selectedObject)
	{
		m_selectedObject = NULL;
		m_gameHud->setSelectedObject(NULL);
	}
}

void GameEngine::drawFloatingText(QPainter& painter, double* model, double* proj, int* view)
{
	QFont font(painter.font());
	font.setPointSize(font.pointSize() + 2);
	font.setBold(true);
	QFontMetrics metrics(font);
	
	foreach (FloatingText* text, m_floatingText)
	{
		double win[3];
		
		gluProject(text->pos.x, text->pos.y, text->pos.z,
		           model, proj, (GLint*)view,
		           &win[0], &win[1], &win[2]);

		if (win[0] < 0 || win[0] >= width() || win[1] < 0 || win[1] >= height())
			continue;

		int w = metrics.width(text->text);
		int h = metrics.height();
		int x = qBound(0, int(win[0] - w/2), width() - w);
		int y = qBound(0, int(height() - win[1] - h - text->height*h*1.5), height() - h);
		QRect rect(x, y, w, h);
		
		painter.setPen(text->color);
		painter.setOpacity(text->opacity());
		painter.setFont(font);
		painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, text->text);
	}
}

void GameEngine::renderSky(bool upsideDown)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	gluLookAt(0.0f, 0.0f, 0.0f,
	          -m_rotationOffset.x, -m_rotationOffset.y, -m_rotationOffset.z,
	          0.0f, 1.0f, 0.0f);
	
	if (upsideDown)
		glScalef(1.0, -1.0, 1.0);

	// Back
	m_skyBox[0]->bind();
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();

	// Front
	m_skyBox[1]->bind();
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
	glEnd();

	// Left
	m_skyBox[2]->bind();
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();

	// Right
	m_skyBox[3]->bind();
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();
	
	if (upsideDown)
	{
		// Top
		m_skyBox[4]->bind();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-1.0f, 1.0f, -1.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(1.0f, 1.0f, -1.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(1.0f, 1.0f, 1.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-1.0f, 1.0f, 1.0f);
		glEnd();
	}
	
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void GameEngine::renderBaseLayer()
{
	if (Settings::instance()->terrainShaders())
	{
		s_baseLayerShader->bind();

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}
	
	float w = m_gameState->terrain()->width() - 1;
	float h = m_gameState->terrain()->height() - 1;

	glColor3f(0.10f, 0.15f, 0.0f);
	glNormal3f(0.0f, 1.0f, 0.0f);

	renderBaseLayer(vec2(-w*2, -h*2), w*5, h*2);
	renderBaseLayer(vec2(-w*2, h), w*5, h*2);
	renderBaseLayer(vec2(-w*2, 0.0f), w*2, h);
	renderBaseLayer(vec2(w, 0.0f), w*2, h);
	
	if (Settings::instance()->terrainShaders())
	{
		s_baseLayerShader->release();
		
		glDisable(GL_BLEND);
	}
}

void GameEngine::renderBaseLayer(const vec2& pos, float width, float height)
{
	float y = -9.9f;
	glPushMatrix();
		translateToGameCoord(pos, false);
		glBegin(GL_QUADS);
			glVertex3f(0.0f, y, 0.0f);
			glVertex3f(width, y, 0.0f);
			glVertex3f(width, y, height);
			glVertex3f(0.0f, y, height);
		glEnd();
	glPopMatrix();
}

void GameEngine::farClippingDistanceChanged()
{
	 resizeGL(width(), height());
}

void GameEngine::renderBillboards()
{
	QList<Billboard*> billboards = m_gameState->billboards();
	if (billboards.count() == 0)
		return;
	
	Billboard::bind();
	
	foreach (Billboard* b, billboards)
	{
		vec2 pos = b->worldObject()->pos();
		Billboard::DrawingHint hint = Billboard::Normal;
		
		if (b == m_billboardDragging)
		{
			pos = m_billboardDraggingPos;
			hint = isBillboardTargetValid() ? Billboard::DraggingValid : Billboard::DraggingInvalid;
		}
		
		glPushMatrix();
			translateToGameCoord(pos, true);
			b->draw(hint);
		glPopMatrix();
	}
	
	Billboard::release();
}

bool GameEngine::isBillboardTargetValid() const
{
	bool valid = false;
	WorldObject* o = qobject_cast<WorldObject*>(m_hoverObject);
	if (o)
		valid = m_billboardDragging->isObjectValidTarget(o);
	else
		valid = m_billboardDragging->isTerrainValidTarget() &&
		        (m_billboardDraggingPos - m_billboardDragging->worldObject()->pos()).len() < m_billboardDragging->distance();
	return valid;
}

