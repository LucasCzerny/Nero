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

uniform bool u_UseGodRays;

uniform sampler2D u_Image;
uniform sampler2D u_Bloom;
uniform sampler2D u_GodRays;

void main()
{
	vec3 bloom = texture(u_Bloom, v_TexCoords).rgb;
	bloom *= 0.5;

	vec3 godRays = vec3(0.0);
	if (u_UseGodRays)
		godRays = texture(u_GodRays, v_TexCoords).rgb;

	vec3 postProcessing = bloom + godRays;

	// HDR
	postProcessing = postProcessing / (postProcessing + vec3(1.0));

	o_Color = vec4(texture(u_Image, v_TexCoords).rgb + postProcessing, 1.0);
}