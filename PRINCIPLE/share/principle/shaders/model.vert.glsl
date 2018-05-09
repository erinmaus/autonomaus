#version 140

uniform mat4 principle_Projection;
uniform mat4 principle_View;
uniform mat4 principle_Model;

in vec3 vert_Position;
in float vert_Bone;
in vec3 vert_Normal;
in vec2 vert_TextureCoordinate;
in vec4 vert_TextureMeta;
in vec4 vert_AtlasMeta;
in vec4 vert_Color;

out vec3 frag_Position;
out vec3 frag_Normal;
out vec2 frag_Texture;
out vec4 frag_Color;

void main()
{
	vec4 world_position = 
		vec4(vert_Position, 1.0) *
		principle_Model;
	gl_Position =
		world_position *
		principle_View *
		principle_Projection;

	frag_Position = world_position.xyz;
	frag_Normal = vert_Normal;
	frag_Texture = vert_TextureCoordinate;
	frag_Color = vert_Color;
}
