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

#include "modelloader.h"
#include "milkshapemodel.h"
#include "opengl.h"

#include <QDebug>
#include <QCoreApplication>

QMap<QString, Model*> ModelLoader::m_models;

ScaledModel::ScaledModel(Model* delegate, float scale)
	: m_delegate(delegate),
	  m_scale(scale)
{
}

void ScaledModel::draw()
{
	glPushMatrix();
		glScalef(m_scale, m_scale, m_scale);
		m_delegate->draw();
	glPopMatrix();
}

void ScaledModel::drawBoundingBox()
{
	glPushMatrix();
		glScalef(m_scale, m_scale, m_scale);
		m_delegate->drawBoundingBox();
	glPopMatrix();
}

float ScaledModel::boundingSphereRadius()
{
	return m_delegate->boundingSphereRadius() * m_scale;
}


void TestModel::drawBoundingBox()
{
	glDisable(GL_LIGHTING);
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


Model* ModelLoader::loadModel(QString fileName, float scale)
{
	if (!m_models.contains(fileName))
	{
		Model* model;
		if (fileName.endsWith(".ms3d"))
			model = new MilkshapeModel(fileName);
		else if (fileName == "TestModel")
			model = new TestModel();
		else
		{
			qDebug() << "Model format not recognised:" << fileName;
			QCoreApplication::exit(1);
		}
		m_models[fileName] = model;
	}

	return new ScaledModel(m_models[fileName], scale);
}
