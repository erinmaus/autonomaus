#version 140

uniform mat4 principle_View;
uniform mat4 principle_Projection;

in vec2 vert_Position;
in vec2 vert_Texture;

out vec2 frag_Texture;

void main()
{
	gl_Position =
		vec4(vert_Position, 0.0, 1.0) *
		principle_View *
		principle_Projection;

	frag_Texture = vert_Texture;
}
