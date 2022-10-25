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

uniform sampler2D u_Image;

void main()
{
	o_Color = texture(u_Image, v_TexCoords);
}