#version 420

layout(std140, binding = 0) uniform Camera
{
	uniform mat4 uProj;
	uniform mat4 uView;
	uniform mat4 uProjInverse;
	uniform mat4 uViewInverse; // Camera Position
};

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(std140, binding = 2) uniform LightScene
{
	uniform vec3 uSceneAmbient;
};

layout(std140, binding = 4) uniform Resolution
{
	uniform vec2 uResolution;
	uniform vec2 uPixelSize;
};

layout(binding = 0) uniform sampler2D uTexDepth;
layout(binding = 1) uniform sampler2D uTexAlbedo;
layout(binding = 2) uniform sampler2D uTexNormal;
layout(binding = 3) uniform sampler2D uTexEmissive;
layout(binding = 4) uniform sampler2D uTexRoughness;
layout(binding = 5) uniform sampler2D uTexMetallic;
layout(binding = 6) uniform samplerCube uTexSky;

out vec3 outColor;

void main()
{
	vec2 texOffset = gl_FragCoord.xy * uPixelSize;

	float depth =  texture(uTexDepth, texOffset).r;
	vec4 position = uProjInverse * (vec4(texOffset, depth, 1.0) * 2 - 1);
	position.xyz /= position.w;

	vec3 ambientColor = texture(uTexAlbedo, texOffset).rgb;
	ambientColor *= uSceneAmbient; 

	vec3 emissiveColor = texture(uTexEmissive, texOffset).rgb;
	outColor = ambientColor + emissiveColor;
}
