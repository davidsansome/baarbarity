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

#ifndef TERRAIN_H
#define TERRAIN_H

#define TILE_SIZE 4
#define TILE_SIZE_PLUS_1 (TILE_SIZE+1)

#define ALPHA_MAP_SIZE_MUL 2

#include "vector.h"

#include <QObject>

class Pools;
class Texture;
class Shader;

class Terrain : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int width READ width)
	Q_PROPERTY(int height READ height)
	
public:
	Terrain(int xTiles, int yTiles);
	Terrain();
	~Terrain();

	void draw();
	void drawWater(Texture* reflection, Texture* refraction, float t, const vec3& eyePos);
	
	void drawFF();
	void drawWaterFF();

	void generate(int iterations, int minDistance, int maxDistance, float flattenFactor);

	int width() { return m_width; }
	int height() { return m_height; }
	int xTiles() { return m_xTiles; }
	int yTiles() { return m_yTiles; }

	float heightAt(int x, int y) const;
	vec3 normalAt(int x, int y) const;
	Q_INVOKABLE float heightAt(const vec2& pos) const;
	Q_INVOKABLE vec3 normalAt(const vec2& pos) const;

	void save(QDataStream& stream) const;
	void load(QDataStream& stream);

private:
	void init(int xTiles, int yTiles);
	
	void setHeightAt(int x, int y, float value);
	void setNormalAt(int x, int y, float* value);
	
	void calculateVertexNormal(int x, int y);
	void calculatePlaneNormal(int x, int y, int x1, int y1, int x2, int y2, float* result);

	void normalize();
	void flatten(float power);
	void generateWater();
	bool makePool(int centerX, int centerY, int radius);
	void finalize();
	void addVertexToBuffer(float* v, float* n, float* t, float* a, int x, int y);
	void finalizeHorizEdge(float** v, float** n, float** t, float** a, int y, float normalDirection, int incDirection);
	void finalizeVertEdge(float** v, float** n, float** t, float** a, int x, float normalDirection, int incDirection);
	
	void setupState();
	void tearDownState();
	void setupStateFF();
	void setupStateFF(int pass);
	void tearDownStateFF();
	
	void drawStrip(int strip);
	void drawEdges();

private:
	quint32 m_xTiles;
	quint32 m_yTiles;
	int m_width;
	int m_height;
	Pools* m_pools;

	float* m_heightMap;
	float* m_normalMap;
	uint m_bufferObjects[4];
	
	Texture* m_alphaMap;
	
	static Texture* s_textures[4];
	static Shader* s_shader;
	static int s_rockTexLoc;
	static int s_grassTexLoc;
};
Q_DECLARE_METATYPE(Terrain*)

QDataStream& operator <<(QDataStream& stream, const Terrain& terrain);
QDataStream& operator >>(QDataStream& stream, Terrain& terrain);

#endif
