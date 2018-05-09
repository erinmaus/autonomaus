#version 140

uniform mat4 principle_Projection;
uniform mat4 principle_View;
uniform mat4 principle_Model;
uniform vec4 principle_BoneTransforms[168];

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
out vec4 frag_TextureMeta;
out vec4 frag_Color;

void main()
{
	int bone = int(vert_Bone) * 3;
	mat4 bone_transform = mat4(
		principle_BoneTransforms[bone].x,
		principle_BoneTransforms[bone].y,
		principle_BoneTransforms[bone].z,
		0.0,
		principle_BoneTransforms[bone].w,
		principle_BoneTransforms[bone + 1].x,
		principle_BoneTransforms[bone + 1].y,
		0.0,
		principle_BoneTransforms[bone + 1].z,
		principle_BoneTransforms[bone + 1].w,
		principle_BoneTransforms[bone + 2].x,
		0.0,
		principle_BoneTransforms[bone + 2].y,
		principle_BoneTransforms[bone + 2].z,
		principle_BoneTransforms[bone + 2].w,
		1.0);
	//mat4 bone_model_transform = transpose(bone_transform) * principle_Model;
	mat4 bone_model_transform = principle_Model;

	vec4 world_position =
		vec4(vert_Position, 1.0) *
		bone_model_transform;
	gl_Position =
		world_position *
		principle_View * 
		principle_Projection;

	frag_Position = world_position.xyz;
	frag_Normal = vert_Normal;
	frag_Texture = vert_TextureCoordinate;
	frag_TextureMeta = vert_TextureMeta;
	frag_Color = vert_Color;
}
