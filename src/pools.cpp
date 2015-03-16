#include "pools.h"
#include "texture.h"
#include "terrain.h"
#include "settings.h"
#include "shader.h"

#include <math.h>

Texture* Pools::s_waterDisplacementMaps;
Shader* Pools::s_shader = NULL;
int Pools::s_reflectionLoc;
int Pools::s_refractionLoc;
int Pools::s_displacementMapLoc;
int Pools::s_tLoc;
int Pools::s_eyePosLoc;


Pool::Pool(Pools* pools, uint x, uint y, uint width, uint height)
	: m_pools(pools)
{
	init(x, y, width, height);
}

Pool::Pool(Pools* pools)
	: m_pools(pools)
{
}

Pool::~Pool()
{
	if (!m_displayListDirty)
		glDeleteLists(m_displayList, 1);
}

void Pool::init(uint x, uint y, uint width, uint height)
{
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
	m_displayListDirty = true;
	m_center = vec2(float(x) + float(width)/2.0f, float(y) + float(height)/2.0f);
	m_origin = vec2(float(x), float(y));

	connect(Settings::instance(), SIGNAL(waterShadersChanged(bool)), SLOT(invalidateDisplayList()));
}

void Pool::invalidateDisplayList()
{
	m_displayListDirty = true;
	glDeleteLists(m_displayList, 1);
}

void Pool::draw()
{
	if (!m_displayListDirty)
	{
		glCallList(m_displayList);
		return;
	}

	m_displayList = glGenLists(1);
	m_displayListDirty = false;

	glNewList(m_displayList, GL_COMPILE_AND_EXECUTE);
	draw(3);
	glEndList();
}

void Pool::draw(int tess)
{
	int n = int(powf(2.0f, tess));
	int w = n * m_width/TILE_SIZE;
	int h = n * m_height/TILE_SIZE;
	
	float spacing = float(TILE_SIZE) / float(n);
	
	for (int x=0 ; x<w ; ++x)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (int y=0 ; y<=h ; ++y)
		{
			float x1 = m_origin.x + x * spacing;
			float y1 = m_origin.y + y * spacing;
			float height1 = m_pools->terrain()->heightAt(vec2(x1, y1));

			float x2 = m_origin.x + (x+1) * spacing;
			float y2 = m_origin.y + y * spacing;
			float height2 = m_pools->terrain()->heightAt(vec2(x2, y2));

			if (Settings::instance()->waterShaders())
			{
				glVertex4f(x1, y1, height1, 0.0f);
				glVertex4f(x2, y2, height2, 0.0f);
			}
			else
			{
				glVertex3f(x1, -0.1f, y1);
				glVertex3f(x2, -0.1f, y2);
			}
		}
		glEnd();
	}
}

Pools::Pools(Terrain* terrain)
	: m_terrain(terrain)
{
	if (s_shader == NULL)
	{
		s_waterDisplacementMaps = new Texture(":/data/textures/waterdisplacementmaps.png");
		s_shader = new Shader(":/data/shaders/waterVert.glsl", ":/data/shaders/waterFrag.glsl");
		s_reflectionLoc = s_shader->uniformLocation("reflection");
		s_refractionLoc = s_shader->uniformLocation("refraction");
		s_displacementMapLoc = s_shader->uniformLocation("displacementMap");
		s_tLoc = s_shader->uniformLocation("t");
		s_eyePosLoc = s_shader->uniformLocation("eyePos");
	}
}


Pools::~Pools()
{
	foreach (Pool* pool, m_pools)
		delete pool;
}


void Pools::setupState(Texture* reflection, Texture* refraction, float t, const vec3& eyePos)
{
	m_reflection = reflection;
	m_refraction = refraction;
	
	s_shader->bind();
	glUniform1f(s_tLoc, t);
	glUniform3fv(s_eyePosLoc, 1, eyePos.data());
	
	m_reflection->bind(0, s_reflectionLoc);
	m_refraction->bind(1, s_refractionLoc);
	s_waterDisplacementMaps->bind(2, s_displacementMapLoc);

	/*glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);*/
}

void Pools::tearDownState()
{
	/*glDisable(GL_BLEND);*/
	
	s_waterDisplacementMaps->release();
	m_reflection->release();

	s_shader->release();
}

void Pools::setupStateFF()
{
	glColor4f(0.0f, 0.0f, 1.0f, 0.25f);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
}

void Pools::tearDownStateFF()
{
	glDisable(GL_BLEND);
}

