#pragma once
#include "gmtk/gmtk.h"

float random(float min, float max);
vec2 random2(float min, float max);
vec3 random3(float min, float max);
vec4 random4(float min, float max);
vec2 random(vec2 min, vec2 max);
vec3 random(vec3 min, vec3 max);
vec4 random(vec4 min, vec4 max);

template<typename T>
inline T pack(const T& a)
{
	return a * 0.5f + 0.5f;
}

template<typename T>
inline T unpack(const T& a)
{
	return a * 2.0f - 1.0f;
}