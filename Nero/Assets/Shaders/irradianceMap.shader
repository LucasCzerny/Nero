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

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(vec3(v_FragmentPosition));

	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));

	float sampleDelta = 0.025;
	float numberOfSamples = 0.0;

	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			vec3 tangentSample = vec3(cos(theta) * cos(phi), cos(theta) * sin(phi), sin(theta));
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(u_Skybox, sampleVec).rgb * cos(theta) * sin(theta);
			numberOfSamples++;
		}
	}

	irradiance *= PI;
	irradiance /= numberOfSamples;
	
	o_Color = vec4(irradiance, 1.0);
}
