#version 140

uniform sampler2D principle_Texture;

in vec2 frag_Texture;

out vec4 out_Color;

void main()
{
	vec4 sample = texture(principle_Texture, frag_Texture);
	out_Color = sample;
}
