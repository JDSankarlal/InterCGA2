#include "Transform.h"
#include "IO.h"

Transform::Transform()
	: m_pLocalScale(1.0f), color(vec4(1.0f)), m_pParent(nullptr)
{
}

Transform::~Transform()
{

}

void Transform::addChild(Transform * newChild)
{
	if (newChild)
	{
		m_pChildren.push_back(newChild);
		newChild->m_pParent = this; 
	}
}

void Transform::removeChild(Transform * rip)
{
	for (unsigned int i = 0; i < m_pChildren.size(); ++i)
	{
		if (m_pChildren[i] == rip) 
		{
			m_pChildren.erase(m_pChildren.begin() + i);
		}
	}
}

void Transform::setLocalPos(vec3 newPosition)
{
	m_pLocalPosition = newPosition;
}

void Transform::setLocalRotX(float newAngle)
{
	m_pLocalRotationEuler.x = newAngle;
}

void Transform::setLocalRotY(float newAngle)
{
	m_pLocalRotationEuler.y = newAngle;
}

void Transform::setLocalRotZ(float newAngle)
{
	m_pLocalRotationEuler.z = newAngle;
}

void Transform::setLocalRot(vec3 newAngle)
{
	m_pLocalRotationEuler = newAngle;
}

void Transform::setScale(vec3 newScale)
{
	m_pLocalScale = newScale;
}

void Transform::setScale(float newScale)
{
	m_pLocalScale = vec3(newScale);
}

mat4 Transform::getLocalToWorld() const
{
	return m_pLocalToWorld;
}

vec3 Transform::getWorldPos() const
{
	if (m_pParent)
		return vec3(m_pParent->getLocalToWorld() * vec4(m_pLocalPosition, 1.0f));
	else
		return m_pLocalPosition;
}

mat4 Transform::getWorldRot() const
{
	if (m_pParent)
		return m_pParent->getWorldRot() * m_pLocalRotation;
	else
		return m_pLocalRotation;
}

vec3 Transform::getLocalScale() const
{
	return m_pLocalScale;
}

vec3 Transform::getLocalPos() const
{
	return m_pLocalPosition;
}

mat4 Transform::getLocalRot() const
{
	return m_pLocalRotation;
}

std::vector<Transform*> Transform::getChildren() const
{
	return m_pChildren;
}

Transform * Transform::getParent() const
{
	return m_pParent;
}

void Transform::update(float dt)
{
	// Create 4x4 transformation matrix

	// Create rotation matrix
	mat4 rx = rx.rotatex(degrees(m_pLocalRotationEuler.x));
	mat4 ry = ry.rotatey(degrees(m_pLocalRotationEuler.y));
	mat4 rz = rz.rotatez(degrees(m_pLocalRotationEuler.z));

	// Note: pay attention to rotation order, ZYX is not the same as XYZ
	m_pLocalRotation = rz * ry * rx;

	// Create translation matrix
	mat4 tran = tran.translate(m_pLocalPosition);

	// Create scale matrix
	mat4 scale = scale.scale(m_pLocalScale);

	// Combine all above transforms into a single matrix
	m_pLocalTransform = tran * m_pLocalRotation * scale;

	if (m_pParent)
	{
		m_pLocalToWorld = m_pParent->getLocalToWorld() * m_pLocalTransform;
	}
	else
	{
		m_pLocalToWorld = m_pLocalTransform;
	}

	// Update children
	for (unsigned int i = 0; i < m_pChildren.size(); i++)
		m_pChildren[i]->update(dt);
}

void Transform::draw()
{
	//SAT_DEBUG_LOG_WARNING("DRAWING TRANSFORM INSTEAD OF GAMEOBJECT");
}
