#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
}


#type geometry
#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

out vec4 v_FragmentPosition;

uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ViewMatrices[6];

void main()
{
	for (int face = 0; face < 6; face++)
	{
		gl_Layer = face;

		for (int i = 0; i < 3; i++)
		{
			v_FragmentPosition = gl_in[i].gl_Position;
			gl_Position = u_ProjectionMatrix * u_ViewMatrices[face] * v_FragmentPosition;
			EmitVertex();
		}

		EndPrimitive();
	}
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

in vec4 v_FragmentPosition;

uniform samplerCube u_Skybox;
uniform float u_Roughness;

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint count);
vec3 ImportanceSampleGGX(vec2 hammersley, vec3 N, float roughness);
float DistributionGGX(float nDotH, float roughness);

const float PI = 3.14159265359;

void main()
{
    vec3 normal = normalize(vec3(v_FragmentPosition));
    vec3 viewDirection = normal;

    float totalWeight = 0.0;
    vec3 prefilteredColor = vec3(0.0);

    const uint SAMPLE_COUNT = 1024u;
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        vec2 hammersley = Hammersley(i, SAMPLE_COUNT);
        vec3 halfwayVector = ImportanceSampleGGX(hammersley, normal, u_Roughness);
        vec3 lightDirection = normalize(2.0 * dot(viewDirection, halfwayVector) * halfwayVector - viewDirection);

        float nDotL = max(dot(normal, lightDirection), 0.0);

        if (nDotL > 0.0)
        {
            float hDotV = max(dot(halfwayVector, viewDirection), 0.0);
            float nDotH = max(dot(normal, halfwayVector), 0.0);

            float geometry = DistributionGGX(nDotH, u_Roughness);

            float pdf = geometry * nDotH / (4.0 * hDotV) + 0.0001;

            float resolution = 512.0;
            float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = u_Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            prefilteredColor += textureLod(u_Skybox, lightDirection, mipLevel).rgb;
            totalWeight += nDotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    o_Color = vec4(prefilteredColor, 1.0);
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