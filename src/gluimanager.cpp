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

#include "gluimanager.h"
#include "smoothvar.h"
#include "worldobject.h"
#include "scripting/ability.h"
#include "gamestate.h"
#include "scripting/requirement.h"
#include "scripting/resourcerequirement.h"

#include <QMetaObject>
#include <QDebug>
#include <QEvent>
#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QStyle>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QFrame>
#include <QStyleOptionButton>
#include <QProgressBar>
#include <QSlider>

const QColor GLUIManager::lightBlue(122, 160, 255);
const QColor GLUIManager::darkBlue(78, 128, 255);

GLUIWidget::GLUIWidget(QWidget* parent, Flags flags)
	: QWidget(parent),
	  m_visible(false),
	  m_flags(flags)
{
	hide();
}

GLUIWidget::~GLUIWidget()
{
}

void GLUIWidget::hideGLUI()
{
	m_visible = false;
	emit(visibilityChanged(m_visible));
}

void GLUIWidget::showGLUI()
{
	m_visible = true;
	emit(visibilityChanged(m_visible));
}



GLUIManager::GLUIManager(QObject* parent)
	: QObject(parent),
	  m_activeHud(NULL),
	  m_mouseHoverWidget(NULL),
	  m_pressedWidget(NULL),
	  m_focusedWidget(NULL)
{
	parent->installEventFilter(this);

	m_style = QApplication::style();
}

GLUIManager::~GLUIManager()
{
}

void GLUIManager::setHud(GLUIWidget* widget)
{
	if (m_activeHud != NULL)
		m_activeHud->hideGLUI();

	if (!m_huds.contains(widget))
	{
		m_huds.append(widget);
		initialiseWidget(widget);
	}

	m_activeHud = widget;
	m_activeHud->showGLUI();
}

void GLUIManager::initialiseWidget(GLUIWidget* widget)
{
	widget->hide();
	updateLayout();
	initialiseWidgetRecursive(widget);
	connect(widget, SIGNAL(visibilityChanged(bool)), SLOT(visibilityChanged(bool)));
	m_opacityMap[widget]->setTargetValue(0.0f);
	m_opacityMap[widget]->updateImmediately();
	m_opacityMap[widget]->setTargetValue(1.0f);
}

void GLUIManager::initialiseWidgetRecursive(QWidget* widget)
{
	connect(widget, SIGNAL(destroyed(QObject*)), SLOT(widgetDestroyed()));
	m_opacityMap[widget] = new SmoothVar<float>();
	m_opacityMap[widget]->setTargetValue(0.2f);
	m_opacityMap[widget]->updateImmediately();

	foreach (QObject* child, widget->children())
	{
		if (!child->inherits("QWidget"))
			continue;

		initialiseWidgetRecursive((QWidget*) child);
	}
}

void GLUIManager::showDialog(GLUIWidget* widget)
{
	if (m_dialogs.contains(widget))
	{
		m_dialogs.removeAll(widget);
		m_dialogs.append(widget);
		widget->showGLUI();
		return;
	}

	m_dialogs.append(widget);
	initialiseWidget(widget);

	widget->showGLUI();
}

void GLUIManager::widgetDestroyed()
{
	QWidget* widget = (QWidget*) sender();
	delete m_opacityMap[widget];
	m_opacityMap.remove(widget);

	if (widget == m_activeHud)
		m_activeHud = NULL;
	if (widget == m_mouseHoverWidget)
		m_mouseHoverWidget = NULL;
	if (m_dialogs.contains((GLUIWidget*) widget))
		m_dialogs.removeAll((GLUIWidget*) widget);
	if (m_huds.contains((GLUIWidget*) widget))
		m_huds.removeAll((GLUIWidget*) widget);
}

void GLUIManager::updateValues(int timeDelta)
{
	foreach (SmoothVar<float>* var, m_opacityMap.values())
	{
		var->update(timeDelta);
	}
	
	m_tooltipOpacity.update(timeDelta);
}

void GLUIManager::updateLayout()
{
	static bool recursionDetection = false;
	if (recursionDetection)
		return;
	recursionDetection = true;
	
	QWidget* parentWidget = (QWidget*) parent();
	
	foreach (GLUIWidget* hud, m_huds)
	{
		QSize size(parentWidget->size());
		size.setWidth(size.width() - 1);
		
		positionWidget(hud, QPoint(0, 0), size);
	}

	foreach (GLUIWidget* widget, m_dialogs)
	{
		QPoint topLeft((parentWidget->width() - widget->sizeHint().width()) / 2,
		               (parentWidget->height() - widget->sizeHint().height()) / 2);

		positionWidget(widget, topLeft, widget->sizeHint());
	}
	
	recursionDetection = false;
}

void GLUIManager::positionWidget(QWidget* widget, const QPoint& pos, const QSize& size)
{
	widget->resize(size);
	widget->move(pos);
	
	// Ugly hack to prepare the widget's layout
	QPixmap p(size);
	widget->render(&p);
}

void GLUIManager::postLayoutRequest(QWidget* widget)
{
	QApplication::postEvent(widget, new QEvent(QEvent::LayoutRequest));
	widget->layout()->update();

	foreach (QObject* object, widget->children())
	{
		if (!object->inherits("QWidget"))
			continue;

		postLayoutRequest((QWidget*) object);
	}
}

QWidget* GLUIManager::widgetAt(const QPoint& point)
{
	QWidget* ret = NULL;

	for (int i=m_dialogs.count()-1 ; i>=0 ; --i)
	{
		if (!m_dialogs[i]->isVisibleGLUI())
			continue;
		
		QRect rect(m_dialogs[i]->x(), m_dialogs[i]->y(), m_dialogs[i]->width(), m_dialogs[i]->height());
		
		if (rect.contains(point))
		{
			QPoint widgetPoint(point - rect.topLeft());
			QWidget* child = m_dialogs[i]->childAt(widgetPoint);
			if (child)
				return child;
			return m_dialogs[i];
		}
	}

	if (m_activeHud)
		ret = m_activeHud->childAt(point);
	if (m_activeHud->widgetAcceptsClicks(ret))
		return ret;
	return NULL;
}

void GLUIManager::updateSliderValue(QSlider* widget, const QPoint& mousePos)
{
	QPoint widgetPos = m_pressedWidget->mapTo((QWidget*)parent(), QPoint(0, 0));
	int value = int(float(mousePos.x() - widgetPos.x()) / float(widget->width()) * float(widget->maximum() - widget->minimum())) + widget->minimum();
	widget->setValue(value);
}

bool GLUIManager::eventFilter(QObject* watched, QEvent* event)
{
	QMouseEvent* mouseEvent = (QMouseEvent*) event;
	
	switch (event->type())
	{
	case QEvent::Resize:
		updateLayout();
		break;
	
	case QEvent::MouseMove:
		return mouseMoveEvent(mouseEvent);
	
	case QEvent::MouseButtonPress:
		m_pressedWidget = widgetAt(mouseEvent->pos());

		if (m_pressedWidget && m_pressedWidget->inherits("QSlider"))
			updateSliderValue((QSlider*) m_pressedWidget, mouseEvent->pos());
		
		return (m_pressedWidget != NULL);

	case QEvent::MouseButtonRelease:
	{
		QWidget* releasedWidget = widgetAt(mouseEvent->pos());
		if (releasedWidget && releasedWidget == m_pressedWidget)
		{
			widgetClicked(m_pressedWidget);
			m_pressedWidget = NULL;
			return true;
		}
		m_pressedWidget = NULL;
		break;
	}

	case QEvent::KeyPress:
		return keyPressEvent((QKeyEvent*) event);
	
	default:
		break;
	}

	return false;
}

bool GLUIManager::mouseMoveEvent(QMouseEvent* event)
{
	QWidget* hover = widgetAt(event->pos());

	clearTooltip();

	if (m_pressedWidget && m_pressedWidget->inherits("QSlider"))
		updateSliderValue((QSlider*) m_pressedWidget, event->pos());

	if (hover == NULL)
	{
		if (m_mouseHoverWidget)
		{
			if (!m_dialogs.contains((GLUIWidget*) m_mouseHoverWidget))
				m_opacityMap[m_mouseHoverWidget]->setTargetValue(0.2f);
			m_mouseHoverWidget = NULL;
		}
		return false;
	}

	bool hit = false;
	foreach (GLUIWidget* hud, m_huds)
	{
		if (m_opacityMap[hud]->targetValue() > 0.01f)
			if (hud->hoverEvent(hover, event->pos()))
				hit = true;
	}

	if (m_mouseHoverWidget && m_mouseHoverWidget != hover && !m_dialogs.contains((GLUIWidget*) m_mouseHoverWidget))
		m_opacityMap[m_mouseHoverWidget]->setTargetValue(0.2f);

	m_mouseHoverWidget = hover;
	if (!m_dialogs.contains((GLUIWidget*) m_mouseHoverWidget) && m_mouseHoverWidget->isEnabled())
		m_opacityMap[m_mouseHoverWidget]->setTargetValue(1.0f);

	return hit;
}

bool GLUIManager::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		if (m_focusedWidget)
		{
			m_focusedWidget = NULL;
			return true;
		}
		
		for (int i=m_dialogs.count()-1 ; i>=0 ; --i)
		{
			if (!m_dialogs[i]->isVisibleGLUI())
				continue;

			m_dialogs[i]->hideGLUI();
			return true;
		}
	}
	
	if (m_focusedWidget)
	{
		QKeyEvent* eventCopy = new QKeyEvent(event->type(), event->key(), event->modifiers(), event->text(), event->isAutoRepeat(), event->count());
		QApplication::postEvent(m_focusedWidget, eventCopy);
		return true;
	}
	return false;
}

void GLUIManager::visibilityChanged(bool visible)
{
	GLUIWidget* senderWidget = (GLUIWidget*) sender();
	
	if (senderWidget == m_activeHud && !visible)
		m_activeHud = NULL;
	
	m_opacityMap[(QWidget*) sender()]->setTargetValue(visible ? 1.0f : 0.0f);
	if (senderWidget->flags() & GLUIWidget::NoFading)
		m_opacityMap[(QWidget*) sender()]->updateImmediately();
}

void GLUIManager::widgetClicked(QWidget* widget)
{
	if (!widget->isEnabled())
		return;
	
	m_focusedWidget = NULL;
	
	if (widget->inherits("QPushButton"))
		((QPushButton*) widget)->click();
	else if (widget->inherits("QCheckBox"))
		((QCheckBox*) widget)->click();
	else if (widget->inherits("QRadioButton"))
		((QRadioButton*) widget)->click();
	else if (widget->inherits("QLineEdit"))
		m_focusedWidget = widget;
}

void GLUIManager::draw(QPainter& painter)
{
	foreach (GLUIWidget* hud, m_huds)
	{
		float baseOpacity = m_opacityMap[hud]->currentValue();
		if (baseOpacity < 0.01f)
			continue;

		m_currentWidget = hud;
		
		painter.save();
		draw(painter, hud, baseOpacity);
		painter.restore();
	}

	foreach (GLUIWidget* dialog, m_dialogs)
	{
		float baseOpacity = m_opacityMap[dialog]->currentValue();
		if (baseOpacity < 0.01f)
			continue;

		m_currentWidget = dialog;
		
		painter.save();
		drawDialog(painter, dialog, baseOpacity);
		painter.restore();

		painter.save();
		draw(painter, dialog, baseOpacity);
		painter.restore();
	}
	
	painter.save();
	drawTooltip(painter);
	painter.restore();
}

void GLUIManager::draw(QPainter& painter, QWidget* widget, float baseOpacity)
{
	if (widget->metaObject() == NULL)
		return;
	
	if (widget->inherits("QPushButton"))
		drawButton(painter, (QPushButton*) widget, baseOpacity);
	else if (widget->inherits("QLabel"))
		drawLabel(painter, (QLabel*) widget, baseOpacity);
	else if (widget->inherits("QCheckBox"))
		drawCheckBox(painter, (QCheckBox*) widget, baseOpacity);
	else if (widget->inherits("QRadioButton"))
		drawRadioButton(painter, (QRadioButton*) widget, baseOpacity);
	else if (widget->inherits("QLineEdit"))
		drawLineEdit(painter, (QLineEdit*) widget, baseOpacity);
	else if (widget->inherits("QProgressBar"))
		drawProgressBar(painter, (QProgressBar*) widget, baseOpacity);
	else if (widget->inherits("QSlider"))
		drawSlider(painter, (QSlider*) widget, baseOpacity);
	else if (widget->inherits("QFrame"))
		drawFrame(painter, (QFrame*) widget, baseOpacity);
	
	foreach (QObject* child, widget->children())
	{
		if (!child->inherits("QWidget"))
			continue;

		QPoint meep = widget->pos();
		QPoint meep2 = ((QWidget*)child)->pos();
		painter.save();
		painter.translate(widget->pos());
		draw(painter, (QWidget*) child, baseOpacity);
		painter.restore();
	}
}

void GLUIManager::drawButton(QPainter& painter, QPushButton* widget, float baseOpacity)
{
	QRect rect(widget->pos(), widget->size());

	if (widget->text() == "<hidden>")
		return;

	QGradient gradient = QLinearGradient(rect.topLeft(), rect.bottomLeft());
	gradient.setColorAt(widget->isChecked() ? 1 : 0, lightBlue);
	gradient.setColorAt(widget->isChecked() ? 0 : 1, darkBlue);

	if (widget->isEnabled())
		painter.setPen(QPen(Qt::white));
	else
		painter.setPen(QPen(Qt::darkGray));

	painter.setOpacity((widget->isChecked() ? 1.0f : m_opacityMap[widget]->currentValue()) * baseOpacity);
	painter.setBrush(QBrush(gradient));
	painter.drawRect(rect);

	painter.setOpacity(1.0 * baseOpacity);
	painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, widget->text());
}

void GLUIManager::drawDialog(QPainter& painter, GLUIWidget* widget, float baseOpacity)
{
	QRect rect(QPoint(widget->x(), widget->y() - 22), QSize(widget->width(), widget->height() + 22));
	QRect titleRect(QPoint(widget->x(), widget->y() - 22), QSize(widget->width(), 22));

	painter.setOpacity(baseOpacity * 0.5);
	painter.setBrush(QColor(64, 64, 64));
	painter.drawRect(rect);
	painter.drawLine(titleRect.bottomLeft(), titleRect.bottomRight());

	QFont titleFont(painter.font());
	titleFont.setBold(true);
	painter.setOpacity(baseOpacity);
	painter.setFont(titleFont);
	painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, widget->windowTitle());
}

void GLUIManager::drawLabel(QPainter& painter, QLabel* widget, float baseOpacity)
{
	QRect rect(widget->pos(), widget->size());
	
	painter.setOpacity(1.0 * baseOpacity);

	if (widget->text() == "<glui>")
		m_currentWidget->drawWidget(painter, widget);
	else if (widget->pixmap() && !widget->pixmap()->isNull())
		painter.drawPixmap(rect, *widget->pixmap());
	else if (!widget->text().isEmpty())
		painter.drawText(rect, widget->alignment(), widget->text());
	else if (widget->frameShape() != QFrame::NoFrame)
		drawFrame(painter, widget, baseOpacity);
}

void GLUIManager::drawCheckBox(QPainter& painter, QCheckBox* widget, float baseOpacity)
{
	int indicatorWidth = m_style->pixelMetric(QStyle::PM_IndicatorWidth) + 5;
	QRect rect(widget->pos(), widget->size());
	QRect labelRect(rect.x() + indicatorWidth, rect.y(), rect.width() - indicatorWidth, rect.height());

	// Draw label
	painter.setOpacity(1.0f * baseOpacity);
	painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, widget->text());

	// Setup options
	QStyleOptionButton styleOption;
	styleOption.state = widget->isChecked() ? QStyle::State_On : QStyle::State_Off;

	// Draw checkbox
	painter.setOpacity(m_opacityMap[widget]->currentValue() * baseOpacity);
	painter.translate(QPoint(rect.x(), rect.y() + rect.height()/2));
	m_style->drawControl(QStyle::CE_CheckBox, &styleOption, &painter, widget);
}

void GLUIManager::drawRadioButton(QPainter& painter, QRadioButton* widget, float baseOpacity)
{
	int indicatorWidth = m_style->pixelMetric(QStyle::PM_IndicatorWidth) + 5;
	QRect rect(widget->pos(), widget->size());
	QRect labelRect(rect.x() + indicatorWidth, rect.y(), rect.width() - indicatorWidth, rect.height());

	// Draw label
	painter.setOpacity(1.0f * baseOpacity);
	painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, widget->text());

	// Setup options
	QStyleOptionButton styleOption;
	styleOption.state = widget->isChecked() ? QStyle::State_On : QStyle::State_Off;

	// Draw radio button
	painter.setOpacity(m_opacityMap[widget]->currentValue() * baseOpacity);
	painter.translate(QPoint(rect.x(), rect.y() + rect.height()/2));
	m_style->drawControl(QStyle::CE_RadioButton, &styleOption, &painter, widget);
}

void GLUIManager::drawLineEdit(QPainter& painter, QLineEdit* widget, float baseOpacity)
{
	QRect rect(widget->pos(), widget->size());
	QRect contentsRect(rect.x() + 4, rect.y() + 2, rect.width() - 8, rect.height() - 4);
	float opacity = (m_focusedWidget == widget) ? 1.0f : m_opacityMap[widget]->currentValue();
	QString contents = widget->displayText();
	QFontMetrics metrics = painter.fontMetrics();

	// Draw frame
	painter.setOpacity(opacity * baseOpacity);
	painter.setBrush(Qt::black);
	painter.drawRect(rect);

	// Draw selection frame if any
	if (widget->hasSelectedText())
	{
		QRect selectionRect(contentsRect);
		selectionRect.setLeft(selectionRect.x() + metrics.width(contents.left(widget->selectionStart())));
		selectionRect.setWidth(qMin(contentsRect.width(), metrics.width(widget->selectedText())));
		
		painter.setBrush(lightBlue);
		painter.setPen(lightBlue);
		painter.drawRect(selectionRect);
	}

	painter.setOpacity(baseOpacity);
	painter.setPen(Qt::white);

	// Draw cursor if we have focus
	if (m_focusedWidget == widget)
	{
		QPoint p1(contentsRect.x() + metrics.width(contents.left(widget->cursorPosition())), contentsRect.top());
		QPoint p2(contentsRect.x() + metrics.width(contents.left(widget->cursorPosition())), contentsRect.bottom());

		painter.setPen(Qt::white);
		painter.drawLine(p1, p2);
	}

	// Draw contents
	painter.drawText(contentsRect, Qt::AlignLeft | Qt::AlignVCenter, contents);
}

void GLUIManager::drawProgressBar(QPainter& painter, QProgressBar* widget, float baseOpacity)
{
	QRect rect(widget->pos(), widget->size());
	QRect barRect(rect.x()+2, rect.y()+1, int(float(rect.width()-4) / float(widget->maximum()) * float(widget->value())), rect.height()-3);

	QGradient gradient = QLinearGradient(rect.topLeft(), rect.bottomLeft());
	gradient.setColorAt(0, lightBlue);
	gradient.setColorAt(1, darkBlue);

	painter.setOpacity(baseOpacity);

	// Draw frame
	painter.setPen(Qt::white);
	painter.setBrush(Qt::black);
	painter.drawRect(rect);

	// Draw bar
	painter.setBrush(QBrush(gradient));
	painter.setPen(Qt::NoPen);
	painter.drawRect(barRect);

	// Draw label
	painter.setPen(Qt::white);
	painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, widget->text());
}

void GLUIManager::drawFrame(QPainter& painter, QFrame* widget, float baseOpacity)
{
	QRect rect(widget->pos(), widget->size());

	painter.setOpacity(0.2f * baseOpacity);
	painter.setBrush(Qt::black);
	painter.drawRect(rect);
}

void GLUIManager::drawSlider(QPainter& painter, QSlider* widget, float baseOpacity)
{
	QRect rect(widget->pos(), widget->size());
	QRect barRect(rect.x(), rect.y()+rect.height()/2-2, rect.width(), 4);
	int handleLoc = int(float(rect.width()-6) / float(widget->maximum() - widget->minimum()) * float(widget->value() - widget->minimum()));
	QRect handleRect(rect.x() + handleLoc, rect.y(), 6, rect.height());

	QGradient gradient = QLinearGradient(rect.topLeft(), rect.bottomLeft());
	gradient.setColorAt(0, lightBlue);
	gradient.setColorAt(1, darkBlue);

	// Draw bar
	painter.setOpacity(0.2f * baseOpacity);
	painter.setPen(Qt::white);
	painter.setBrush(Qt::black);
	painter.drawRect(barRect);

	// Draw handle
	painter.setOpacity(m_opacityMap[widget]->currentValue() * baseOpacity);
	painter.setBrush(QBrush(gradient));
	painter.setPen(Qt::white);
	painter.drawRect(handleRect);
}

void GLUIManager::drawTooltip(QPainter& painter)
{
	if (m_tooltipOpacity < 0.01f)
		return;
	
	QFont titleFont(painter.font());
	titleFont.setBold(true);
	QFontMetrics titleMetrics(titleFont);
	QFont smallFont(painter.font());
	smallFont.setPointSize(smallFont.pointSize() - 2);
	QFontMetrics smallMetrics(smallFont);
	QFont normalFont(painter.font());
	QFontMetrics normalMetrics(painter.fontMetrics());

	if (m_tooltipAbility)
	{
		int h = titleMetrics.height();
		int w = titleMetrics.width(m_tooltipAbility->name()) + 25 +
		        smallMetrics.width(QString::number(m_tooltipAbility->cost()) + " devotion");
		foreach (Requirement* requirement, m_tooltipAbility->requirements())
		{
			if (qobject_cast<ResourceRequirement*>(requirement))
				continue;
			w = qMax(w, normalMetrics.width(requirement->description()));
			h += normalMetrics.height();
		}
		w = qMax(w, 150);

		QString description = m_tooltipAbility->description().replace("<player>", const_cast<GameState*>(m_tooltipGameState)->humanPlayer()->name());
		if (description.length() > 0 && m_tooltipTextLayout.lineCount() <= 0)
		{
			// Rebuild layout
			m_tooltipTextLayout.setText("\"" + description + "\"");
			m_tooltipTextLayout.beginLayout();
			m_tooltipTextLayout.setFont(smallFont);

			int leading = smallMetrics.leading();
			int height = 0;
			while (true)
			{
				QTextLine line = m_tooltipTextLayout.createLine();
				if (!line.isValid())
					break;

				line.setLineWidth(w);
				height += leading;
				line.setPosition(QPoint(0, height));
				height += int(line.height());
			}
		}

		if (description.length() > 0)
			h += int(m_tooltipTextLayout.boundingRect().height());
		h += 4;
		w += 8;

		int x, y;
		switch (m_tooltipAlignment)
		{
		case TooltipAlignment_Above:
			x = qBound(0, m_tooltipPos.x() - w/2, painter.device()->width() - w);
			y = qBound(0, m_tooltipPos.y() - h - 5, painter.device()->height() - h);
			break;

		case TooltipAlignment_Right:
			x = qBound(0, m_tooltipPos.x() + 5, painter.device()->width() - w);
			y = qBound(0, m_tooltipPos.y() - h/2, painter.device()->height() - h);
			break;

		case TooltipAlignment_Left:
			x = qBound(0, m_tooltipPos.x() - w - 5, painter.device()->width() - w);
			y = qBound(0, m_tooltipPos.y() - h/2, painter.device()->height() - h);
			break;

		case TooltipAlignment_Below:
			x = qBound(0, m_tooltipPos.x() - w/2, painter.device()->width() - w);
			y = qBound(0, m_tooltipPos.y() + 5, painter.device()->height() - h);
			break;
		}
		
		QRect tooltipRect(x, y, w, h);
		QRect titleRect(x+4, y+2, w-8, titleMetrics.height());
		QRect requirementRect(x+4, y+2+titleRect.height(), w-8, normalMetrics.height());

		// Draw outline
		painter.setOpacity(0.5f * m_tooltipOpacity);
		painter.setBrush(Qt::black);
		painter.drawRect(tooltipRect);

		// Draw title text
		painter.setOpacity(m_tooltipOpacity);
		painter.setPen(m_tooltipColor);
		painter.setFont(titleFont);
		painter.drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, m_tooltipAbility->name());

		// Draw devotion cost
		if (m_tooltipGameState->humanPlayer()->devotion() >= m_tooltipAbility->cost())
			painter.setPen(Qt::white);
		else
			painter.setPen(QColor(255, 128, 128));
		painter.setFont(smallFont);
		painter.drawText(titleRect, Qt::AlignRight | Qt::AlignVCenter, QString::number(m_tooltipAbility->cost()) + " devotion");

		// Draw requirements
		painter.setFont(normalFont);
		painter.setPen(Qt::white);
		foreach (Requirement* requirement, m_tooltipAbility->requirements())
		{
			if (qobject_cast<ResourceRequirement*>(requirement))
				continue;
			
			if (requirement->isMet(m_tooltipGameState, m_tooltipGameState->humanPlayer(), m_tooltipObject))
				painter.setPen(Qt::white);
			else
				painter.setPen(QColor(255, 128, 128));
			
			painter.drawText(requirementRect, Qt::AlignLeft | Qt::AlignVCenter, requirement->description());
			requirementRect.moveTop(requirementRect.top() + requirementRect.height());
		}

		// Draw description
		if (description.length() > 0)
		{
			painter.setPen(Qt::yellow);
			m_tooltipTextLayout.draw(&painter, requirementRect.topLeft());
		}
	}
	else
	{
		int descriptionHeight = m_tooltipDescription.isEmpty() ? 0 : normalMetrics.height();
		int w = qMax(titleMetrics.width(m_tooltipTitle), normalMetrics.width(m_tooltipDescription)) + 8;
		int h = titleMetrics.height() + descriptionHeight + 4;
		int x = qBound(0, m_tooltipPos.x() - w/2, painter.device()->width() - w);
		int y = qBound(0, m_tooltipPos.y() - h - 5, painter.device()->height() - h);
		
		QRect tooltipRect(x, y, w, h);
		QRect titleRect(x+4, y+2, w-8, titleMetrics.height());
		QRect descriptionRect(x+4, y+2+titleMetrics.height(), w-8, normalMetrics.height());

		// Draw outline
		painter.setOpacity(0.5f * m_tooltipOpacity);
		painter.setBrush(Qt::black);
		painter.drawRect(tooltipRect);

		// Draw description text
		painter.setOpacity(1.0f * m_tooltipOpacity);
		if (!m_tooltipDescription.isEmpty())
			painter.drawText(descriptionRect, Qt::AlignLeft | Qt::AlignVCenter, m_tooltipDescription);

		// Draw title text
		painter.setFont(titleFont);
		painter.setPen(m_tooltipColor);
		painter.drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter, m_tooltipTitle);
	}
}

void GLUIManager::setTooltip(const QString& title, const QString& description, const QPoint& pos, const QColor& color, TooltipAlignment align)
{
	m_tooltipTitle = title;
	m_tooltipDescription = description;
	m_tooltipPos = pos;
	m_tooltipColor = color;
	m_tooltipAbility = NULL;
	m_tooltipOpacity = 1.0f;
	m_tooltipAlignment = align;
}

void GLUIManager::setTooltip(const GameState* gameState, const Ability* ability, const WorldObject* object, const QPoint& pos, TooltipAlignment align)
{
	m_tooltipAbility = ability;
	m_tooltipGameState = gameState;
	m_tooltipPos = pos;
	m_tooltipObject = object;
	m_tooltipOpacity = 1.0f;
	m_tooltipColor = Qt::white;
	m_tooltipTextLayout.setText(QString::null);
	m_tooltipAlignment = align;
}

void GLUIManager::clearTooltip()
{
	m_tooltipOpacity = 0.0f;
}




