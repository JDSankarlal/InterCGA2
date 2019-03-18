#version 420

layout(std140, binding = 0) uniform Camera
{
	uniform mat4 uProj;
	uniform mat4 uView;
	uniform mat4 uProjInverse;
	uniform mat4 uViewInverse; // Camera Position
};

layout(std140, binding = 3) uniform Light
{
	uniform vec3 uLightColor;
	uniform vec3 uLightPosition;
	uniform vec3 uLightDirection;
	uniform vec4 uLightAttenuation; 
};

uniform mat4 uViewToShadow;
layout(binding = 30) uniform sampler2D uTexShadowDepth;
layout(binding = 29) uniform sampler2D uTexLightColor;

#define uLightConstantAttenuation uLightAttenuation.x
#define uLightLinearAttenuation uLightAttenuation.y
#define uLightQuadAttenuation uLightAttenuation.z
#define uLightRadius uLightAttenuation.w

layout(std140, binding = 4) uniform Resolution
{
	uniform vec2 uResolution;
	uniform vec2 uPixelSize;
};

layout(std140, binding = 5) uniform Toon
{
	uniform bool uToonActive;
};
layout(binding = 31) uniform sampler2D uTexToonRamp;

uniform float uMaterialSpecularExponent = 64.0;

layout(binding = 0) uniform sampler2D uTexDepth;
layout(binding = 1) uniform sampler2D uTexAlbedo;
layout(binding = 2) uniform sampler2D uTexNormal;
//layout(binding = 3) uniform sampler2D uTexEmissive;
layout(binding = 4) uniform sampler2D uTexRoughness;
layout(binding = 5) uniform sampler2D uTexMetallic;

struct vData
{
	vec2 texcoord;
	vec3 norm;
	vec3 pos;
};
layout(location = 0) in vData o;

out vec3 outColor;

void main()
{
	// Calculate texture coordinate position on screen
	vec2 texOffset = gl_FragCoord.xy * uPixelSize;

	float depth =  texture(uTexDepth, texOffset).r;
	if(depth == 1) discard;
	vec4 position = uProjInverse * (vec4(texOffset, depth, 1.0) * 2 - 1);
	position /= position.w;

	vec4 albedoColor = texture(uTexAlbedo, texOffset);
	outColor.rgb = vec3(0); 

	vec3 normal = normalize(texture(uTexNormal, texOffset).rgb * 2 - 1);

	// TODO: Calculate the shadow texture coordinates by converting the viewspace position into the light's clip space
	vec4 shadowCoord = uViewToShadow * position;
	float shadowDepth = texture(uTexShadowDepth, shadowCoord.xy).r;

	// TODO: change level of shadow depending on whether the depth of the shadow map is in front or behind the scene
	// You'll also want to adjust this calculation to avoid shadow acne! (Refer to slides 17-18 for an example)
	// This threshold should be increased based on how perpendicular the light is to the surface!
	vec3 shadowAmount = texture(uTexLightColor, shadowCoord.xy).rgb;
	// shadowAmount is multiplied into the diffuse and specular equations, meaning there will be no lighting if shadowAmount is 0!
	if(shadowDepth < shadowCoord.z - 0.001)
	{
		shadowAmount = vec3(0.0);
	}

	// the dot product of the normal and light direction determines how much light there is
	vec3 lightDir = uLightDirection;
	float NdotL = dot(normal, lightDir);		
	// Clamp NdotL so that there are negative values, otherwise 
	// the opposite side of an object would receive negative lighting!
	NdotL = max(NdotL, 0.0);
	
	// Calculate the diffuse contribution
	outColor.rgb += albedoColor.rgb * uLightColor * NdotL;
				
	// Normalized vector pointing towards the camera
	vec3 eye = normalize(-position.xyz);
	vec3 reflection = reflect(-lightDir, normal);	
	float specularStrength = dot(reflection, eye);
	specularStrength = max(specularStrength, 0.0); // don't let it fall before zero

	// Calculate the specular contribution
	outColor.rgb += uLightColor * pow(specularStrength, uMaterialSpecularExponent);

	//outColor.rg = fract(shadowCoord.rg);
	//outColor.b = shadowCoord.b;
	//outColor.rg = vec2(0);
	//outColor.rgb = shadowDepth.xxx;
}
