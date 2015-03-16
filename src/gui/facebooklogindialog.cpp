#include "facebooklogindialog.h"
#include "gameengine.h"

FacebookLoginDialog::FacebookLoginDialog(GameEngine* gameEngine)
	: GLUIWidget(gameEngine),
	  m_gameEngine(gameEngine)
{
	m_ui.setupUi(this);

	connect(m_ui.loginButton, SIGNAL(clicked()), SLOT(loginClicked()));
	connect(m_ui.cancelButton, SIGNAL(clicked()), SLOT(hideGLUI()));
}

FacebookLoginDialog::~FacebookLoginDialog()
{
}

void FacebookLoginDialog::clearKey()
{
	m_ui.key->clear();
}

void FacebookLoginDialog::loginClicked()
{
	if (m_ui.key->text().length() != 6)
		return;

	emit(keyEntered(m_ui.key->text()));
	hideGLUI();
}


