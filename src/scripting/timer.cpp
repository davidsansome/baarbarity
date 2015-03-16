#include "timer.h"
#include "worldobject.h"
#include "scriptmanager.h"

QList<Timer*> Timer::s_timers;
uint Timer::s_currentTime = 0;

Timer::Timer(WorldObject* thisObject)
	: QObject(thisObject),
	  m_thisObject(thisObject),
	  m_interval(0),
	  m_timeout(0)
{
	s_timers << this;
}

Timer::~Timer()
{
	s_timers.removeAll(this);
}

void Timer::setInterval(uint interval)
{
	m_interval = interval;
	m_timeout = s_currentTime + m_interval;
}

void Timer::call()
{
	QScriptValue object(m_callback.engine()->toScriptValue(m_thisObject));
	
	QScriptValue ret(m_callback.call(object));
	ScriptManager::handleExceptions();
	
	if (ret.isBoolean() && !ret.toBoolean())
		deleteLater();
	else
		m_timeout = s_currentTime + m_interval;
}

bool Timer::isReady() const
{
	return m_timeout < s_currentTime;
}

void Timer::tick(uint timeDelta)
{
	s_currentTime += timeDelta;
	
	foreach (Timer* const timer, s_timers)
	{
		if (timer->isReady())
			timer->call();
	}
}

