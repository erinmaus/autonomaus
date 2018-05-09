#version 140

uniform vec4 principle_Tint;

in vec4 frag_Color;

out vec4 out_Color;

void main()
{
	out_Color = principle_Tint;
}
