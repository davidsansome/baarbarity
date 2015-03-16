#ifndef POOLS_H
#define POOLS_H

#include <QDataStream>

#include "vector.h"

class Terrain;
class Pools;
class Texture;
class Shader;

class Pool : public QObject
{
	Q_OBJECT
public:
	Pool(Pools* pools, uint x, uint y, uint width, uint height);
	Pool(Pools* pools);
	~Pool();

	void draw();
	
	int x() { return m_x; }
	int y() { return m_y; }
	int width() { return m_width; }
	int height() { return m_height; }
	vec2 center() { return m_center; }
	vec2 origin() { return m_origin; }

	void save(QDataStream& stream) const;
	void load(QDataStream& stream);

private slots:
	void invalidateDisplayList();

private:
	void init(uint x, uint y, uint width, uint height);
	void draw(int tess);
	
	bool m_displayListDirty;
	uint m_displayList;

	Pools* m_pools;
	quint32 m_x;
	quint32 m_y;
	quint32 m_width;
	quint32 m_height;
	vec2 m_center;
	vec2 m_origin;
};

QDataStream& operator <<(QDataStream& stream, const Pool& terrain);
QDataStream& operator >>(QDataStream& stream, Pool& terrain);

class Pools
{
public:
	Pools(Terrain* terrain);
	~Pools();

	void addPool(Pool* pool) { m_pools << pool; }
	QList<Pool*> pools() { return m_pools; }

	Terrain* terrain() { return m_terrain; }

	void setupState(Texture* reflection, Texture* refraction, float t, const vec3& eyePos);
	void setupStateFF();

	void tearDownState();
	void tearDownStateFF();

private:
	QList<Pool*> m_pools;
	Terrain* m_terrain;

	Texture* m_reflection;
	Texture* m_refraction;
	
	static Texture* s_waterDisplacementMaps;
	static Shader* s_shader;
	static int s_reflectionLoc;
	static int s_refractionLoc;
	static int s_displacementMapLoc;
	static int s_tLoc;
	static int s_eyePosLoc;
};

#endif
