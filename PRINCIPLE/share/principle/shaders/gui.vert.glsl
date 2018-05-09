#version 140

uniform mat4 principle_View;
uniform mat4 principle_Projection;

in vec2 vert_Position;
in vec3 vert_Texture;
in vec4 vert_TextureMeta;
in vec4 vert_Color;

out vec3 frag_Texture;
out vec4 frag_TextureMeta;
out vec4 frag_Color;

void main()
{
	gl_Position =
		vec4(vert_Position, 0.0, 1.0) *
		principle_View *
		principle_Projection;

	frag_Texture = vert_Texture;
	frag_TextureMeta = vert_TextureMeta;
	frag_Color = vert_Color;
}
