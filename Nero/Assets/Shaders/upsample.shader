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

layout(location = 0) out vec3 o_Upsampled;

in vec2 v_TexCoords;

uniform sampler2D u_Image;
uniform float u_FilterRadius;

void main()
{
    float x = u_FilterRadius;
    float y = u_FilterRadius;

    vec3 a = texture(u_Image, vec2(v_TexCoords.x - x, v_TexCoords.y + y)).rgb;
    vec3 b = texture(u_Image, vec2(v_TexCoords.x,     v_TexCoords.y + y)).rgb;
    vec3 c = texture(u_Image, vec2(v_TexCoords.x + x, v_TexCoords.y + y)).rgb;

    vec3 d = texture(u_Image, vec2(v_TexCoords.x - x, v_TexCoords.y)).rgb;
    vec3 e = texture(u_Image, vec2(v_TexCoords.x,     v_TexCoords.y)).rgb;
    vec3 f = texture(u_Image, vec2(v_TexCoords.x + x, v_TexCoords.y)).rgb;

    vec3 g = texture(u_Image, vec2(v_TexCoords.x - x, v_TexCoords.y - y)).rgb;
    vec3 h = texture(u_Image, vec2(v_TexCoords.x,     v_TexCoords.y - y)).rgb;
    vec3 i = texture(u_Image, vec2(v_TexCoords.x + x, v_TexCoords.y - y)).rgb;

    o_Upsampled = e * 4.0;
    o_Upsampled += (b + d + f + g) * 2.0;
    o_Upsampled += (a + c + g + i);
    o_Upsampled *= 0.0625; // = 1/16
}