#version 140

uniform vec3 principle_MinBounds;
uniform vec3 principle_MaxBounds;

in vec3 frag_Position;
in vec4 frag_Color;

out vec4 out_Color;

void main()
{
	if (frag_Position.x < principle_MinBounds.x ||
		frag_Position.x > principle_MaxBounds.x)
	{
		discard;
	}

	if (frag_Position.y < principle_MinBounds.y ||
		frag_Position.y > principle_MaxBounds.y)
	{
		discard;
	}

	if (frag_Position.z < principle_MinBounds.z ||
		frag_Position.z > principle_MaxBounds.z)
	{
		discard;
	}

	out_Color = frag_Color;
}
