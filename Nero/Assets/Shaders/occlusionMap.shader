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

layout(location = 0) out float o_Color;

in vec2 v_TexCoords;

uniform sampler2D u_Depth;

void main()
{
	vec2 texelSize = 1.0 / vec2(textureSize(u_Depth, 0));

	float result = 0.0;

	for (int x = -2; x < 2; x++)
	{
		for (int y = -2; y < 2; y++)
		{
			vec2 offset = 2.0 * vec2(float(x), float(y)) * texelSize;

			float depth = texture(u_Depth, v_TexCoords + offset).r;

			if (depth != 1.0)
				depth = 0.0;

			result += depth;
		}
	}

	o_Color = result / 25.0;
}