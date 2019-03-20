/*
===========================================================================

SpeedStorm Shader Source Code
Copyright (C) 2015-2017 Stephen Thompson

===========================================================================
*/

#version 420

layout(binding = 0) uniform sampler2D uSceneTex;
in vec2 texcoord;
out vec4 outColor;

void main() 
{
	outColor = texture(uSceneTex, texcoord);
}