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

#include "milkshapemodel.h"

#include <QFile>
#include <QDataStream>
#include <QDebug>

#include <math.h>
#ifdef Q_OS_DARWIN
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

MilkshapeModel::MilkshapeModel(const QString& fileName)
	: Model(),
	  m_dirty(true),
	  m_boundingBoxDirty(true)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	QDataStream stream(&file);
	stream.setByteOrder(QDataStream::LittleEndian);
	
	// Skip header
	stream.skipRawData(14);
	
	// Read vertices
	quint16 vertexCount;
	stream >> vertexCount;
	for (int i=0 ; i<vertexCount ; ++i)
	{
		MS3DVertex vertex;
		stream >> vertex.flags;
		stream >> vertex.vertex[0] >> vertex.vertex[1] >> vertex.vertex[2];
		stream >> vertex.boneId;
		stream >> vertex.referenceCount;
		
		m_vertices << vertex;
	}
	
	// Read triangles
	unsigned short triangleCount;
	stream >> triangleCount;
	for (int i=0 ; i<triangleCount ; ++i)
	{
		MS3DTriangle triangle;
		stream >> triangle.flags;
		stream >> triangle.vertexIndices[0] >> triangle.vertexIndices[1] >> triangle.vertexIndices[2];
		stream >> triangle.vertexNormals[0][0] >> triangle.vertexNormals[0][1] >> triangle.vertexNormals[0][2];
		stream >> triangle.vertexNormals[1][0] >> triangle.vertexNormals[1][1] >> triangle.vertexNormals[1][2];
		stream >> triangle.vertexNormals[2][0] >> triangle.vertexNormals[2][1] >> triangle.vertexNormals[2][2];
		stream >> triangle.s[0] >> triangle.s[1] >> triangle.s[2];
		stream >> triangle.t[0] >> triangle.t[1] >> triangle.t[2];
		stream >> triangle.smoothingGroup;
		stream >> triangle.groupIndex;
		
		m_triangles << triangle;
	}

	// Calculate bounds
	m_limits[0][0] = HUGE_VAL;
	m_limits[0][1] = -HUGE_VAL;
	m_limits[1][0] = HUGE_VAL;
	m_limits[1][1] = -HUGE_VAL;
	m_limits[2][0] = HUGE_VAL;
	m_limits[2][1] = -HUGE_VAL;
	m_boundingSphereRadius = 0.0f;
	for (int i=0 ; i<m_vertices.count() ; ++i)
	{
		float radialDistance = 0.0f;
		for (int v=0 ; v<3 ; ++v)
		{
			if (m_vertices[i].vertex[v] < m_limits[v][0])
				m_limits[v][0] = m_vertices[i].vertex[v];
			if (m_vertices[i].vertex[v] > m_limits[v][1])
				m_limits[v][1] = m_vertices[i].vertex[v];

			radialDistance += m_vertices[i].vertex[v] * m_vertices[i].vertex[v];
		}
		radialDistance = sqrt(radialDistance);
		if (radialDistance > m_boundingSphereRadius)
			m_boundingSphereRadius = radialDistance;
	}
}

void MilkshapeModel::renderTriangle(int i)
{
	MS3DTriangle triangle = m_triangles[i];
	
	for (int v=0 ; v<3 ; ++v)
	{
		MS3DVertex vertex = m_vertices[triangle.vertexIndices[v]];
		
		glNormal3f(triangle.vertexNormals[v][0], triangle.vertexNormals[v][1], triangle.vertexNormals[v][2]);
		//glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(vertex.vertex[0], vertex.vertex[1], vertex.vertex[2]);
	}
}

void MilkshapeModel::draw()
{
	if (!m_dirty)
		glCallList(m_displayList);
	else
	{
		m_displayList = glGenLists(1);
		m_dirty = false;

		glNewList(m_displayList, GL_COMPILE_AND_EXECUTE);
			glBegin(GL_TRIANGLES);
				for (int i=0 ; i<m_triangles.count() ; ++i)
					renderTriangle(i);
			glEnd();
		glEndList();
	}
}

void MilkshapeModel::drawBoundingBox()
{
	glPushMatrix();
		if (!m_boundingBoxDirty)
			glCallList(m_boundingBoxDisplayList);
		else
		{
			m_boundingBoxDisplayList = glGenLists(1);
			m_boundingBoxDirty = false;
				
			glNewList(m_boundingBoxDisplayList, GL_COMPILE_AND_EXECUTE);
				glBegin(GL_QUAD_STRIP);
					glVertex3f(m_limits[0][1], m_limits[1][1], m_limits[2][1]);
					glVertex3f(m_limits[0][0], m_limits[1][1], m_limits[2][1]);
					glVertex3f(m_limits[0][1], m_limits[1][1], m_limits[2][0]);
					glVertex3f(m_limits[0][0], m_limits[1][1], m_limits[2][0]);
					glVertex3f(m_limits[0][1], m_limits[1][0], m_limits[2][0]);
					glVertex3f(m_limits[0][0], m_limits[1][0], m_limits[2][0]);
					glVertex3f(m_limits[0][1], m_limits[1][0], m_limits[2][1]);
					glVertex3f(m_limits[0][0], m_limits[1][0], m_limits[2][1]);
					glVertex3f(m_limits[0][1], m_limits[1][1], m_limits[2][1]);
					glVertex3f(m_limits[0][0], m_limits[1][1], m_limits[2][1]);
				glEnd();
				glBegin(GL_QUADS);
					glVertex3f(m_limits[0][0], m_limits[1][1], m_limits[2][1]);
					glVertex3f(m_limits[0][0], m_limits[1][1], m_limits[2][0]);
					glVertex3f(m_limits[0][0], m_limits[1][0], m_limits[2][0]);
					glVertex3f(m_limits[0][0], m_limits[1][0], m_limits[2][1]);
					glVertex3f(m_limits[0][1], m_limits[1][1], m_limits[2][1]);
					glVertex3f(m_limits[0][1], m_limits[1][1], m_limits[2][0]);
					glVertex3f(m_limits[0][1], m_limits[1][0], m_limits[2][0]);
					glVertex3f(m_limits[0][1], m_limits[1][0], m_limits[2][1]);
				glEnd();
			glEndList();
		}
	glPopMatrix();
}


