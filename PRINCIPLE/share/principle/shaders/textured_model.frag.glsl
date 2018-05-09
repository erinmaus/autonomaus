#version 140

uniform sampler2D principle_Texture;

in vec4 frag_Color;
in vec2 frag_Texture;
in vec4 frag_TextureMeta;

out vec4 out_Color;

vec4 compute_atlas_varyings(vec2 texture_coordinate, vec4 texture_meta)
{
	vec4 result;
	result.p = texture_meta.s;
	result.q = texture_meta.t;
	result.s = texture_coordinate.s;
	result.t = texture_coordinate.t;
	return result;
}

vec2 compute_texture_coordinates(vec2 texture_coordinate, vec4 texture_meta)
{
	vec4 v = compute_atlas_varyings(texture_coordinate, texture_meta);
	vec4 d = vec4(256.0, 16.0, 1.0 / 4096.0, 4.0);;
	float m = v.p;
	float o = v.q;
	float p = d.s;
	float l = d.t;
	float u = d.p;
	m = floor(m + 0.5f);
	float q = floor(m / p);
	float L = m - p * q;
	float a = v.s;
	float C = v.t;
	float N = 0.5f / o;

	const vec2 i = vec2(1.0f);

	vec2 c;
	// LOOKUP_MODE_REPEAT
	c = mod(vec2(a, C), i);
	// LOOKUP_MODE_CLAMP
	//c = clamp(vec2(a, C), vec2(N), i - vec2(N));

	c = vec2(L, q) * l + c * o;
	c *= u;

	return c;
}

void main()
{
	vec2 texture_coordinate = compute_texture_coordinates(frag_Texture, frag_TextureMeta);
	vec4 sample = texture(principle_Texture, texture_coordinate);
	if (sample.a < 1.0 / 255.0)
	{
		discard;
	}

	out_Color = frag_Color * vec4(sample.rgb, 1);
}
