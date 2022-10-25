#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_ViewProjectionMatrix;

void main()
{
	v_TexCoords = a_Position;

	gl_Position = u_ViewProjectionMatrix * vec4(a_Position, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

in vec3 v_TexCoords;

uniform samplerCube u_Skybox;

void main()
{
	o_Color = textureLod(u_Skybox, v_TexCoords, 0);
	// o_Color = texture(u_Skybox, v_TexCoords);
}