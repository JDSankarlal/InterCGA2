#version 420

layout(binding = 0) uniform sampler2D uTexAlbedo;
layout(binding = 1) uniform sampler2D uTexEmissive;
layout(binding = 2) uniform sampler2D uTexRoughness;
layout(binding = 3) uniform sampler2D uTexMetallic;
layout(binding = 4) uniform sampler2D uTexNormal;

struct vData
{
	vec2 texcoord;
	vec3 norm;
	vec3 pos;
	vec3 tangent;
	vec3 bitangent;
};
layout(location = 0) in vData o;

uniform float uRoughness = 1.0f;
uniform float uMetallic = 0.0f;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outNormal;
layout (location = 2) out vec3 outEmissive;
layout (location = 3) out float outRoughness;
layout (location = 4) out float outMetallic;

void main()
{
	vec2 texOffset = o.texcoord;

	vec4 albedoColor = texture(uTexAlbedo, texOffset);
	if(albedoColor.a < 0.5)	discard;
	
	outColor = albedoColor.rgb;

	outRoughness = texture(uTexRoughness, texOffset).r * uRoughness;
	outMetallic = texture(uTexMetallic, texOffset).r * uMetallic;

	vec3 normal = normalize(o.norm);
	vec3 tangent = normalize(o.tangent);
	vec3 bitangent = normalize(o.bitangent);
	mat3 tangentSpaceToViewSpace = mat3(
		tangent.x, bitangent.x, normal.x,
		tangent.y, bitangent.y, normal.y,
		tangent.z, bitangent.z, normal.z);

	//Get tangent space normals, and convert to -1 to 1 range
	normal = texture(uTexNormal, texOffset).rgb * 2.0 - 1.0;

	// Rotate tangent spae normals into view space
	normal = normal * tangentSpaceToViewSpace;

	// Fix length after rasterizer interpolates
	outNormal = normalize(normal) * 0.5 + 0.5;
	outEmissive = texture(uTexEmissive, texOffset).rgb;
}
