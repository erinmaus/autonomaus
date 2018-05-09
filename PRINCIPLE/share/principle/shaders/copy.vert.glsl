#version 140

in vec2 vert_Position;
in vec2 vert_Texture;

out vec2 frag_Texture;

void main()
{
	gl_Position = vec4(vert_Position, -1.0, 1.0);

	frag_Texture = vert_Texture;
}
