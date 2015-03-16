#ifndef FACEBOOKREQUEST_H
#define FACEBOOKREQUEST_H

#include <QMap>
#include <QHttp>
#include <QDomElement>

class FacebookRequest : public QObject
{
	Q_OBJECT
	
public:
	FacebookRequest(const QString& method);
	~FacebookRequest();

	void setArgument(const QString& name, const QString& value);
	void start();

	int errorCode() const { return m_errorCode; }
	QString errorMessage() const { return m_errorMessage; }

	QDomElement response() const { return m_response; }

signals:
	void complete(bool error);

private slots:
	void requestFinished(int id, bool error);

private:
	QString createSignature() const;
	static QString escapePost(const QString& input);
	void parseResponse(const QString& data);

private:
	QMap<QString, QString> m_attributes;

	QHttp* m_http;
	int m_errorCode;
	QString m_errorMessage;

	QDomElement m_response;

	static int callId;
};

#endif
