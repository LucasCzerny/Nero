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

layout(location = 0) out vec3 o_Downsampled;

in vec2 v_TexCoords;

uniform sampler2D u_Image;
uniform vec2 u_ImageResolution;

void main()
{
    vec2 texelSize = 1.0 / u_ImageResolution;
    float x = texelSize.x;
    float y = texelSize.y;

    vec3 a = texture(u_Image, vec2(v_TexCoords.x - 2 * x, v_TexCoords.y + 2 * y)).rgb;
    vec3 b = texture(u_Image, vec2(v_TexCoords.x,         v_TexCoords.y + 2 * y)).rgb;
    vec3 c = texture(u_Image, vec2(v_TexCoords.x + 2 * x, v_TexCoords.y + 2 * y)).rgb;

    vec3 d = texture(u_Image, vec2(v_TexCoords.x - 2 * x, v_TexCoords.y)).rgb;
    vec3 e = texture(u_Image, vec2(v_TexCoords.x,         v_TexCoords.y)).rgb;
    vec3 f = texture(u_Image, vec2(v_TexCoords.x + 2 * x, v_TexCoords.y)).rgb;

    vec3 g = texture(u_Image, vec2(v_TexCoords.x - 2 * x, v_TexCoords.y - 2 * y)).rgb;
    vec3 h = texture(u_Image, vec2(v_TexCoords.x,         v_TexCoords.y - 2 * y)).rgb;
    vec3 i = texture(u_Image, vec2(v_TexCoords.x + 2 * x, v_TexCoords.y - 2 * y)).rgb;

    vec3 j = texture(u_Image, vec2(v_TexCoords.x - x,     v_TexCoords.y + y)).rgb;
    vec3 k = texture(u_Image, vec2(v_TexCoords.x + x,     v_TexCoords.y + y)).rgb;
    vec3 l = texture(u_Image, vec2(v_TexCoords.x - x,     v_TexCoords.y - y)).rgb;
    vec3 m = texture(u_Image, vec2(v_TexCoords.x + x,     v_TexCoords.y - y)).rgb;

    o_Downsampled = e * 0.125;                  // max contribution: 0.125
    o_Downsampled += (a + c + g + i) * 0.03125; // max: 0.125
    o_Downsampled += (b + d + f + h) * 0.03125; // max: 0.25
    o_Downsampled += (j + k + l + m) * 0.03125; // max: 0.5
                                                // = max: 1.0
}