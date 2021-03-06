#version 420 //Version of OpenGL we're using. - 4.2

layout(binding = 0) uniform sampler2D uTexScene;
layout(binding = 1) uniform sampler2D uTexDepth;

layout(std140, binding = 0) uniform Camera
{
	uniform mat4 uProj;
	uniform mat4 uView;
	uniform mat4 uProjInverse;
	uniform mat4 uViewInverse; // Camera Position
};

layout(std140, binding = 4) uniform Resolution
{
	uniform vec2 uResolution;
	uniform vec2 uPixelSize;
};

uniform float uBlurClamp = 1.05;  // max blur amount
uniform float uBias = 0.06; //aperture - bigger values for shallower depth of field
uniform float uFocus = 0.8f;  // this is the depth to focus at
uniform float uThreshold = 0.025f;

in vec2 texcoord;
out vec4 outColor;

vec2 Pack(vec2 param)
{
	return param * 0.5 + 0.5;
}

vec2 Unpack(vec2 param)
{
	return param + param - 1.0;
}

void main()
{
	vec2 texOffset = gl_FragCoord.xy * uPixelSize;
	
	// This value determines the amount of blur, we want it to be based on distance
	float depth = texture(uTexDepth, texOffset).r;
	float factor = abs(depth - uFocus);

	vec2 dofblur = vec2(clamp(factor * uBias - uThreshold, 0.0f, uBlurClamp));

	vec4 col = vec4(0, 0, 0, 1);
				
	col.rgb += texture(uTexScene, texOffset + vec2(0.0, - 4.0 * dofblur.y)).rgb * 0.06;
	col.rgb += texture(uTexScene, texOffset + vec2(0.0, - 3.0 * dofblur.y)).rgb * 0.09;
	col.rgb += texture(uTexScene, texOffset + vec2(0.0, - 2.0 * dofblur.y)).rgb * 0.12;
	col.rgb += texture(uTexScene, texOffset + vec2(0.0, -       dofblur.y)).rgb * 0.15;
	col.rgb += texture(uTexScene, texOffset                                          ).rgb * 0.16;
	col.rgb += texture(uTexScene, texOffset + vec2(0.0,         dofblur.y)).rgb * 0.15;
	col.rgb += texture(uTexScene, texOffset + vec2(0.0,   2.0 * dofblur.y)).rgb * 0.12;
	col.rgb += texture(uTexScene, texOffset + vec2(0.0,   3.0 * dofblur.y)).rgb * 0.09;
	col.rgb += texture(uTexScene, texOffset + vec2(0.0,   4.0 * dofblur.y)).rgb * 0.06;
	               
	outColor = col;
}