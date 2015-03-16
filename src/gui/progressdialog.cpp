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

#include "progressdialog.h"
#include "gameengine.h"

#include <QProgressBar>
#include <QCoreApplication>

ProgressDialog::ProgressDialog(QWidget* parent)
	: GLUIWidget(parent)
{
	m_ui.setupUi(this);
}

ProgressDialog::~ProgressDialog()
{
}

void ProgressDialog::setProgress(int value, int max)
{
	m_ui.progressBar->setMaximum(max);
	m_ui.progressBar->setValue(value);
	QCoreApplication::processEvents();
}

