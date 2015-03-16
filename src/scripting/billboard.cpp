#include "billboard.h"
#include "scriptmanager.h"
#include "worldobject.h"
#include "texture.h"
#include "gamestate.h"
#include "shader.h"


Shader* Billboard::s_shader = NULL;
int Billboard::s_texLoc;


Billboard::Billboard(WorldObject* parent)
	: QObject(parent),
	  m_worldObject(parent),
	  m_enabled(false),
	  m_distance(0.0),
	  m_terrainValidTarget(false),
	  m_texture(NULL)
{
	m_worldObject->gameState()->addBillboard(this);
	
	if (s_shader == NULL)
	{
		s_shader = new Shader(":/data/shaders/billboardVert.glsl", ":/data/shaders/billboardFrag.glsl");
		s_texLoc = s_shader->uniformLocation("texture");
	}
}

Billboard::~Billboard()
{
	delete m_texture;
}

void Billboard::loadTexture(const QString& filename)
{
	m_texture = new Texture(filename, Texture::Mipmaps);
}

bool Billboard::isObjectValidTarget(WorldObject* target)
{
	QScriptValue object = m_objectValidTargetCallback.engine()->toScriptValue(this);
	
	QScriptValue targetObject;
	if (target)
		targetObject = m_objectValidTargetCallback.engine()->toScriptValue(target);
	
	QScriptValue ret = m_objectValidTargetCallback.call(object, QScriptValueList() << targetObject);
	ScriptManager::handleExceptions();
	
	if (!ret.isBoolean())
		return false;
	return ret.toBoolean();
}

void Billboard::dropped(const vec2& target)
{
	QScriptValue object = m_droppedCallback.engine()->toScriptValue(this);
	QScriptValue targetObject = m_droppedCallback.engine()->toScriptValue(target);
	
	m_droppedCallback.call(object, QScriptValueList() << targetObject);
	ScriptManager::handleExceptions();
}

void Billboard::dropped(WorldObject* target)
{
	QScriptValue object = m_droppedCallback.engine()->toScriptValue(this);
	QScriptValue targetObject = m_droppedCallback.engine()->toScriptValue(target);
	
	m_droppedCallback.call(object, QScriptValueList() << targetObject);
	ScriptManager::handleExceptions();
}

void Billboard::bind()
{
	s_shader->bind();
	glEnable(GL_BLEND);
}

void Billboard::release()
{
	s_shader->release();
	glDisable(GL_BLEND);
}

void Billboard::draw(DrawingHint hint)
{
	if (!m_texture)
		return;
	
	m_texture->bind(0, s_texLoc);
	
	if (!m_enabled)
		glColor4f(0.25, 0.25, 0.25, 0.5);
	else
	{
		switch (hint)
		{
			case Normal:
			case DraggingValid:   glColor4f(0.5, 0.5, 0.5, 1.0); break;
			case DraggingInvalid: glColor4f(0.25, 0.25, 0.25, 0.25); break;
		}
	}
	
	const float size = 0.4;
	glBegin(GL_QUADS);
		glTexCoord4f(0.0, 1.0, -size, -size);
		glVertex3f(0.0, 2.0, 0.0);
		
		glTexCoord4f(1.0, 1.0, size, -size);
		glVertex3f(0.0, 2.0, 0.0);
		
		glTexCoord4f(1.0, 0.0, size, size);
		glVertex3f(0.0, 2.0, 0.0);
		
		glTexCoord4f(0.0, 0.0, -size, size);
		glVertex3f(0.0, 2.0, 0.0);
	glEnd();
	
	m_texture->release();
}

