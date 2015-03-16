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

#ifndef MILKSHAPEMODEL_H
#define MILKSHAPEMODEL_H

#include "model.h"

#include <QList>

struct MS3DVertex
{
	quint8 flags;
	float vertex[3];
	qint8 boneId;
	quint8 referenceCount;
};

struct MS3DTriangle
{
	quint16 flags;
	quint16 vertexIndices[3];
	float vertexNormals[3][3];
	float s[3];
	float t[3];
	quint8 smoothingGroup;
	quint8 groupIndex;
};

class MilkshapeModel : public Model
{
public:
	MilkshapeModel(const QString& fileName);

	void draw();
	void drawBoundingBox();
	float boundingSphereRadius() { return m_boundingSphereRadius; }

private:
	void renderTriangle(int i);

private:
	QList<MS3DVertex> m_vertices;
	QList<MS3DTriangle> m_triangles;
	
	bool m_dirty;
	uint m_displayList;
	bool m_boundingBoxDirty;
	uint m_boundingBoxDisplayList;

	float m_limits[3][2];
	float m_boundingSphereRadius;
};

#endif
