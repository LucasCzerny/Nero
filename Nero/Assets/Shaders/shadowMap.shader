#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_LightSpaceTransform;
uniform mat4 u_ModelMatrix;

void main()
{
	v_TexCoords = a_TexCoords;

	gl_Position = u_LightSpaceTransform * u_ModelMatrix * vec4(a_Position, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoords;

uniform sampler2D u_AlbedoMap;

void main()
{
	o_Color = texture(u_AlbedoMap, v_TexCoords);
}