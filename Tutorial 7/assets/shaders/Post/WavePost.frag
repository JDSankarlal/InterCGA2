/*
===========================================================================

SpeedStorm Shader Source Code
Copyright (C) 2015-2017 Stephen Thompson

===========================================================================
*/

#version 420

layout(binding = 0) uniform sampler2D uTexScene;
uniform vec2 uWaveCount = vec2(4.0, 4.0);
uniform vec2 uWaveIntensity = vec2(0.03, 0.03);

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};
uniform vec2 uWaveSpeed = vec2(1.0);

in vec2 texcoord;
out vec4 outColor;

void main( void ) 
{
	outColor = texture(uTexScene, texcoord + vec2(sin(texcoord.y * uWaveCount.x + uWaveSpeed.x * uTime) * uWaveIntensity.x, sin(texcoord.x * uWaveCount.y + uWaveSpeed.y * uTime) * uWaveIntensity.y));
}