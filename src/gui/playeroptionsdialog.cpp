#include "playeroptionsdialog.h"
#include "gameengine.h"
#include "settings.h"
#include "gamestate.h"
#include "facebookrequest.h"

#include <QDesktopServices>
#include <QUrl>

PlayerOptionsDialog::PlayerOptionsDialog(GameEngine* gameEngine)
	: GLUIWidget(gameEngine),
	  m_gameEngine(gameEngine)
{
	m_ui.setupUi(this);

	m_ui.nameBox->setText(Settings::instance()->playerName());

	connect(m_ui.closeButton, SIGNAL(clicked()), SLOT(updateSettings()));
	connect(m_ui.closeButton, SIGNAL(clicked()), SLOT(hideGLUI()));

	m_facebookRequest = NULL;

	updateFacebookState();
	
	m_facebookLoginDialog = new FacebookLoginDialog(gameEngine);
	connect(m_facebookLoginDialog, SIGNAL(keyEntered(const QString&)), SLOT(keyEntered(const QString&)));
	connect(m_ui.facebookButton, SIGNAL(clicked()), SLOT(facebookButtonClicked()));
}


PlayerOptionsDialog::~PlayerOptionsDialog()
{
}

void PlayerOptionsDialog::updateFacebookState()
{
	if (Settings::instance()->facebookSessionKey().isEmpty())
	{
		m_ui.facebookName->setText("Not logged in");
		m_ui.facebookButton->setText("Login...");
	}
	else
	{
		m_ui.facebookName->setText("Logged in (" + Settings::instance()->facebookName() + ")");
		m_ui.facebookButton->setText("Logout");
	}
}

void PlayerOptionsDialog::updateSettings()
{
	Settings::instance()->setPlayerName(m_ui.nameBox->text());
}

void PlayerOptionsDialog::facebookButtonClicked()
{
	if (m_facebookRequest)
		return;
	
	if (Settings::instance()->facebookSessionKey().isEmpty())
	{
		QDesktopServices::openUrl(QUrl("http://www.facebook.com/code_gen.php?v=1.0&api_key=" + Settings::instance()->facebookApiKey()));
		m_gameEngine->showMinimized();

		m_facebookLoginDialog->clearKey();
		m_gameEngine->uiManager()->showDialog(m_facebookLoginDialog);
		hideGLUI();
	}
	else
	{
		Settings::instance()->clearFacebookSession();
		updateFacebookState();
	}
}

void PlayerOptionsDialog::keyEntered(const QString& key)
{
	Settings::instance()->clearFacebookSession();

	m_gameEngine->uiManager()->showDialog(this);
	m_ui.facebookName->setText("Logging in...");

	delete m_facebookRequest;
	m_facebookRequest = new FacebookRequest("facebook.auth.getSession");
	m_facebookRequest->setArgument("auth_token", key);
	m_facebookRequest->start();
	connect(m_facebookRequest, SIGNAL(complete(bool)), SLOT(getSessionResponse(bool)));
}

void PlayerOptionsDialog::getSessionResponse(bool error)
{
	if (error)
		m_ui.facebookName->setText("Login error: " + m_facebookRequest->errorMessage());
	else
	{
		QString sessionKey = m_facebookRequest->response().elementsByTagName("session_key").at(0).toElement().text();
		QString sessionSecret = m_facebookRequest->response().elementsByTagName("secret").at(0).toElement().text();
		QString uid = m_facebookRequest->response().elementsByTagName("uid").at(0).toElement().text();

		Settings::instance()->setFacebookSession(sessionKey, sessionSecret, uid);
	}
	
	m_facebookRequest->deleteLater();
	m_facebookRequest = NULL;
	
	if (!error)
	{
		m_facebookRequest = new FacebookRequest("facebook.fql.query");
		m_facebookRequest->setArgument("query", "SELECT name FROM user WHERE uid=" + Settings::instance()->facebookUid());
		m_facebookRequest->start();
		connect(m_facebookRequest, SIGNAL(complete(bool)), SLOT(getNameResponse(bool)));
	}
}

void PlayerOptionsDialog::getNameResponse(bool error)
{
	if (error)
		m_ui.facebookName->setText("Login error: " + m_facebookRequest->errorMessage());
	else
	{
		QString name = m_facebookRequest->response().elementsByTagName("user").at(0).toElement().
		               elementsByTagName("name").at(0).toElement().text();
		
		Settings::instance()->setFacebookName(name);
		m_ui.facebookName->setText("Getting friends list...");
	}
	
	m_facebookRequest->deleteLater();
	m_facebookRequest = NULL;

	if (!error)
	{
		m_facebookRequest = new FacebookRequest("facebook.fql.query");
		m_facebookRequest->setArgument("query", "SELECT name FROM user WHERE uid IN (SELECT uid2 FROM friend WHERE uid1=" + Settings::instance()->facebookUid() + ")");
		m_facebookRequest->start();
		connect(m_facebookRequest, SIGNAL(complete(bool)), SLOT(getFriendsResponse(bool)));
	}
}

void PlayerOptionsDialog::getFriendsResponse(bool error)
{
	if (error)
		m_ui.facebookName->setText("Login error: " + m_facebookRequest->errorMessage());
	else
	{
		QStringList friends;
		
		QDomNodeList nodes = m_facebookRequest->response().elementsByTagName("user");
		for (int i=0 ; i<nodes.count() ; ++i)
			friends << nodes.at(i).toElement().elementsByTagName("name").at(0).toElement().text();

		Settings::instance()->setFacebookFriends(friends);

		updateFacebookState();
	}
	
	m_facebookRequest->deleteLater();
	m_facebookRequest = NULL;
}



