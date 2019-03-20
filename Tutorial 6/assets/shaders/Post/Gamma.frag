#version 420

layout(binding = 0) uniform sampler2D uTexScene;

uniform float uGamma = 2.2;

in vec2 texcoord;
out vec4 outColor;

void main()
{
	vec4 color = texture(uTexScene, texcoord);
	color = pow(color, vec4(1.0/uGamma));
	
	outColor = color;
}