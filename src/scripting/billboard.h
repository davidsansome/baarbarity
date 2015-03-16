#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <QObject>
#include <QtScript>

#include "vector.h"

class WorldObject;
class Texture;
class Shader;

class Billboard : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
	Q_PROPERTY(float distance READ distance WRITE setDistance)
	Q_PROPERTY(bool terrainValidTarget READ isTerrainValidTarget WRITE setTerrainValidTarget)
	Q_PROPERTY(QScriptValue objectValidTargetCallback READ objectValidTargetCallback WRITE setObjectValidTargetCallback)
	Q_PROPERTY(QScriptValue droppedCallback READ droppedCallback WRITE setDroppedCallback)
	
public:
	enum DrawingHint
	{
		Normal,
		DraggingValid,
		DraggingInvalid
	};
	
	Billboard(WorldObject* parent);
	~Billboard();
	
	WorldObject* worldObject() const { return m_worldObject; }
	
	Q_INVOKABLE void loadTexture(const QString& fileName);
	
	QString name() const { return m_name; }
	void setName(const QString& name) { m_name = name; }
	
	QString description() const { return m_description; }
	void setDescription(const QString& description) { m_description = description; }
	
	bool isEnabled() const { return m_enabled; }
	void setEnabled(bool e) { m_enabled = e; }
	
	float distance() const { return m_distance; }
	void setDistance(float d) { m_distance = d; }
	
	bool isTerrainValidTarget() const { return m_terrainValidTarget; }
	void setTerrainValidTarget(bool v) { m_terrainValidTarget = v; }
	
	QScriptValue objectValidTargetCallback() const { return m_objectValidTargetCallback; }
	void setObjectValidTargetCallback(const QScriptValue& cb) { m_objectValidTargetCallback = cb; }
	bool isObjectValidTarget(WorldObject* target);
	
	QScriptValue droppedCallback() const { return m_droppedCallback; }
	void setDroppedCallback(const QScriptValue& cb) { m_droppedCallback = cb; }
	void dropped(const vec2& target);
	void dropped(WorldObject* target);
	
	static void bind();
	static void release();
	void draw(DrawingHint hint = Normal);
	
private:
	WorldObject* m_worldObject;
	
	QString m_name;
	QString m_description;
	bool m_enabled;
	float m_distance;
	bool m_terrainValidTarget;
	QScriptValue m_objectValidTargetCallback;
	QScriptValue m_droppedCallback;
	
	Texture* m_texture;
	
	static Shader* s_shader;
	static int s_texLoc;
};
Q_DECLARE_METATYPE(Billboard*)

#endif
