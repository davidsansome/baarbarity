#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QtScript>

class WorldObject;

class Timer : public QObject, protected QScriptable
{
	Q_OBJECT
	Q_PROPERTY(QScriptValue callback READ callback WRITE setCallback)
	Q_PROPERTY(uint interval READ interval WRITE setInterval)
	
public:
	Timer(WorldObject* thisObject);
	~Timer();
	
	QScriptValue callback() const { return m_callback; }
	void setCallback(const QScriptValue& callback) { m_callback = callback; }
	
	uint interval() const { return m_interval; }
	void setInterval(uint interval);
	
	static void tick(uint timeDelta);

private:
	void call();
	inline bool isReady() const;
	
	WorldObject* m_thisObject;
	
	QScriptValue m_callback;
	uint m_interval;
	uint m_timeout;
	
	static QList<Timer*> s_timers;
	static uint s_currentTime;
};
Q_DECLARE_METATYPE(Timer*)

#endif
