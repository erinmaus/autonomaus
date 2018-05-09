#version 140

uniform mat4 principle_Projection;
uniform mat4 principle_View;
uniform mat4 principle_Model;

in vec3 vert_Position;
in vec4 vert_Color;

out vec4 frag_Color;

void main()
{
	gl_Position =
		vec4(vert_Position, 1.0) *
		principle_Model *
		principle_View *
		principle_Projection;

	frag_Color = vert_Color;
}
