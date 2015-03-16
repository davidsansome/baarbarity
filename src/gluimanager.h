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

#ifndef GLUIMANAGER_H
#define GLUIMANAGER_H

#include "smoothvar.h"

#include <QPainter>
#include <QWidget>
#include <QMap>
#include <QTextLayout>

class Ability;
class WorldObject;
class GameState;

class QPushButton;
class QMouseEvent;
class QKeyEvent;
class QLabel;
class QStyle;
class QCheckBox;
class QRadioButton;
class QLineEdit;
class QFrame;
class QSlider;
class QProgressBar;

class GLUIWidget : public QWidget
{
	Q_OBJECT

public:
	enum Flags
	{
		NoFlags = 0,
		NoFading = 1
	};
	
	GLUIWidget(QWidget* parent, Flags flags = NoFlags);
	virtual ~GLUIWidget();

	bool isVisibleGLUI() { return m_visible; }
	Flags flags() { return m_flags; }

	virtual void drawWidget(QPainter& painter, QWidget* widget) {}
	virtual bool hoverEvent(QWidget* widget, const QPoint& pos) { return true; }
	virtual bool widgetAcceptsClicks(QWidget* widget) { return true; }

public slots:
	void showGLUI();
	void hideGLUI();

signals:
	void visibilityChanged(bool visible);

private:
	bool m_visible;
	Flags m_flags;
};


class GLUIManager : public QObject
{
	Q_OBJECT
	
public:
	enum TooltipAlignment
	{
		TooltipAlignment_Above,
		TooltipAlignment_Right,
		TooltipAlignment_Below,
		TooltipAlignment_Left
	};
	
	GLUIManager(QObject* parent);
	~GLUIManager();

	void setHud(GLUIWidget* widget);
	void showDialog(GLUIWidget* widget);

	void updateValues(int timeDelta);
	void draw(QPainter& painter);
	
	void setTooltip(const QString& title, const QString& description, const QPoint& pos, const QColor& color, TooltipAlignment align = TooltipAlignment_Above);
	void setTooltip(const GameState* gameState, const Ability* ability, const WorldObject* object, const QPoint& pos, TooltipAlignment align = TooltipAlignment_Above);
	void clearTooltip();

	void clearFocusedWidget() { m_focusedWidget = NULL; }

	void postLayoutRequest(QWidget* widget);

	static const QColor lightBlue;
	static const QColor darkBlue;

private slots:
	void widgetDestroyed();
	void visibilityChanged(bool visible);

private:
	bool eventFilter(QObject* watched, QEvent* event);
	bool mouseMoveEvent(QMouseEvent* event);
	bool keyPressEvent(QKeyEvent* event);
	void widgetClicked(QWidget* widget);

	void updateSliderValue(QSlider* widget, const QPoint& mousePos);
	
	void initialiseWidget(GLUIWidget* widget);
	void initialiseWidgetRecursive(QWidget* widget);
	void updateLayout();
	void positionWidget(QWidget* widget, const QPoint& pos, const QSize& size);

	QWidget* widgetAt(const QPoint& point);
	
	void draw(QPainter& painter, QWidget* widget, float baseOpacity);
	void drawButton(QPainter& painter, QPushButton* widget, float baseOpacity);
	void drawDialog(QPainter& painter, GLUIWidget* widget, float baseOpacity);
	void drawLabel(QPainter& painter, QLabel* widget, float baseOpacity);
	void drawCheckBox(QPainter& painter, QCheckBox* widget, float baseOpacity);
	void drawRadioButton(QPainter& painter, QRadioButton* widget, float baseOpacity);
	void drawLineEdit(QPainter& painter, QLineEdit* widget, float baseOpacity);
	void drawFrame(QPainter& painter, QFrame* widget, float baseOpacity);
	void drawProgressBar(QPainter& painter, QProgressBar* widget, float baseOpacity);
	void drawSlider(QPainter& painter, QSlider* widget, float baseOpacity);
	void drawTooltip(QPainter& painter);

	QList<GLUIWidget*> m_huds;
	GLUIWidget* m_activeHud;
	QList<GLUIWidget*> m_dialogs;
	GLUIWidget* m_currentWidget;

	QWidget* m_mouseHoverWidget;
	QWidget* m_pressedWidget;
	QWidget* m_focusedWidget;
	QMap<QWidget*, SmoothVar<float>*> m_opacityMap;

	const Ability* m_tooltipAbility;
	const WorldObject* m_tooltipObject;
	const GameState* m_tooltipGameState;
	QTextLayout m_tooltipTextLayout;
	QString m_tooltipTitle;
	QString m_tooltipDescription;
	QPoint m_tooltipPos;
	TooltipAlignment m_tooltipAlignment;
	QColor m_tooltipColor;
	SmoothVar<float> m_tooltipOpacity;

	QStyle* m_style;
};

#endif
