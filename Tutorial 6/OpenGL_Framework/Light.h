#pragma once
#include "Transform.h"
#include "UniformBuffer.h"
#include "Mesh.h"

class Light : public Transform
{
public:
	enum LightType
	{
		Directional,
		Point,
		Spotlight
	};
	
	vec4 color;
	vec4 position;
	vec4 direction;
	
	float constantAtten;
	float linearAtten;
	float quadAtten;
	float radius;

	LightType type;
	Mesh* m_LightVolume;

	Light();
	void init();

	void bind(); // Binds the Uniform Buffer Object
	void update(float dt);
	void sendToGPU(); // Updates the uniform buffer on the GPU
	float calculateRadius();

	UniformBuffer _UBO;
};