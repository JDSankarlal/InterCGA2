#version 420

layout(std140, binding = 1) uniform Time
{
	uniform float uTime;
};

layout(std140, binding = 2) uniform LightScene
{
	uniform vec3 uSceneAmbient;
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

layout(std140, binding = 5) uniform Toon
{
	uniform bool uToonActive;
};
layout(binding = 31) uniform sampler2D uTexToonRamp;

uniform float uMaterialSpecularExponent = 64.0;

layout(binding = 0) uniform sampler2D uTexAlbedo;
layout(binding = 1) uniform sampler2D uTexEmissive;
layout(binding = 2) uniform sampler2D uTexSpecular;

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
	vec2 texOffset = o.texcoord;

	vec4 albedoColor = texture(uTexAlbedo, texOffset);
	outColor.rgb = albedoColor.rgb * uSceneAmbient; 
	outColor.a = albedoColor.a;

	// Fix length after rasterizer interpolates
	vec3 normal = normalize(o.norm);

	vec3 lightVec = uLightPosition - o.pos;
	float dist = length(lightVec);
	vec3 lightDir = lightVec / dist;

	float NdotL = dot(normal, lightDir);

	

	// If the normal is facing the light
	{
		// Normalized vector pointing towards the camera
		vec3 eye = normalize(-o.pos);
		
		// Calculate attenuation (falloff)
		// Add a small number to avoid divide by zero.
		float attenuation = 1.0 / (uLightConstantAttenuation + dist * uLightLinearAttenuation + dist * dist * uLightQuadAttenuation);

		if(uToonActive)
		{
			NdotL = NdotL * 0.5 + 0.5;
			outColor.rgb += albedoColor.rgb * uLightColor * texture(uTexToonRamp, vec2(NdotL, 0.5)).rgb * attenuation;
		}
		else
		{
			NdotL = max(NdotL, 0.0);
			// Calculate the diffuse contribution
			outColor.rgb += albedoColor.rgb * uLightColor * NdotL * attenuation;
		}
				
		vec3 reflection = reflect(-lightDir, normal);
		
		float specularStrength = dot(reflection, eye);
		specularStrength = max(specularStrength, 0.0f); // don't let it fall before zero

		// Calculate the specular contribution
		outColor.rgb += texture(uTexSpecular, texOffset).rgb * uLightColor * pow(specularStrength, uMaterialSpecularExponent) * attenuation;
	}

	outColor.rgb += texture(uTexEmissive, texOffset).rgb;
}
