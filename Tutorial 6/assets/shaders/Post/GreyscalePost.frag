/*
===========================================================================

SpeedStorm Shader Source Code
Copyright (C) 2015-2017 Stephen Thompson

===========================================================================
*/

#version 420 //Version of OpenGL we're using. - 4.2

layout(binding = 0) uniform sampler2D uSceneTex; 

uniform float uAmount = 1.0f;
in vec2 texcoord;
out vec4 outColor;

void main()
{
	vec4 source = texture(uSceneTex, texcoord);
	
	float luminance = 0.2989 * source.r + 0.5874 * source.g + 0.1137 * source.b;
	outColor.rgb = mix(source.rgb, vec3(luminance), uAmount);
	outColor.a = 1.0f;
}