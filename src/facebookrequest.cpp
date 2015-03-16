#include "facebookrequest.h"
#include "settings.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QUrl>
#include <QDomDocument>

#include <sys/time.h>
#include <time.h>

int FacebookRequest::callId = 0;

FacebookRequest::FacebookRequest(const QString& method)
	: m_errorCode(0)
{
	m_attributes["api_key"] = Settings::instance()->facebookApiKey();
	m_attributes["v"] = "1.0";
	m_attributes["method"] = method;
	if (!Settings::instance()->facebookSessionKey().isNull())
		m_attributes["session_key"] = Settings::instance()->facebookSessionKey();

	m_http = new QHttp("api.facebook.com", 80, this);
	connect(m_http, SIGNAL(requestFinished(int, bool)), SLOT(requestFinished(int, bool)));
}


FacebookRequest::~FacebookRequest()
{
}

QString FacebookRequest::createSignature() const
{
	QString requestStr;
	
	QMapIterator<QString, QString> it(m_attributes);
	while (it.hasNext())
	{
		it.next();

		if (it.key() == "sig")
			continue;
		
		requestStr += it.key() + "=" + it.value();
	}

	if (Settings::instance()->facebookSessionSecret().isNull())
		requestStr += Settings::instance()->facebookSecret();
	else
		requestStr += Settings::instance()->facebookSessionSecret();

	QByteArray result = QCryptographicHash::hash(requestStr.toAscii(), QCryptographicHash::Md5);

	QString signature;
	for (int i=0 ; i<16 ; ++i)
	{
		unsigned char c = result[i];
		QString hex = QString::number(c, 16);
		
		if (hex.length() == 1)
			hex = "0" + hex;
		
		signature += hex;
	}

	return signature;
}

void FacebookRequest::start()
{
	m_attributes["call_id"] = QString::number(callId++, 'f', 0);
	m_attributes["sig"] = createSignature();

	QString postData;
	QMapIterator<QString, QString> it(m_attributes);
	while (it.hasNext())
	{
		it.next();
		if (!postData.isEmpty()) postData += "&";
		postData += escapePost(it.key()) + "=" + escapePost(it.value());
	}

	qDebug() << postData;

	QHttpRequestHeader header("POST", "/restserver.php");
	header.setValue("Content-type", "application/x-www-form-urlencoded");
	header.setValue("Host", "api.facebook.com");
	header.setValue("User-Agent", "Photo Manager");
	m_http->request(header, postData.toAscii());
}

QString FacebookRequest::escapePost(const QString& input)
{
	QString ret = QUrl::toPercentEncoding(input);
	return ret.replace(" ", "+");
}

void FacebookRequest::requestFinished(int id, bool error)
{
	Q_UNUSED(id)
	
	if (error)
	{
		m_errorCode = m_http->error();
		m_errorMessage = m_http->errorString();
	}
	else
	{
		parseResponse(m_http->readAll());
	}
	emit(complete(m_errorCode != 0));
}

void FacebookRequest::setArgument(const QString& name, const QString& value)
{
	m_attributes[name] = value;
}

void FacebookRequest::parseResponse(const QString& data)
{
	qDebug() << data;
	
	QDomDocument doc;
	doc.setContent(data);

	m_response = doc.documentElement();
	
	if (m_response.tagName() == "error_response")
	{
		m_errorCode = m_response.elementsByTagName("error_code").at(0).toElement().text().toInt();
		m_errorMessage = m_response.elementsByTagName("error_msg").at(0).toElement().text();
	}
}


