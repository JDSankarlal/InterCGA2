#include "SatMath.h"
#include <random>

std::default_random_engine generator;

float random(float min, float max)
{
	std::uniform_real_distribution<float> randomfloats(min, max);
	return randomfloats(generator);
}

vec2 random2(float min, float max)
{
	std::uniform_real_distribution<float> randomfloats(min, max);
	vec2 output;
	for (unsigned i = 0; i < 2; ++i)
	{
		output[i] = randomfloats(generator);
	}
	return output;
}

vec3 random3(float min, float max)
{
	std::uniform_real_distribution<float> randomfloats(min, max);
	vec3 output;
	for (unsigned i = 0; i < 3; ++i)
	{
		output[i] = randomfloats(generator);
	}
	return output;
}

vec4 random4(float min, float max)
{
	std::uniform_real_distribution<float> randomfloats(min, max);
	vec4 output;
	for (unsigned i = 0; i < 4; ++i)
	{
		output[i] = randomfloats(generator);
	}
	return output;
}

vec2 random(vec2 min, vec2 max)
{
	std::uniform_real_distribution<float> randomfloats;
	vec2 output;
	for (unsigned i = 0; i < 2; ++i)
	{
		randomfloats = std::uniform_real_distribution<float>(min[i], max[i]);
		output[i] = randomfloats(generator);
	}
	return output;
}

vec3 random(vec3 min, vec3 max)
{
	std::uniform_real_distribution<float> randomfloats;
	vec3 output;
	for (unsigned i = 0; i < 3; ++i)
	{
		randomfloats = std::uniform_real_distribution<float>(min[i], max[i]);
		output[i] = randomfloats(generator);
	}
	return output;
}

vec4 random(vec4 min, vec4 max)
{
	std::uniform_real_distribution<float> randomfloats;
	vec4 output;
	for (unsigned i = 0; i < 4; ++i)
	{
		randomfloats = std::uniform_real_distribution<float>(min[i], max[i]);
		output[i] = randomfloats(generator);
	}
	return output;
}
