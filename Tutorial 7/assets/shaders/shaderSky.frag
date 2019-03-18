#version 420

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(binding = 0) uniform samplerCube uTexCube;

struct vData
{
	vec2 texcoord;
	vec3 norm;
	vec3 pos;
};
layout(location = 0) in vData o;

out vec4 outColor;

void main()
{
	outColor = texture(uTexCube, normalize(o.norm));
}
