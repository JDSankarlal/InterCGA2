#version 440

layout(std140, binding = 0) uniform Camera
{
	uniform mat4 uProj;
	uniform mat4 uView;
	uniform mat4 uProjInverse;
	uniform mat4 uViewInverse; // Camera Position
};
layout(location=0) uniform mat4 uModel;

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(location = 0) in vec3 in_vert;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_normal;
layout(location = 4) in vec3 in_tangent;
layout(location = 5) in vec3 in_bitangent;

struct vData
{
	vec2 texcoord;
	vec3 norm;
	vec3 pos;
	vec3 tangent;
	vec3 bitangent;
};
layout(location = 0) out vData o;

void main()
{
	o.texcoord = in_uv;
	o.texcoord.y = 1 - o.texcoord.y;
	
	o.norm = mat3(uView) * mat3(uModel) * in_normal;
	o.tangent = mat3(uView) * mat3(uModel) * in_tangent;
	o.bitangent = mat3(uView) * mat3(uModel) * in_bitangent;

	o.pos = (uView * uModel * vec4(in_vert, 1.0f)).xyz;

	gl_Position = uProj * vec4(o.pos, 1.0f);
}
