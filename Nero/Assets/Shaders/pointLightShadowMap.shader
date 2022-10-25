#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_ModelMatrix;

void main()
{
	v_TexCoords = a_TexCoords;

	gl_Position = u_ModelMatrix * vec4(a_Position, 1.0);
}


#type geometry
#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

in vec2 v_TexCoords[];

out vec2 v_GeoTexCoords;
out vec4 v_FragmentPosition;

uniform mat4 u_LightSpaceTransforms[6];

void main()
{
	for (int face = 0; face < 6; face++)
	{
		gl_Layer = face;

		for (int i = 0; i < 3; i++)
		{
			v_GeoTexCoords = v_TexCoords[i];

			v_FragmentPosition = gl_in[i].gl_Position;
			gl_Position = u_LightSpaceTransforms[face] * v_FragmentPosition;
			EmitVertex();
		}

		EndPrimitive();
	}
}


#type fragment
#version 450

// TRANSPARENT STUFF
// ANDREI DESPINOIU

layout(location = 0) out vec4 o_Color;

in vec4 v_FragmentPosition;
in vec2 v_GeoTexCoords;

uniform vec3 u_LightPosition;
uniform float u_FarPlane;

uniform sampler2D u_AlbedoMap;
 
void main()
{
	float lightDistance = length(v_FragmentPosition.xyz - u_LightPosition);
	lightDistance = lightDistance / u_FarPlane;
	gl_FragDepth = lightDistance;
	
	o_Color = texture(u_AlbedoMap, v_GeoTexCoords);
}