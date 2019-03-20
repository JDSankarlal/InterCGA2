#version 420

layout(std140, binding = 0) uniform Camera
{
	uniform mat4 uProj;
	uniform mat4 uView;
	uniform mat4 uProjInverse;
	uniform mat4 uViewInverse; // Camera Position
};
uniform mat4 uModel;

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;

struct vData
{
	vec2 texcoord;
	vec3 norm;
	vec3 pos;
};
layout(location = 0) out vData o;

void main()
{	
	o.norm = mat3(uView) * in_normal;
	o.pos = (uModel * vec4(in_vert, 1.0f)).xyz;

	gl_Position = uProj * vec4(o.pos, 1.0f);
}
