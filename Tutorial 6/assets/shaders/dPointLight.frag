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
	vec2 texOffset = gl_FragCoord.xy * uPixelSize;

	float depth =  texture(uTexDepth, texOffset).r;
	if(depth == 1) discard;
	vec4 position = uProjInverse * (vec4(texOffset, depth, 1.0) * 2 - 1);
	position /= position.w;

	vec4 albedoColor = texture(uTexAlbedo, texOffset);
	outColor.rgb = vec3(0); 

	vec3 normal = normalize(texture(uTexNormal, texOffset).rgb * 2 - 1);

	vec3 lightVec = uLightPosition - position.xyz;
	float dist = length(lightVec);
	vec3 lightDir = lightVec / dist;

	// the dot product of the normal and light direction determines how much light there is
	float NdotL = dot(normal, lightDir);
	
	// Calculate attenuation (falloff)
	// Add a small number to avoid divide by zero.
	float attenuation = 1.0 / (uLightConstantAttenuation + dist * uLightLinearAttenuation + dist * dist * uLightQuadAttenuation);

	if(uToonActive)
	{
		NdotL = NdotL * 0.5 + 0.5;
		// Calculate the diffuse contribution, substituting the NdotL into a color ramp
		outColor.rgb += albedoColor.rgb * uLightColor * texture(uTexToonRamp, vec2(NdotL, 0.5)).rgb * attenuation;
	}
	else
	{
		NdotL = max(NdotL, 0.0);
		// Calculate the diffuse contribution
		outColor.rgb += albedoColor.rgb * uLightColor * NdotL * attenuation;
	}
			
	// Normalized vector pointing towards the camera
	vec3 eye = normalize(position.xyz);
	vec3 reflection = reflect(lightDir, normal);
	float specularStrength = dot(reflection, eye);
	specularStrength = max(specularStrength, 0.0); // don't let it fall before zero

	// Calculate the specular contribution
	outColor.rgb += uLightColor * pow(specularStrength, uMaterialSpecularExponent) * attenuation;
}
