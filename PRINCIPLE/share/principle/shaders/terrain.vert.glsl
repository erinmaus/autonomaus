#version 140

uniform mat4 principle_Projection;
uniform mat4 principle_View;
uniform mat4 principle_Model;

in vec3 vert_Position;
in vec4 vert_Normal;
in vec4 vert_TextureCoordinate;
in vec4 vert_TextureMeta;
in vec4 vert_AtlasMeta;
in vec4 vert_Color;

out vec3 frag_Position;
out vec4 frag_TextureSlot;
out vec4 frag_TextureSize;
out vec4 frag_TextureScale;
out vec4 frag_TextureWeight;
out vec4 frag_Color;

void main()
{
	gl_Position =
		vec4(vert_Position, 1.0) *
		principle_Model *
		principle_View * 
		principle_Projection;

	frag_Position = vert_Position;
	frag_TextureSlot = vert_TextureCoordinate;
	frag_TextureSize = vec4(vert_TextureMeta.stp*16.0,1.0);
	frag_TextureScale = vert_AtlasMeta;
	frag_TextureWeight = vert_Normal;
	frag_Color = vert_Color;
}
