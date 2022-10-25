#type vertex
#version 450

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_TexCoords;
	gl_Position = vec4(a_Position, 0.0, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoords;

struct DirectionalLight
{
	vec3 Direction;
	vec3 Diffuse;
	bool ShadowCaster;
	mat4 LightSpaceTransform;
};

struct PointLight
{
	vec3 Position;
	vec3 Diffuse;
	bool ShadowCaster;
};

#define NR_DIRECTIONAL_LIGHTS 6
#define NR_POINT_LIGHTS 15

uniform int u_NumDirectionalLights;
uniform int u_NumPointLights;

uniform DirectionalLight u_DirectionalLights[NR_DIRECTIONAL_LIGHTS];
uniform PointLight u_PointLights[NR_POINT_LIGHTS];

#define NR_SHADOW_MAPS 2
#define NR_POINT_LIGHT_SHADOW_MAPS 5

uniform sampler2D u_ShadowMaps[NR_SHADOW_MAPS];
uniform samplerCube u_PointLightShadowMaps[NR_POINT_LIGHT_SHADOW_MAPS];

uniform sampler2D u_FragmentPosition;
uniform sampler2D u_Normal;
uniform sampler2D u_Albedo;
uniform sampler2D u_MetallicRoughness;

uniform vec3 u_ViewPosition;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrix;
uniform mat4 u_InvViewMatrix;

uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilteredEnvironmentMap;
uniform sampler2D u_BrdfLut;

uniform sampler2D u_Ssao;

uniform sampler2D u_PreviousFrame;

vec3 CalculateIrradiance(vec3 lightDirection, vec3 diffuse, float distance);

float ShadowDirectionalLight(vec4 lightSpaceFragmentPosition, int index);
float ShadowPointLight(vec3 lightToFragment, int index);

vec3 fresnelSchlick(float hDotV, vec3 baseReflectivity);
vec3 fresnelSchlickRoughness(float nDotV, float roughness, vec3 baseReflectivity);
float DistributionGGX(float nDotH, float roughness);
float GeometrySchlickGGX(float nDot, float roughness);
float GeometrySmith(float nDotV, float nDotL, float roughness);

vec4 ScreenSpaceReflections();
vec4 RayCast(vec3 dir, inout vec3 hitCoords, out float deltaDepth);
vec3 BinarySearch(vec3 dir, inout vec3 hitCoords, inout float deltaDepth);
vec3 hash(vec3 a);

const float PI = 3.14159265359;

// SSR Parameters
const float rayStep = 0.1;
const float minRayStep = 0.1;
const int maxSteps = 30;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 5.0;
const vec3 hashScale = vec3(0.8, 0.8, 0.8);
const float K = 19.19;

vec3 fragmentPosition, albedo, normal, baseReflectivity, viewDirection;
vec4 metallicRoughness;
float metallic, roughness, nDotV;

void main()
{
	fragmentPosition = texture(u_FragmentPosition, v_TexCoords).xyz;

	if (fragmentPosition == vec3(0.0, 0.0, 0.0))
		discard;

	albedo = pow(texture(u_Albedo, v_TexCoords).rgb, vec3(2.2));

	normal = texture(u_Normal, v_TexCoords).xyz;

	metallicRoughness = texture(u_MetallicRoughness, v_TexCoords);
	metallic = metallicRoughness.b;
	roughness = metallicRoughness.g;

	float ambientOcclusion = texture(u_Ssao, v_TexCoords).r;
	float ao = 1.0 - ambientOcclusion;
	ao *= 0.1;

	baseReflectivity = mix(vec3(0.04), albedo, metallic);

	vec3 irradiance = vec3(0.0);

	viewDirection = normalize(u_ViewPosition - fragmentPosition);

	/*
	 * CONSTANT DOT PRODUCTS
	 */

	nDotV = max(dot(normal, viewDirection), 0.0);

	for (int i = 0; i < u_NumDirectionalLights; i++)
	{
		DirectionalLight directionalLight = u_DirectionalLights[i];

		vec3 direction = normalize(directionalLight.Direction);

		float shadow = 0.0;

		if (directionalLight.ShadowCaster)
		{
			vec4 lightSpaceFragmentPosition = directionalLight.LightSpaceTransform * vec4(fragmentPosition, 1.0);
			shadow = ShadowDirectionalLight(lightSpaceFragmentPosition, i);
		}

		vec3 lightIrradiance = CalculateIrradiance(direction, directionalLight.Diffuse, 1.0);

		irradiance += lightIrradiance * (1 - shadow);
	}

	for (int i = 0; i < u_NumPointLights; i++)
	{
		PointLight pointLight = u_PointLights[i];

		vec3 lightToFragment = pointLight.Position - fragmentPosition;
		float distance = length(lightToFragment);

		float shadow = 0.0;

		if (pointLight.ShadowCaster)
			shadow = ShadowPointLight(lightToFragment, i);

		vec3 lightDirection = normalize(lightToFragment);

		vec3 lightIrradiance = CalculateIrradiance(lightDirection, pointLight.Diffuse, distance);
		
		irradiance += lightIrradiance * 0.0; /* * (1 - shadow) */;
	}

	vec3 fSR = fresnelSchlickRoughness(nDotV, roughness, baseReflectivity);
	vec3 kS = fSR;
	vec3 kD = 1.0 - kS;
	kD *= (1.0 - metallic);

	vec3 irradianceEnv = texture(u_IrradianceMap, normal).rgb;
	vec3 diffuse = irradianceEnv * albedo;

	vec2 envBRDF = texture(u_BrdfLut, vec2(nDotV, roughness)).rg;

	/*
	 * REFLECTIONS
	 */

	const float MAX_REFLECTION_LOD = 4.0;
	vec3 R = reflect(-viewDirection, normal);
	vec3 prefilteredColor = textureLod(u_PrefilteredEnvironmentMap, R, roughness * MAX_REFLECTION_LOD).rgb;

	vec3 reflectionColor;

	if (metallic > 0.01)
	{
		vec2 deltaCoords;
		vec4 ssrOutput = ScreenSpaceReflections();

		vec3 SSR = ssrOutput.rgb;
		float reflectionMultiplier = ssrOutput.a;

		reflectionColor = SSR * reflectionMultiplier + prefilteredColor * (0.9 - reflectionMultiplier);
	}
	else
	{
		reflectionColor = prefilteredColor;
	}

	/*
	 * PUTTING IT ALL TOGETHER
	 */

	vec3 specular = reflectionColor * (fSR * envBRDF.x + envBRDF.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

	vec3 color = ambient + irradiance;

	// HDR
	color = color / (color + vec3(1.0));

	// gamma correction
	color = pow(color, vec3(1.0 / 2.2));

	o_Color = vec4(color, 1.0);
}

vec3 CalculateIrradiance(vec3 lightDirection, vec3 diffuse, float distance)
{
	vec3 halfwayVector = normalize(viewDirection + lightDirection);

	float attenuation = 1.0 / (distance * distance);

	vec3 radiance = diffuse * attenuation;

	/*
	 * VARIABLE DOT PRODUCTS
	 */

	float hDotV = max(dot(halfwayVector, viewDirection), 0.0);
	float nDotH = max(dot(normal, halfwayVector), 0.0);
	float nDotL = max(dot(normal, lightDirection), 0.0);

	/*
	 * BRDF
	 */

	 // Fresnel-Schlick Approximation

	vec3 fresnel = fresnelSchlick(hDotV, baseReflectivity);

	// Normal Distribution Function

	float normalDistribution = DistributionGGX(nDotH, roughness);

	// Geometry Function

	float geometry = GeometrySmith(nDotV, nDotL, roughness);

	// Combining everything

	vec3 numerator = normalDistribution * geometry * fresnel;
	float denominator = 4.0 * nDotV * nDotL + 0.0001;
	vec3 specular = numerator / denominator;

	/*
	 * SPECULAR AND DEFUSE FACTORS
	 */

	vec3 kS = fresnel;
	vec3 kD = vec3(1.0) - kS;

	kD *= (1.0 - metallic);

	return (kD * albedo / PI + specular) * radiance * nDotL;
}

float ShadowDirectionalLight(vec4 lightSpaceFragmentPosition, int index)
{
	vec3 sampleCoords = lightSpaceFragmentPosition.xyz / lightSpaceFragmentPosition.w;
	sampleCoords = sampleCoords * 0.5 + 0.5;

	if (sampleCoords.z > 1.0)
		return 0.0;

	float closestDepth = texture(u_ShadowMaps[index], sampleCoords.xy).r;
	float currentDepth = sampleCoords.z;

	const float bias = 0.001;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_ShadowMaps[index], 0);

	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(u_ShadowMaps[index], sampleCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;

	return shadow;
}

float ShadowPointLight(vec3 lightToFragment, int index)
{
	float closestDepth = texture(u_PointLightShadowMaps[index], -lightToFragment).r;

	float currentDepth = length(lightToFragment);

	float bias = 0.05;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
}

vec3 fresnelSchlick(float hDotV, vec3 baseReflectivity)
{
	/*
		F_(schlick) (h, v, F0) = F0 + (1 - F0) * (1 - h * v)^5
	*/

	float clampedDot = clamp(1.0 - hDotV, 0.0, 1.0);
	return baseReflectivity + (1.0 - baseReflectivity) * pow(clampedDot, 5.0);
}

vec3 fresnelSchlickRoughness(float nDotV, float roughness, vec3 baseReflectivity)
{
	float clampedDot = clamp(1.0 - nDotV, 0.0, 1.0);
	vec3 reflectivity = max(vec3(1.0 - roughness), baseReflectivity);
	return baseReflectivity + (reflectivity - baseReflectivity) * pow(clampedDot, 5.0);
}

float DistributionGGX(float nDotH, float roughness)
{
	/*
											   a^2
		NDF_(GGXTR) (n, h, a) = ---------------------------------
								pi * ((n * h)^2 * (a^2 - 1) + 1)^2
	*/

	float a = roughness * roughness;
	float a2 = a * a;
	float denomFactor = nDotH * nDotH * (a2 - 1) + 1;
	float denom = PI * denomFactor * denomFactor;

	return a2 / denom;
}

float GeometrySchlickGGX(float nDot, float roughness)
{
	/*
										   n * v
		G_(SchlickGGX) (n, v, k) = ---------------------
								   (n * v) * (1 - k) + k


		See GeometrySmith -> v can also be h

				 a^2
		with k = ---
				  8
	*/

	float a = roughness * roughness;
	float aPlusOne = a + 1.0;
	float k = (aPlusOne * aPlusOne) / 8.0;

	float nom = nDot;
	float denom = nDot * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(float nDotV, float nDotL, float roughness)
{
	/*
		G(n, v, l, k) = G_(sub) (n, v, k) * G_(sub) (n, l, k)
	*/

	return GeometrySchlickGGX(nDotV, roughness) * GeometrySchlickGGX(nDotL, roughness);
}

vec4 ScreenSpaceReflections()
{
	vec3 viewSpacePosition = vec3(u_ViewMatrix * vec4(fragmentPosition, 1.0));
	vec3 viewSpaceNormal = vec3(vec4(normal, 1.0) * u_InvViewMatrix);

	vec3 reflectionVector = normalize(reflect(normalize(viewSpacePosition), normalize(viewSpaceNormal)));

	vec3 hitPos = viewSpacePosition;
	float deltaDepth;

	vec3 jitt = mix(vec3(0.0), vec3(hash(fragmentPosition)), roughness);

	vec4 coords = RayCast(jitt + reflectionVector * max(minRayStep, -viewSpacePosition.z), hitPos, deltaDepth);
	vec2 deltaCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));

	float screenEdgeFactor = clamp(1.0 - (deltaCoords.x + deltaCoords.y), 0.0, 1.0);

	float reflectionMultiplier = pow(metallic, reflectionSpecularFalloffExponent) * screenEdgeFactor * -reflectionVector.z;
	reflectionMultiplier = clamp(reflectionMultiplier, 0.0, 0.9);

	return vec4(texture(u_PreviousFrame, coords.xy).rgb, reflectionMultiplier);
}

vec4 RayCast(vec3 dir, inout vec3 hitCoords, out float deltaDepth)
{
	dir *= rayStep;

	float depth;
	vec4 projectedCoords;

	for (int i = 0; i < maxSteps; i++)
	{
		hitCoords += dir;

		projectedCoords = u_ProjectionMatrix * vec4(hitCoords, 1.0);
		projectedCoords.xy /= projectedCoords.w;
		projectedCoords.xy = projectedCoords.xy * 0.5 + 0.5;

		depth = (u_ViewMatrix * texture(u_FragmentPosition, projectedCoords.xy)).z;

		if (depth > 1000.0)
			continue;

		deltaDepth = hitCoords.z - depth;

		if ((dir.z - deltaDepth) < 1.5 && deltaDepth <= 0.05)
		{
			return vec4(BinarySearch(dir, hitCoords, deltaDepth), 1.0);
		}
	}

	return vec4(BinarySearch(dir, hitCoords, deltaDepth), 0.0);
}

vec3 BinarySearch(vec3 dir, inout vec3 hitCoords, inout float deltaDepth)
{
	float depth;
	vec4 projectedCoords;

	for (int i = 0; i < numBinarySearchSteps; i++)
	{
		projectedCoords = u_ProjectionMatrix * vec4(hitCoords, 1.0);
		projectedCoords.xy /= projectedCoords.w;
		projectedCoords.xy = projectedCoords.xy * 0.5 + 0.5;

		depth = (u_ViewMatrix * texture(u_FragmentPosition, projectedCoords.xy)).z;

		deltaDepth = hitCoords.z - depth;

		dir *= 0.5;
		if (deltaDepth > 0.0)
			hitCoords += dir;
		else
			hitCoords -= dir;
	}

	projectedCoords = u_ProjectionMatrix * vec4(hitCoords, 1.0);
	projectedCoords.xy /= projectedCoords.w;
	projectedCoords.xy = projectedCoords.xy * 0.5 + 0.5;

	return vec3(projectedCoords.xy, depth);
}

vec3 hash(vec3 a)
{
	a = fract(a * hashScale);
	a += dot(a, a.yxz + K);
	return fract((a.xxy + a.yxx) * a.zyx);
}
