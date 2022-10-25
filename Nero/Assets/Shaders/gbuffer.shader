#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in vec3 a_Tangent;

out mat3 v_TBN;
out vec2 v_TexCoords;
out vec3 v_FragmentPosition;

out vec3 v_Normal;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
	vec3 surfaceNormal = normalize(vec3(u_ModelMatrix * vec4(a_Normal, 0.0)));
	vec3 tangent = normalize(vec3(u_ModelMatrix * vec4(a_Tangent, 0.0)));

	tangent = normalize(tangent - dot(tangent, surfaceNormal) * surfaceNormal);
	vec3 bitangent = cross(surfaceNormal, tangent);

	v_TBN = mat3(tangent, bitangent, surfaceNormal);

	v_Normal = surfaceNormal;
	v_TexCoords = a_TexCoords;

	v_FragmentPosition = vec3(u_ModelMatrix * vec4(a_Position, 1.0));

	gl_Position = u_ViewProjectionMatrix * vec4(v_FragmentPosition, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 o_FragmentPosition;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_Albedo;
layout(location = 3) out vec4 o_MetallicRoughness;

in mat3 v_TBN;
in vec2 v_TexCoords;
in vec3 v_FragmentPosition;

in vec3 v_Normal;

uniform bool u_UseNormalMapping;

uniform vec3 u_ViewPosition;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicRoughnessMap;
uniform sampler2D u_DisplacementMap;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
	mat3 transposedTBN = transpose(v_TBN);
	vec3 viewDirection = transposedTBN * normalize(u_ViewPosition - v_FragmentPosition);

	vec2 texCoords = v_TexCoords; // ParallaxMapping(v_TexCoords, viewDirection);

	//if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
	//	discard;

	vec4 albedo = texture(u_AlbedoMap, texCoords);
	if (albedo.a < 0.5)
		discard;

	vec3 normal;

	if (u_UseNormalMapping)
	{
		normal = texture(u_NormalMap, texCoords).xyz;
		normal = 2.0 * normal - 1.0;
		normal = normalize(v_TBN * normal);
	}
	else
	{
		normal = v_Normal;
	}

	o_FragmentPosition = vec4(v_FragmentPosition, 1.0);
	o_Normal = vec4(normal, 1.0);
	o_Albedo = albedo;
	o_MetallicRoughness = texture(u_MetallicRoughnessMap, texCoords);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	const float minLayers = 8.0;
	const float maxLayers = 32.0;

	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

	float layerDepth = 1.0 / numLayers;

	vec2 range = viewDir.xy * 0.1;
	vec2 texCoordsStep = range / numLayers; // * layerDepth;

	float currentLayerDepth = 0.0;
	vec2 currentTexCoords = texCoords;
	float currentDepth = texture(u_DisplacementMap, currentTexCoords).r;

	while (currentLayerDepth < currentDepth)
	{
		currentTexCoords -= texCoordsStep;
		currentDepth = texture(u_DisplacementMap, currentTexCoords).r;
		currentLayerDepth += layerDepth;
	}

	vec2 prevTexCoords = currentTexCoords + texCoordsStep;

	float currentDepthDifference = currentDepth - currentLayerDepth;
	float prevDepthDifference = texture(u_DisplacementMap, prevTexCoords).r - currentLayerDepth + layerDepth;

	float weight = currentDepthDifference / (currentDepthDifference - prevDepthDifference);
	vec2 interpolatedTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return interpolatedTexCoords;
}