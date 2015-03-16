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

#include "gamehud.h"
#include "gameengine.h"
#include "gamestate.h"
#include "worldobject.h"
#include "scripting/ability.h"
#include "actions/action.h"

#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QTextDocument>
#include <QTextBlock>

GameHud::GameHud(GameEngine* parent)
	: GLUIWidget(parent),
	  m_gameEngine(parent),
	  m_selectedObject(NULL)
{
	m_ui.setupUi(this);

	m_abilityButtons.append(m_ui.ability1);
	m_abilityButtons.append(m_ui.ability2);
	m_abilityButtons.append(m_ui.ability3);
	m_abilityButtons.append(m_ui.ability4);
	m_abilityButtons.append(m_ui.ability5);

	foreach (QPushButton* button, m_abilityButtons)
	{
		connect(button, SIGNAL(clicked()), SLOT(abilityClicked()));
	}

	connect(m_ui.chatInput, SIGNAL(returnPressed()), SLOT(chatReturnPressed()));
}

GameHud::~GameHud()
{
}

void GameHud::setSelectedObject(WorldObject* object)
{
	m_selectedObject = object;

	// Set protrait
	QPixmap portrait;
	if (object)
		portrait = object->portrait();
	m_ui.selectedPortrait->setPixmap(portrait);

	// Set ability buttons
	QList<Ability*> abilities = abilityList();
	int i=0;
	foreach (Ability* ability, abilities)
	{
		if (i >= 5)
			break;

		m_abilityButtons[i]->setText(ability->name());
		++i;
	}

	// Hide the rest of the ability buttons
	while (i < 5)
	{
		m_abilityButtons[i]->setText("<hidden>");
		++i;
	}

	recheckAbilityRequirements();
	m_gameEngine->uiManager()->postLayoutRequest(this);
}

void GameHud::drawWidget(QPainter& painter, QWidget* widget)
{
	if (widget == m_ui.selectedInfo && m_selectedObject && m_gameEngine->gameState())
		drawObjectInfo(painter);
	else if (widget == m_ui.playerInfo && m_gameEngine->gameState())
		drawPlayerInfo(painter);
	else if (widget == m_ui.chatFrame)
		drawChatBox(painter);
}

void GameHud::drawObjectInfo(QPainter& painter)
{
	// Setup the fonts
	QFont titleFont(painter.font());
	titleFont.setBold(true);
	QFontMetrics titleMetrics(titleFont);
	QFont normalFont(painter.font());
	QFontMetrics normalMetrics(painter.font());
	QFont smallFont(painter.font());
	smallFont.setPointSize(smallFont.pointSize() - 2);

	QString title = m_selectedObject->name();
	QString player = m_selectedObject->playerName();
	QString action;
	QColor color = m_selectedObject->playerColor();

	if (m_selectedObject->currentAction())
		action = m_selectedObject->currentAction()->name();

	// Setup the rects
	QRect rect(m_ui.selectedInfo->pos(), m_ui.selectedInfo->size());
	QRect titleRect(rect.x(), rect.y(), rect.width(), titleMetrics.height());
	QRect actionRect(rect.x(), titleRect.bottom(), rect.width(), normalMetrics.height());
	QRect actionBarRect(actionRect.x() + 2, actionRect.y() + 1, actionRect.width() - 3, actionRect.height() - 3);
	QRect extraRect(rect.x(), actionRect.bottom(), rect.width(), rect.bottom() - actionRect.bottom());

	// Draw the title
	painter.setFont(titleFont);
	painter.setPen(color);
	painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, title);
	painter.setFont(smallFont);
	painter.drawText(titleRect, Qt::AlignRight | Qt::AlignVCenter, player);

	if (m_selectedObject->currentAction() && m_selectedObject->currentAction()->hasProgressBar())
	{
		actionBarRect.setWidth(int(float(actionBarRect.width()) * m_selectedObject->currentAction()->progress()));

		// Draw the action frame
		painter.setPen(Qt::white);
		painter.setBrush(Qt::black);
		painter.drawRect(actionRect);

		QGradient gradient = QLinearGradient(actionBarRect.topLeft(), actionBarRect.bottomLeft());
		gradient.setColorAt(0, GLUIManager::lightBlue);
		gradient.setColorAt(1, GLUIManager::darkBlue);

		// Draw the action bar
		painter.setPen(Qt::NoPen);
		painter.setBrush(QBrush(gradient));
		painter.drawRect(actionBarRect);

		actionRect.setLeft(actionRect.left() + 5);
	}

	// Draw the action text
	painter.setFont(normalFont);
	painter.setPen(Qt::white);
	painter.drawText(actionRect, Qt::AlignLeft | Qt::AlignVCenter, action);

	// Draw any extra information about the object
	m_selectedObject->drawHudInfo(painter, extraRect);
}

void GameHud::drawPlayerInfo(QPainter& painter)
{
	// Setup the fonts
	QFont normalFont(painter.font());
	QFontMetrics normalMetrics(painter.font());
	
	// Setup the rects
	QRect rect(m_ui.playerInfo->pos(), m_ui.playerInfo->size());
	QRect devotionRect(rect.x(), rect.bottom()-normalMetrics.height(), rect.width(), normalMetrics.height());
	QRect sheepRect(rect.x(), devotionRect.y()-normalMetrics.height(), rect.width(), normalMetrics.height());

	QString devotionCount = "Devotion: " + QString::number(m_gameEngine->gameState()->humanPlayer()->devotion());
	QString sheepCount = "Sheep: " + QString::number(m_gameEngine->gameState()->humanPlayer()->sheepCount());

	// Draw the devotion count
	painter.setOpacity(1.0f);
	painter.setPen(Qt::white);
	painter.setFont(normalFont);
	painter.drawText(devotionRect, Qt::AlignRight | Qt::AlignVCenter, devotionCount);

	// Draw the sheep count
	painter.drawText(sheepRect, Qt::AlignRight | Qt::AlignVCenter, sheepCount);
}

void GameHud::drawChatBox(QPainter& painter)
{
	// Setup the fonts
	QFont normalFont(painter.font());
	QFontMetrics normalMetrics(painter.font());
	
	int yOffset = 0;
	
	for (int i=m_chat.count()-1 ; i>=0 ; --i)
	{
		QTextLayout* layout = m_chat[i].first;
		bool* dirty = &m_chat[i].second;

		// Redo the layout if the text has changed since last time it was drawn
		if (*dirty)
		{
			int leading = normalMetrics.leading();
			int height = 0;
			layout->beginLayout();
			while (1)
			{
				QTextLine line = layout->createLine();
				if (!line.isValid())
					break;

				line.setLineWidth(m_ui.chatFrame->width() - 12);
				height += int(leading);
				line.setPosition(QPoint(0, height));
				height += int(line.height());
			}
			layout->endLayout();
			*dirty = false;
		}

		yOffset += int(layout->boundingRect().height());

		if (yOffset > m_ui.chatFrame->height())
			break;

		layout->draw(&painter, QPointF(m_ui.chatFrame->x() + 6, m_ui.chatFrame->y() + m_ui.chatFrame->height() - yOffset));
	}
}

bool GameHud::widgetAcceptsClicks(QWidget* widget)
{
	if (widget == m_ui.chatFrame || widget == m_ui.playerInfo)
		return false;
	return true;
}

bool GameHud::hoverEvent(QWidget* widget, const QPoint& pos)
{
	if (!widgetAcceptsClicks(widget))
		return false;
	
	int abilityIndex = m_abilityButtons.indexOf((QPushButton*) widget);
	QList<Ability*> abilities = abilityList();
	if (abilityIndex == -1 || abilityIndex >= abilities.count())
		return true;

	QPoint fiddledPos(pos);
	fiddledPos.setX(widget->rect().right());

	m_gameEngine->uiManager()->setTooltip(m_gameEngine->gameState(), abilities[abilityIndex], m_selectedObject, fiddledPos, GLUIManager::TooltipAlignment_Right);
	return true;
}

void GameHud::abilityClicked()
{
	int abilityIndex = m_abilityButtons.indexOf((QPushButton*) sender());
	QList<Ability*> abilities = abilityList();
	if (abilityIndex == -1 || abilityIndex >= abilities.count())
		return;
	Ability* ability = abilities[abilityIndex];

	ability->invoke(m_selectedObject);
}

void GameHud::appendChatLine(const QString& line)
{
	QTextDocument doc;
	doc.setHtml(line);

	QTextLayout* layout = new QTextLayout(doc.toPlainText());
	m_chat.append(QPair<QTextLayout*, bool>(layout, true));

	QList<QTextLayout::FormatRange> formatRanges;
	for (QTextBlock::iterator it = doc.begin().begin(); !it.atEnd(); ++it)
	{
		QTextFragment frag = it.fragment();
		QTextCharFormat cf = frag.charFormat();
		QTextLayout::FormatRange fr;
		fr.format = cf;
		fr.start = frag.position();
		fr.length = frag.length();

		formatRanges << fr;
	}

	layout->setAdditionalFormats(formatRanges);
}

void GameHud::chatReturnPressed()
{
	QString name = "<font color=\"" + m_gameEngine->gameState()->humanPlayer()->color().name() + "\"><b>&lt;" +
	               m_gameEngine->gameState()->humanPlayer()->name() + "&gt;</b></font> ";
	
	appendChatLine(name + m_ui.chatInput->text());
	m_ui.chatInput->setText(QString::null);
	m_gameEngine->uiManager()->clearFocusedWidget();
}

QList<Ability*> GameHud::abilityList()
{
	if (!m_selectedObject)
		return m_gameEngine->gameState()->humanPlayer()->abilities();
	if (m_selectedObject->player() == m_gameEngine->gameState()->humanPlayer())
		return m_selectedObject->abilities();
	return QList<Ability*>();
}

void GameHud::recheckAbilityRequirements()
{
	QList<Ability*> abilities = abilityList();
	
	int i=0;
	foreach (Ability* ability, abilities)
	{
		if (i >= 5)
			break;

		m_abilityButtons[i]->setEnabled(ability->areRequirementsMet(m_gameEngine->gameState(), m_gameEngine->gameState()->humanPlayer(), m_selectedObject));
		++i;
	}
}



