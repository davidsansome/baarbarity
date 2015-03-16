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

#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "model.h"

#include <QMap>

class ScaledModel : public Model
{
public:
	ScaledModel(Model* delegate, float scale);
	
	void draw();
	void drawBoundingBox();
	float boundingSphereRadius();

private:
	Model* m_delegate;
	float m_scale;
};

class TestModel : public Model
{
public:
	TestModel() {}
	
	void draw() { drawBoundingBox(); }
	void drawBoundingBox();
	float boundingSphereRadius() { return 2.0f; }
};

class ModelLoader
{
public:
	static Model* loadModel(QString fileName, float scale);

private:
	static QMap<QString, Model*> m_models;
};

#endif
