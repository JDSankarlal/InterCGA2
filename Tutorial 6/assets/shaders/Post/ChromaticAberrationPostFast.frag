/*
===========================================================================

SpeedStorm Shader Source Code
Copyright (C) 2015-2017 Stephen Thompson

===========================================================================
*/

#version 420 //Version of OpenGL we're using. - 4.2

uniform sampler2D uSceneTex; 

layout(std140, binding = 4) uniform Resolution
{
	uniform vec2 uResolution;
	uniform vec2 uPixelSize;
};
//uniform vec2 uAspect =  vec2(1.0, 1.7777777777777777778);
uniform float uDispersal = 2.00;


uniform float rOffset = -0.050;
uniform float gOffset =  0.000;
uniform float bOffset =  0.050;

in vec2 texcoord;
out vec4 outColor;

vec2 pack(vec2 param)
{
	return param * 0.5 + 0.5;
}

vec2 unpack(vec2 param)
{
	return param + param - 1.0;
}

void main () 
{
	vec2 texOffset =  unpack(texcoord);
	vec2 uAspect = vec2(1.0, uResolution.y / uResolution.x);

	vec2 rTexOffset = texOffset;
	vec2 gTexOffset = texOffset;
	vec2 bTexOffset = texOffset;
	
	vec2 absTexOffset = abs(texOffset);
	vec2 isPositive = absTexOffset / texOffset;

	if((texOffset.x) != 0.0)
	{
		float distanceToCenter = absTexOffset.x * uAspect.x;
		float distanceToCenterSquared = pow(distanceToCenter, uDispersal);
	
		rTexOffset.x += rOffset * distanceToCenterSquared * isPositive.x;
		gTexOffset.x += gOffset * distanceToCenterSquared * isPositive.x;
		bTexOffset.x += bOffset * distanceToCenterSquared * isPositive.x;
	}

	if((texOffset.y) != 0.0)
	{
		float distanceToCenter = absTexOffset.y * uAspect.y;
		float distanceToCenterSquared = pow(distanceToCenter, uDispersal);
	
		rTexOffset.y += rOffset * distanceToCenterSquared * isPositive.y;
		gTexOffset.y += gOffset * distanceToCenterSquared * isPositive.y;
		bTexOffset.y += bOffset * distanceToCenterSquared * isPositive.y;
	}



	vec4 rValue;
	vec4 gValue;
	vec4 bValue;

	rTexOffset = pack(rTexOffset);
	gTexOffset = pack(gTexOffset);
	bTexOffset = pack(bTexOffset);

	rValue += texture2D(uSceneTex, rTexOffset);
	gValue += texture2D(uSceneTex, gTexOffset);
	bValue += texture2D(uSceneTex, bTexOffset);

	// Combine the offset colors.
	outColor =	vec4(rValue.r, gValue.g, bValue.b, 1.0);
}
