#version 140

uniform mat4 principle_Model;

uniform vec3 principle_LightPosition;
uniform vec3 principle_LightColor;
uniform float principle_LightIntensity;
uniform float principle_Ambience;

in vec3 frag_Position;
in vec3 frag_Normal;
in vec4 frag_Color;

out vec4 out_Color;

void main()
{
	mat3 normal_matrix = transpose(inverse(mat3(principle_Model)));
	vec3 normal = normalize(normal_matrix * frag_Normal);

	vec3 light_normal = normalize(principle_LightPosition);
	vec3 ambient_color = principle_Ambience * frag_Color.rgb;
	float diffuse_coefficient = max(0.0, dot(normal, light_normal));
	vec3 light_color = principle_LightIntensity * principle_LightColor;
	vec3 diffuse_color = diffuse_coefficient * frag_Color.rgb;
	diffuse_color *= light_color;

	out_Color = vec4(diffuse_color + ambient_color, frag_Color.a);
}
