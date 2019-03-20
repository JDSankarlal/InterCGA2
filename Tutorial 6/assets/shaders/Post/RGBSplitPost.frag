/*
===========================================================================

SpeedStorm Shader Source Code
Copyright (C) 2015-2017 Stephen Thompson

===========================================================================
*/

#version 420 //Version of OpenGL we're using. - 4.2

layout(binding = 0) uniform sampler2D uSceneTex; 
uniform float uRandom = 0.0;
uniform float uRange = 0.0;
uniform vec2 rOffset = vec2(-0.002, 0.0);
uniform vec2 gOffset = vec2(0.0, 0.0);
uniform vec2 bOffset = vec2(0.002, 0.0);
in vec2 texcoord;
out vec4 outColor;

float rand(vec2 co)
{
    return fract(sin(1000.0*dot(co.xy ,vec2(21.5739,43.421))) * 617284.3);
}

void main () 
{
	vec2 random = vec2((1.0 - uRange) + uRange * 2.0 * rand(vec2(texcoord.y, uRandom)));

	vec2 rTexOffset = texcoord + rOffset * random;
	vec2 gTexOffset = texcoord + gOffset * random;
	vec2 bTexOffset = texcoord + bOffset * random;

	vec4 rValue = texture2D(uSceneTex, rTexOffset);
	vec4 gValue = texture2D(uSceneTex, gTexOffset);
	vec4 bValue = texture2D(uSceneTex, bTexOffset);


	rValue += texture2D(uSceneTex, mix(rTexOffset, gTexOffset, 0.250)) * 0.8;
	rValue += texture2D(uSceneTex, mix(rTexOffset, gTexOffset, 0.500)) * 0.6;
	rValue += texture2D(uSceneTex, mix(rTexOffset, gTexOffset, 0.750)) * 0.4;
	rValue += texture2D(uSceneTex, mix(rTexOffset, gTexOffset, 1.000)) * 0.2;

	bValue += texture2D(uSceneTex, mix(bTexOffset, gTexOffset, 0.250)) * 0.8;
	bValue += texture2D(uSceneTex, mix(bTexOffset, gTexOffset, 0.500)) * 0.6;
	bValue += texture2D(uSceneTex, mix(bTexOffset, gTexOffset, 0.750)) * 0.4;
	bValue += texture2D(uSceneTex, mix(bTexOffset, gTexOffset, 1.000)) * 0.2;

	gValue += texture2D(uSceneTex, mix(gTexOffset, rTexOffset, 0.250)) * 0.75;
	gValue += texture2D(uSceneTex, mix(gTexOffset, bTexOffset, 0.250)) * 0.75;
	gValue += texture2D(uSceneTex, mix(gTexOffset, rTexOffset, 0.500)) * 0.50;
	gValue += texture2D(uSceneTex, mix(gTexOffset, bTexOffset, 0.500)) * 0.50;
	gValue += texture2D(uSceneTex, mix(gTexOffset, rTexOffset, 0.750)) * 0.25;
	gValue += texture2D(uSceneTex, mix(gTexOffset, bTexOffset, 0.750)) * 0.25;

	
	//gValue *= 0.14285714285714285714285714285714;

	//rValue *= 0.2;
	gValue *= 0.25;
	//bValue *= 0.2;
	rValue *= 0.33333333333333333;
	//gValue *= 0.33333333333333333;
	bValue *= 0.33333333333333333;



	// Combine the offset colors.
	outColor =	vec4(rValue.r, gValue.g, bValue.b, 1.0);
}