#pragma once

#include <gmtk/gmtk.h>
#include <vector>
#include <string>

class Transform
{
public:
	vec3f m_pLocalScale;
	vec3f m_pLocalRotationEuler;
	vec3f m_pLocalPosition;

	mat4f m_pLocalRotation;
	
	mat4f m_pLocalTransform;
	mat4f m_pLocalToWorld;

	Transform* m_pParent;
	std::vector<Transform*> m_pChildren;

	Transform();
	~Transform();

	void addChild(Transform* newChild);
	void removeChild(Transform* ripChild);

	void setLocalPos(vec3 newPosiiton); // Local Position
	void setLocalRotX(float newAngle); // Local Rotation X
	void setLocalRotY(float newAngle); // Local Rotation Y
	void setLocalRotZ(float newAngle); // Local Rotation Z
	void setLocalRot(vec3 newAngle); // Local Rotation
	void setScale(vec3 newScale); // Local Scale
	void setScale(float newScale); // Local Scale

	mat4 getLocalToWorld() const; // Get Local to World Matrix
	vec3 getWorldPos() const; // Get World Position
	mat4 getWorldRot() const; // Get World Rotation
	vec3 getLocalScale() const; // Get Local Scale
	vec3 getLocalPos() const; // Get Local Position
	mat4 getLocalRot() const; // Get Local Rotation

	std::vector<Transform*> getChildren() const;
	Transform* getParent() const;

	virtual void update(float dt);	
	virtual void draw();

protected:
	// Other Properties
	std::string name;
	vec4f color; 
};