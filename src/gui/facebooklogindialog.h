#ifndef FACEBOOKLOGINDIALOG_H
#define FACEBOOKLOGINDIALOG_H

#include "ui_facebooklogindialog.h"
#include "gluimanager.h"

class GameEngine;

class FacebookLoginDialog : public GLUIWidget
{
	Q_OBJECT
public:
	FacebookLoginDialog(GameEngine* gameEngine);
	~FacebookLoginDialog();

	void clearKey();

private slots:
	void loginClicked();

signals:
	void keyEntered(const QString& key);

private:
	Ui_FacebookLoginDialog m_ui;
	GameEngine* m_gameEngine;
};

#endif
