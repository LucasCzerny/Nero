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

layout(location = 0) out float o_Color;

in vec2 v_TexCoords;

uniform vec3 u_SsaoKernel[64];

uniform mat4 u_ViewProjectionMatrix;

uniform float u_ScreenWidth;
uniform float u_ScreenHeight;

uniform sampler2D u_FragmentPosition;
uniform sampler2D u_Normal;
uniform sampler2D u_NoiseTexture;

// RADIUS PARAMETER
// KERNEL SIZE PARAMETER

void main()
{
	vec2 noiseScale = vec2(u_ScreenWidth / 4.0, u_ScreenHeight / 4.0);

	vec3 position = texture(u_FragmentPosition, v_TexCoords).xyz;
	vec3 normal = texture(u_Normal, v_TexCoords).xyz;
	vec3 noiseVec = texture(u_NoiseTexture, v_TexCoords * noiseScale).xyz;

	vec3 tangent = normalize(noiseVec - normal * dot(noiseVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	vec3 positionScreenSpace = vec3(u_ViewProjectionMatrix * vec4(position, 1.0));;

	const int kernelSize = 64;
	const float radius = 0.5;
	const float bias = 0.01;

	float occlusion = 0.0;

	for (int i = 0; i < kernelSize; i++)
	{
		vec3 sampleVec = TBN * u_SsaoKernel[i]; // from tangent to world space

		vec3 samplePos = position + sampleVec * radius;

		vec4 sampleCoords = vec4(samplePos, 1.0);
		sampleCoords = u_ViewProjectionMatrix * sampleCoords;
		sampleCoords.xyz /= sampleCoords.w;
		sampleCoords.xyz = sampleCoords.xyz * 0.5 + 0.5;

		samplePos = vec3(u_ViewProjectionMatrix * vec4(samplePos, 1.0));

		vec3 firstFragment = texture(u_FragmentPosition, sampleCoords.xy).xyz;
		firstFragment = vec3(u_ViewProjectionMatrix * vec4(firstFragment, 1.0));
		float sampleDepth = firstFragment.z;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(positionScreenSpace.z - sampleDepth));

		occlusion += ((sampleDepth >= samplePos.z + bias) ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / kernelSize);
	o_Color = occlusion;
}