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

layout(location = 0) out vec2 o_Color;

in vec2 v_TexCoords;

vec2 IntegrateBRDF(float nDotV, float roughness);
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint count);
vec3 ImportanceSampleGGX(vec2 hammersley, vec3 normal, float roughness);
float GeometrySchlickGGX(float nDot, float roughness);
float GeometrySmith(float nDotV, float nDotL, float roughness);

const float PI = 3.14159265359;

void main()
{
	vec2 integratedBRDF = IntegrateBRDF(v_TexCoords.x, v_TexCoords.y);
	o_Color = integratedBRDF;
}

vec2 IntegrateBRDF(float nDotV, float roughness)
{
	vec3 viewDirection;
	viewDirection.x = sqrt(1.0 - nDotV * nDotV);
	viewDirection.y = 0.0;
	viewDirection.z = nDotV;

	float A = 0.0;
	float B = 0.0;

	vec3 normal = vec3(0.0, 0.0, 1.0);

	const uint SAMPLE_COUNT = 1024u;

	for (uint i = 0u; i < SAMPLE_COUNT; i++)
	{
		vec2 hammersley = Hammersley(i, SAMPLE_COUNT);
		vec3 halfwayVector = ImportanceSampleGGX(hammersley, normal, roughness);
		vec3 lightDirection = normalize(2.0 * dot(viewDirection, halfwayVector) * halfwayVector - viewDirection);

		float nDotL = max(lightDirection.z, 0.0);
		float nDotH = max(halfwayVector.z, 0.0);
		float vDotH = max(dot(viewDirection, halfwayVector), 0.0);

		if (nDotL > 0.0)
		{
			float G = GeometrySmith(nDotL, nDotV, roughness);
			float G_Vis = (G * vDotH) / (nDotH * nDotV);
			float Fc = pow(1.0 - vDotH, 5.0);

			A += (1.0 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	A /= float(SAMPLE_COUNT);
	B /= float(SAMPLE_COUNT);

	return vec2(A, B);
}

float RadicalInverse_VdC(uint bits)
{
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint count)
{
	return vec2(float(i) / float(count), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 hammersley, vec3 normal, float roughness)
{
	float a = roughness * roughness;

	float phi = 2.0 * PI * hammersley.x;
	float cosTheta = sqrt((1.0 - hammersley.y) / (1.0 + (a * a - 1.0) * hammersley.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, normal));
	vec3 bitangent = cross(normal, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + normal * H.z;
	return normalize(sampleVec);
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
				  2
	*/

	float a = roughness;
	float k = (a * a) / 2.0;

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

