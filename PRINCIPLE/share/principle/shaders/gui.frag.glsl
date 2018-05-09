#version 140

uniform sampler2D principle_Texture;

in vec3 frag_Texture;
in vec4 frag_TextureMeta;
in vec4 frag_Color;

out vec4 out_Color;

void main()
{
	vec2 texture_coordinates = mod(frag_Texture.st - frag_TextureMeta.xy, frag_TextureMeta.zw);
	texture_coordinates += frag_TextureMeta.xy;

	vec4 sample = texture(principle_Texture, texture_coordinates);
	out_Color = frag_Color * mix(vec4(1.0), sample, frag_Texture.z);
}
