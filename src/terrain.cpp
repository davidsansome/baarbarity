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

#include "terrain.h"
#include "texture.h"
#include "gameengine.h"
#include "settings.h"
#include "utilities.h"
#include "pools.h"
#include "opengl.h"
#include "shader.h"

#include <QtGlobal>
#include <QDebug>

#include <math.h>
#include <stdlib.h>



Texture* Terrain::s_textures[4];
Shader* Terrain::s_shader = NULL;
int Terrain::s_rockTexLoc;
int Terrain::s_grassTexLoc;


QDataStream& operator <<(QDataStream& stream, const Pool& pool)
{
	pool.save(stream);
	return stream;
}

QDataStream& operator >>(QDataStream& stream, Pool& pool)
{
	pool.load(stream);
	return stream;
}

void Pool::save(QDataStream& stream) const
{
	stream << m_x << m_y << m_width << m_height;
}

void Pool::load(QDataStream& stream)
{
	quint32 x, y, w, h;
	stream >> x >> y >> w >> h;
	init(x, y, w, h);
}



Terrain::Terrain(int xTiles, int yTiles)
{
	init(xTiles, yTiles);
}

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
	delete[] m_heightMap;
	delete[] m_normalMap;

	delete m_alphaMap;
}

void Terrain::init(int xTiles, int yTiles)
{
	if (s_shader == NULL)
	{
		s_textures[0] = new Texture(":/data/textures/rock.png", Texture::Mipmaps);
		s_textures[1] = new Texture(":/data/textures/grass.png", Texture::Mipmaps);
		s_textures[2] = new Texture(":/data/textures/rock_s.png", Texture::Mipmaps);
		s_textures[3] = new Texture(":/data/textures/grass_s.png", Texture::Mipmaps);
		
		s_shader = new Shader(":/data/shaders/terrainVert.glsl", ":/data/shaders/terrainFrag.glsl");
		s_rockTexLoc = s_shader->uniformLocation("rockTex");
		s_grassTexLoc = s_shader->uniformLocation("grassTex");
	}
	
	m_alphaMap = new Texture();
	
	m_xTiles = xTiles;
	m_yTiles = yTiles;
	m_width = xTiles * TILE_SIZE + 1;
	m_height = yTiles * TILE_SIZE + 1;

	m_heightMap = new float[m_width * m_height];
	m_normalMap = new float[m_width * m_height * 3];
	m_pools = new Pools(this);

	for (int i=0 ; i<m_width*m_height ; ++i)
 		m_heightMap[i] = 0.0f;
}

void Terrain::generate(int iterations, int minDistance, int maxDistance, float flattenFactor)
{
	// Generate heightmap
	for (int i=0 ; i<iterations ; ++i)
	{
		int centerX = qrand() % m_width;
		int centerY = qrand() % m_height;
		int radius;
		if (maxDistance == minDistance)
			radius = maxDistance;
		else
			radius = minDistance + qrand() % (maxDistance - minDistance);
		int radiusSquared = int(pow(radius, 2));
		
		int x = qMax(0, centerX-radius);
		int initialY = qMax(0, centerY-radius);
		int maxX = qMin(m_width, centerX+radius);
		int maxY = qMin(m_height, centerY+radius);
		
		for ( ; x<maxX ; ++x)
		{
			for (int y=initialY ; y<maxY ; ++y)
			{
				int heightMod = radiusSquared - (int(pow(x - centerX, 2)) + int(pow(y - centerY, 2)));
				if (heightMod > 0)
					setHeightAt(x, y, heightAt(x, y) + heightMod);
			}
		}
	}
	
	normalize();
	flatten(flattenFactor);
	generateWater();

	finalize();
}

void Terrain::finalize()
{
	// Calculate vertex normals
	for (int x=0 ; x<m_width ; ++x)
		for (int y=0 ; y<m_height ; ++y)
			calculateVertexNormal(x, y);

	// Generate texture alphamaps
	int w = m_width*ALPHA_MAP_SIZE_MUL;
	int h = m_height*ALPHA_MAP_SIZE_MUL;
	uchar* tex = new uchar[w*h*4];

	for (int x=0 ; x<w ; ++x)
	{
		for (int y=0 ; y<h ; ++y)
		{
			vec2 point(float(x)/float(ALPHA_MAP_SIZE_MUL),
			             float(y)/float(ALPHA_MAP_SIZE_MUL));

			vec3 normal = normalAt(point);
			float value = pow(normal.y, 5);
			tex[(y*w + x)*4 + 0] = uchar(value*255.0f);
			tex[(y*w + x)*4 + 1] = uchar(value*255.0f);
			tex[(y*w + x)*4 + 2] = uchar(value*255.0f);
			tex[(y*w + x)*4 + 3] = uchar(value*255.0f);
		}
	}

	m_alphaMap->bind();
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, w, h, GL_RGBA, GL_UNSIGNED_BYTE, tex);
	delete[] tex;

	// Generate vertex buffers
	int horizEdgeElements = m_width*2;
	int vertEdgeElements = m_height*2;
	int elementsPerStrip3 = m_height*2 * 3;
	int elementsPerStrip2 = m_height*2 * 2;
	int size3 = elementsPerStrip3 * (m_width-1) + horizEdgeElements*3*2 + vertEdgeElements*3*2;
	int size2 = elementsPerStrip2 * (m_width-1) + horizEdgeElements*2*2 + vertEdgeElements*2*2;
	float* vertexData     = new float[size3];
	float* normalData     = new float[size3];
	float* texCoordData   = new float[size2];
	float* alphaCoordData = new float[size2];

	float* v = vertexData;
	float* n = normalData;
	float* t = texCoordData;
	float* a = alphaCoordData;
	
	for (int x=0 ; x<(m_width-1) ; ++x)
	{
		for (int y=0 ; y<m_height ; ++y)
		{
			addVertexToBuffer(v, n, t, a, x+1, y);
			v+=3; n+=3; t+=2; a+=2;
			addVertexToBuffer(v, n, t, a, x, y);
			v+=3; n+=3; t+=2; a+=2;
		}
	}

	finalizeHorizEdge(&v, &n, &t, &a, 0, 1.0f, 1);
	finalizeVertEdge(&v, &n, &t, &a, m_width-1, 1.0f, 1);
	finalizeHorizEdge(&v, &n, &t, &a, m_height-1, -1.0f, -1);
	finalizeVertEdge(&v, &n, &t, &a, 0, -1.0f, -1);
	
	glGenBuffers(4, (GLuint *)m_bufferObjects);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[0]);
	glBufferData(GL_ARRAY_BUFFER, size3 * sizeof(float), vertexData, GL_STATIC_DRAW);
	delete[] vertexData;

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[1]);
	glBufferData(GL_ARRAY_BUFFER, size3 * sizeof(float), normalData, GL_STATIC_DRAW);
	delete[] normalData;

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[2]);
	glBufferData(GL_ARRAY_BUFFER, size2 * sizeof(float), texCoordData, GL_STATIC_DRAW);
	delete[] texCoordData;

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[3]);
	glBufferData(GL_ARRAY_BUFFER, size2 * sizeof(float), alphaCoordData, GL_STATIC_DRAW);
	delete[] alphaCoordData;
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Terrain::finalizeHorizEdge(float** v, float** n, float** t, float** a, int y, float normalDirection, int incDirection)
{
	int start = incDirection>0 ? 0 : m_width-1;
	for (int x=start ; x<m_width && x>=0 ; x+=incDirection)
	{
		float h = qMax(0.0f, heightAt(x, y));
		(*v)[0] = x;      (*v)[1] = -10.0f;            (*v)[2] = y;
		(*n)[0] = 0.0f;   (*n)[1] = 0.0f;              (*n)[2] = normalDirection;
		(*t)[0] = float(x) / float(TILE_SIZE);
		(*t)[1] = (h + 10.0f) / float(TILE_SIZE);
		(*a)[0] = 0.0f; (*a)[1] = 0.0f;

		(*v)+=3; (*n)+=3; (*t)+=2; (*a)+=2;

		(*v)[0] = x;      (*v)[1] = h;                 (*v)[2] = y;
		(*n)[0] = 0.0f;   (*n)[1] = 0.0f;              (*n)[2] = normalDirection;
		(*t)[0] = float(x) / float(TILE_SIZE);
		(*t)[1] = 0.0f;
		(*a)[0] = 0.0f; (*a)[1] = 0.0f;

		(*v)+=3; (*n)+=3; (*t)+=2; (*a)+=2;
	}
}

void Terrain::finalizeVertEdge(float** v, float** n, float** t, float** a, int x, float normalDirection, int incDirection)
{
	int start = incDirection>0 ? 0 : m_height-1;
	for (int y=start ; y<m_height && y>=0 ; y+=incDirection)
	{
		float h = qMax(0.0f, heightAt(x, y));
		(*v)[0] = x;                 (*v)[1] = -10.0f;              (*v)[2] = y;
		(*n)[0] = normalDirection;   (*n)[1] = 0.0f;              (*n)[2] = 0.0f;
		(*t)[0] = (h + 10.0f) / float(TILE_SIZE);
		(*t)[1] = float(y) / float(TILE_SIZE);
		(*a)[0] = 0.0f; (*a)[1] = 0.0f;

		(*v)+=3; (*n)+=3; (*t)+=2; (*a)+=2;
		
		(*v)[0] = x;                 (*v)[1] = h;                 (*v)[2] = y;
		(*n)[0] = normalDirection;   (*n)[1] = 0.0f;              (*n)[2] = 0.0f;
		(*t)[0] = 0.0f;
		(*t)[1] = float(y) / float(TILE_SIZE);
		(*a)[0] = 0.0f; (*a)[1] = 0.0f;

		(*v)+=3; (*n)+=3; (*t)+=2; (*a)+=2;
	}
}

void Terrain::addVertexToBuffer(float* v, float* n, float* t, float* a, int x, int y)
{
	v[0] = x;
	v[1] = heightAt(x, y);
	v[2] = y;

	vec3 normal = normalAt(x, y);
	n[0] = normal.x;
	n[1] = normal.y;
	n[2] = normal.z;

	t[0] = float(x) / float(TILE_SIZE);
	t[1] = float(y) / float(TILE_SIZE);

	a[0] = float(x) / float(m_width);
	a[1] = float(y) / float(m_height);
}

void Terrain::generateWater()
{
	float limit = 0.10 * Settings::instance()->maxHeight();
	int minExtent = 8;
	int maxExtent = 24;
	
	int poolsMade = 0;
	int tries = 0;
	while (poolsMade < 15 && tries < 200)
	{
		tries++;
		
		// Pick a random coordinate to start from
		int x = qrand() % m_width;
		int y = qrand() % m_height;
		
		// Pick a random direction
		int dir = (qrand() % 2) ? 1 : -1;
		
		// Now look for a bit of terrain where the height stays below limit for extent tiles
		int poolStart = -1;
		bool poolBigEnough = false;
		for (; y<m_height && y>=0 ; y+=dir)
		{
			float h = heightAt(x, y);
			
			if (poolBigEnough && (h > limit || y == m_height-1 || y == 0 || qAbs(y - poolStart) > maxExtent)) // End of a possible pool area
			{
				int radius = qAbs(y - poolStart)/2;
				if (makePool(x, poolStart + dir*radius, radius))
					poolsMade++;
				break;
			}
			else if (h > limit) // Terrain too high
				poolStart = -1;
			else if (poolStart == -1) // Terrain suitable for a pool, start here
				poolStart = y;
			else if (qAbs(y - poolStart) >= minExtent) // We've had low terrain for 4 tiles
				poolBigEnough = true;
		}
	}
}

bool Terrain::makePool(int centerX, int centerY, int radius)
{
	int xMin = m_width;
	int xMax = 0;
	int yMin = m_height;
	int yMax = 0;
	bool madeAPool = false;

	// First pass - ensure we don't make 2 pools in the same location
	for (int x=-radius ; x<=radius ; ++x)
	{
		for (int y=-radius ; y<=radius ; ++y)
		{
			int realX = x + centerX;
			int realY = y + centerY;
			
			if (realX < 0 && realY < 0 && realX >= width() && realY >= height())
				continue;
			
			float h = heightAt(realX, realY);
			if (h < 0.0f)
				return false;
		}
	}

	// Second pass - make the pool
	for (int x=-radius ; x<=radius ; ++x)
	{
		for (int y=-radius ; y<=radius ; ++y)
		{
			int realX = x + centerX;
			int realY = y + centerY;
			
			if (realX < 0 || realY < 0 || realX >= width() || realY >= height())
				continue;
			
			float h = heightAt(realX, realY);
			
			float distanceFromMiddle = sqrt(x*x + y*y);
			if (distanceFromMiddle > radius)
				continue;
			
			float scaledDistanceFromMiddle = distanceFromMiddle/radius * M_PI;
			float heightMod = cos(scaledDistanceFromMiddle + M_PI) - 1;
			float newHeight = h + heightMod;
			
			setHeightAt(realX, realY, newHeight);
			
			if (newHeight < 0.0f)
			{
				madeAPool = true;
				if (realX < xMin) xMin = realX;
				if (realX > xMax) xMax = realX;
				if (realY < yMin) yMin = realY;
				if (realY > yMax) yMax = realY;
			}
		}
	}
	
	if (madeAPool)
		m_pools->addPool(new Pool(m_pools,
		                          qMax(0, xMin-1),
		                          qMax(0, yMin-1),
		                          qMin(m_width-2, xMax) - xMin + 2,
		                          qMin(m_height-2, yMax) - yMin + 2));
	return madeAPool;
}

void Terrain::calculateVertexNormal(int x, int y)
{
	float result[3];
	
	result[0] = 0.0f;
	result[1] = 0.0f;
	result[2] = 0.0f;

	// First quadrant
	if (x < m_width-1 && y < m_height-1)
	{
		calculatePlaneNormal(x, y, x+1, y, x+1, y+1, result);
		calculatePlaneNormal(x, y, x+1, y+1, x, y+1, result);
	}

	// Second quadrant
	if (x > 0 && y < m_height-1)
		calculatePlaneNormal(x, y, x, y+1, x-1, y, result);

	// Third quadrant
	if (x > 0 && y > 0)
	{
		calculatePlaneNormal(x, y, x-1, y, x-1, y-1, result);
		calculatePlaneNormal(x, y, x-1, y-1, x, y-1, result);
	}

	// Fourth quadrant
	if (x < m_width-1 && y > 0)
		calculatePlaneNormal(x, y, x, y-1, x+1, y, result);

	normalizeVector(result);

	setNormalAt(x, y, result);
}

void Terrain::calculatePlaneNormal(int x, int y, int x1, int y1, int x2, int y2, float* result)
{
	float v1[3] = {x1 - x,
	              heightAt(x1, y1) - heightAt(x, y),
	              y - y1};
	float v2[3] = {x2 - x,
	              heightAt(x2, y2) - heightAt(x, y),
	              y - y2};
	
	normalizeVector(v1);
	normalizeVector(v2);
	
	result[0] += v1[1] * v2[2] - v1[2] * v2[1];
	result[1] += v1[2] * v2[0] - v1[0] * v2[2];
	result[2] -= v1[0] * v2[1] - v1[1] * v2[0];
}

void Terrain::setHeightAt(int x, int y, float value)
{
	Q_ASSERT(x < m_width);
	Q_ASSERT(y < m_height);

	m_heightMap[y*m_width + x] = value;
}

void Terrain::setNormalAt(int x, int y, float* value)
{
	Q_ASSERT(x < m_width);
	Q_ASSERT(y < m_height);

	float* targetValue = m_normalMap + (y*m_width + x)*3;
	targetValue[0] = value[0];
	targetValue[1] = value[1];
	targetValue[2] = value[2];
}

void Terrain::normalize()
{
	float max = 0.0f;
	float min = HUGE_VAL;
	
	// Find max and min
	for (int x=0 ; x<m_width ; ++x)
	{
		for (int y=0 ; y<m_height ; ++y)
		{
			float val = heightAt(x, y);
			
			if (val > max)
				max = val;
			if (val < min)
				min = val;
		}
	}
	
	// Normalize
	float diff = max - min;
	
	for (int x=0 ; x<m_width ; ++x)
	{
		for (int y=0 ; y<m_height ; ++y)
		{
			setHeightAt(x, y, (heightAt(x, y) - min) / diff * Settings::instance()->maxHeight());
		}
	}
}

void Terrain::flatten(float power)
{
	for (int x=0 ; x<m_width ; ++x)
		for (int y=0 ; y<m_height ; ++y)
			setHeightAt(x, y, pow(heightAt(x, y) / Settings::instance()->maxHeight(), power) * Settings::instance()->maxHeight());
}

float Terrain::heightAt(int x, int y) const
{
	if (x<0 || y<0 || x>=m_width || y>=m_width)
		return 0.0f;
	
	return m_heightMap[y*m_width + x];
}

float Terrain::heightAt(const vec2& pos) const
{
	float tl = heightAt(int(pos.x), int(pos.y));
	float tr = heightAt(int(pos.x) + 1, int(pos.y));
	float bl = heightAt(int(pos.x), int(pos.y) + 1);
	float br = heightAt(int(pos.x) + 1, int(pos.y) + 1);
	float weightX = (pos.x - float(int(pos.x)));
	float weightY = (pos.y - float(int(pos.y)));

	return interpolate(tl, tr, bl, br,
	                   weightX, weightY,
	                   1.0f - weightX, 1.0f - weightY);
}

vec3 Terrain::normalAt(int x, int y) const
{
	if (x<0 || y<0 || x>=m_width || y>=m_width)
		return vec3(0.0f, 1.0f, 0.0f);
	
	return vec3(m_normalMap + (y*m_width + x)*3);
}

vec3 Terrain::normalAt(const vec2& pos) const
{
	vec3 tl(normalAt(int(pos.x), int(pos.y)));
	vec3 tr(normalAt(int(pos.x) + 1, int(pos.y)));
	vec3 bl(normalAt(int(pos.x), int(pos.y) + 1));
	vec3 br(normalAt(int(pos.x) + 1, int(pos.y) + 1));
	float weightX = (pos.x - float(int(pos.x)));
	float weightY = (pos.y - float(int(pos.y)));
	
	return vec3::interpolate(tl, tr, bl, br, weightX, weightY);
}

void Terrain::drawStrip(int strip)
{
	int size = m_height*2;
	int first = strip * size;
	glDrawArrays(GL_TRIANGLE_STRIP, first, size);
}

void Terrain::drawEdges()
{
	int stripSize = m_height*2;
	int first = stripSize * (m_width-1);
	int horizEdgeElements = m_width*2;
	int vertEdgeElements = m_height*2;
	int size = horizEdgeElements*2 + vertEdgeElements*2;
	glDrawArrays(GL_TRIANGLE_STRIP, first, size);
}

QDataStream& operator <<(QDataStream& stream, const Terrain& terrain)
{
	terrain.save(stream);
	return stream;
}

QDataStream& operator >>(QDataStream& stream, Terrain& terrain)
{
	terrain.load(stream);
	return stream;
}

void Terrain::save(QDataStream& stream) const
{
	stream << m_xTiles;
	stream << m_yTiles;

	/*for (int x=0 ; x<m_xTiles ; ++x)
		for (int y=0 ; y<m_yTiles ; ++y)
			stream << *tile(x, y);*/

	/*stream << m_pools.count();
	foreach (Pool* pool, m_pools)
		stream << *pool;*/
}

void Terrain::load(QDataStream& stream)
{
	quint32 xTiles, yTiles;
	stream >> xTiles >> yTiles;
	init(xTiles, yTiles);

	/*for (int x=0 ; x<m_xTiles ; ++x)
	{
		for (int y=0 ; y<m_yTiles ; ++y)
		{
			stream >> *tile(x, y);
		}
	}*/

	finalize();

	/*int count;
	stream >> count;
	for (int i=0 ; i<count ; ++i)
	{
		Pool* pool = new Pool(this);
		stream >> *pool;
		m_pools << pool;
	}*/
}

void Terrain::setupState()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[1]);
	glNormalPointer(GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[2]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	
	glEnable(GL_BLEND);
	
	s_shader->bind();
	
	s_textures[2]->bind(0, s_rockTexLoc);
	s_textures[3]->bind(1, s_grassTexLoc);
}

void Terrain::setupStateFF()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[0]);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[1]);
	glNormalPointer(GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[2]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glEnable(GL_LIGHTING);
}

void Terrain::setupStateFF(int pass)
{
	s_textures[pass]->bind(0);
	if (pass == 1)
	{
		m_alphaMap->bind(1);

		glClientActiveTexture(GL_TEXTURE1);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferObjects[3]);
		glTexCoordPointer(2, GL_FLOAT, 0, 0);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glEnable(GL_BLEND);
	}
}

void Terrain::tearDownState()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_BLEND);

	s_textures[3]->release();
	s_textures[2]->release();

	s_shader->release();
}

void Terrain::tearDownStateFF()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_BLEND);
	
	m_alphaMap->release();
	s_textures[0]->release();
	
	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_LIGHTING);
}

void Terrain::draw()
{
	setupState();
	for (int x=0 ; x<m_width-1 ; ++x)
		drawStrip(x);
	drawEdges();
	tearDownState();
}

void Terrain::drawWater(Texture* reflection, Texture* refraction, float t, const vec3& eyePos)
{
	m_pools->setupState(reflection, refraction, t, eyePos);
	foreach (Pool* pool, m_pools->pools())
		pool->draw();
	m_pools->tearDownState();
}

void Terrain::drawFF()
{
	setupStateFF();

	setupStateFF(0);
	for (int x=0 ; x<m_width-1 ; ++x)
		drawStrip(x);
	drawEdges();
	
	setupStateFF(1);
	for (int x=0 ; x<m_width-1 ; ++x)
		drawStrip(x);

	tearDownStateFF();
	
}

void Terrain::drawWaterFF()
{
	m_pools->setupStateFF();
	foreach (Pool* pool, m_pools->pools())
		pool->draw();
	m_pools->tearDownStateFF();
}
