#version 420

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
	outColor.rgb = vec3(1.0); 
}
