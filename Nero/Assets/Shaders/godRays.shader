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

#define NR_DIRECTIONAL_LIGHTS 6

uniform int u_NumLights;
uniform vec3 u_LightDirections[NR_DIRECTIONAL_LIGHTS];

uniform mat4 u_ViewProjectionMatrix;

uniform sampler2D u_OcclusionMap;

const float density = 1.0;
const float weight = 0.1;
const float decay = 1.0;
const float exposure = 1.0;
const int numSamples = 100;

vec3 GetOcclusion(vec2 texCoords);

void main()
{
	vec3 result;

	for (int i = 0; i < u_NumLights; i++)
	{
		vec3 lightDirection = u_LightDirections[i];

		vec4 lightScreenSpace = u_ViewProjectionMatrix * vec4(lightDirection, 1.0);
		lightScreenSpace.xy /= lightScreenSpace.w;
		lightScreenSpace = lightScreenSpace * 0.5 + 0.5;

		vec2 screenSpaceDirection = -lightScreenSpace.xy;
		screenSpaceDirection *= (1.0 / float(numSamples)) * density;
		
		vec2 texCoords = v_TexCoords;
		float currentDecay = 1.0;

		for (int i = 0; i < numSamples; i++)
		{
			texCoords += screenSpaceDirection;

			vec3 occlusion = GetOcclusion(texCoords);
			result += occlusion * currentDecay * weight;

			currentDecay *= decay;
		}
	}

	o_Color = vec4(result * exposure, 1.0);
}

vec3 GetOcclusion(vec2 texCoords)
{
	return vec3(texture2D(u_OcclusionMap, texCoords).r);
}