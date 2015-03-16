#ifndef PLAYEROPTIONSDIALOG_H
#define PLAYEROPTIONSDIALOG_H

#include "ui_playeroptionsdialog.h"
#include "facebooklogindialog.h"
#include "gluimanager.h"

#include <QWidget>

class GameEngine;
class FacebookRequest;

class PlayerOptionsDialog : public GLUIWidget
{
	Q_OBJECT
public:
	PlayerOptionsDialog(GameEngine* gameEngine);
	~PlayerOptionsDialog();

private slots:
	void updateSettings();
	void facebookButtonClicked();
	void keyEntered(const QString& key);
	void getSessionResponse(bool error);
	void getNameResponse(bool error);
	void getFriendsResponse(bool error);

private:
	void updateFacebookState();

private:
	Ui_PlayerOptionsDialog m_ui;
	GameEngine* m_gameEngine;

	FacebookLoginDialog* m_facebookLoginDialog;
	FacebookRequest* m_facebookRequest;
};


#endif
