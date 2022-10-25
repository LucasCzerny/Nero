#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	gl_Position = u_ViewProjectionMatrix * vec4(a_Position, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4(0.5, 0.54, 0.83, 1.0);
}