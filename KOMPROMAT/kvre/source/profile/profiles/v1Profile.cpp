#include "kvre/profile/profiles/v1Profile.hpp"
#include "kvre/model/model.hpp"

const kvre::Profile::AtlasMeta* kvre::V1Profile::get_default_atlas_meta() const
{
	return &this->default_atlas_meta;
}

std::size_t kvre::V1Profile::get_num_model_vertices(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aVertexPosition_BoneLabel");
	assert(fetcher != nullptr);

	return fetcher->get_num_vertices();
}

static glm::vec4 compute_atlas_varyings(
	const glm::vec2& texture_coordinate,
	const glm::vec4& texture_atlas)
{
	glm::vec4 result;
	result.p = texture_atlas.s;
	result.q = texture_atlas.t;
	result.s = texture_coordinate.s;
	result.t = texture_coordinate.t;
	return result;
}

// XXX: De-obfuscate.
glm::vec2 kvre::V1Profile::compute_texture_coordinate_to_atlas(
	const glm::vec2& texture_coordinate, const glm::vec4& texture_meta,
	const AtlasMeta* atlas_meta) const
{
	glm::vec4 v = compute_atlas_varyings(texture_coordinate, texture_meta);
	glm::vec4 d = ((const V1AtlasMeta*)atlas_meta)->uniform;
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

	// LOOKUP_MODE_CLAMP
	static const glm::vec2 i = glm::vec2(1.0f);
	glm::vec2 c = clamp(glm::vec2(a, C), glm::vec2(N), i - glm::vec2(N));

	c = glm::vec2(L, q) * l + c * o;
	c *= u;

	return c;
}

glm::vec3 kvre::V1Profile::get_model_vertex_position(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec3(element.x, element.y, element.z);
	}

	return glm::vec3(0.0f);
}

int kvre::V1Profile::get_model_vertex_bone(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index) const
{
	kompvter::UnsignedVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return element.w & 0xff;
	}

	return 0;
}

glm::vec4 kvre::V1Profile::get_model_vertex_color(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec4(element.x, element.y, element.z, element.w);
	}

	return glm::vec4(0.0f);
}

glm::vec2 kvre::V1Profile::get_model_vertex_texture_coordinate(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec2(element.x, element.y);
	}

	return glm::vec2(0.0f);
}

glm::vec4 kvre::V1Profile::get_model_vertex_texture_meta(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec4(element.x, element.y, element.z, element.w);
	}

	return glm::vec4(0.0f);
}

glm::vec3 kvre::V1Profile::get_model_vertex_normal(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec3(element.x, element.y, element.z);
	}

	return glm::vec3(0.0f);
}

const kompvter::VertexFetcher* kvre::V1Profile::get_model_vertex_positions_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aVertexPosition_BoneLabel");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_model_vertex_bones_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aVertexPosition_BoneLabel");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_model_vertex_colors_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aVertexColourUnwhitened");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_model_vertex_texture_coordinates_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aTextureUV");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_model_vertex_texture_metas_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aTextureMeta");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_model_vertex_normals_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aVertexNormal_BatchFlags");
	return fetcher;
}

std::size_t kvre::V1Profile::get_num_terrain_vertices(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = get_terrain_vertex_positions_fetcher(attributes);
	assert(fetcher != nullptr);

	return fetcher->get_num_vertices();
}

glm::vec3 kvre::V1Profile::get_terrain_vertex_position(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec3(element.x, element.y, element.z);
	}

	return glm::vec3(0.0f);
}

glm::vec4 kvre::V1Profile::get_terrain_vertex_color(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec4(element.x, element.y, element.z, element.w);
	}

	return glm::vec4(0.0f);
}

const kompvter::VertexFetcher* kvre::V1Profile::get_terrain_vertex_positions_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	if (attributes.has("aVertexPosition"))
	{
		return attributes.get("aVertexPosition");
	}
	else if (attributes.has("aVertexPosition_Spec"))
	{
		return attributes.get("aVertexPosition_Spec");
	}
	else if (attributes.has("aWaterPositionAndDepth"))
	{
		return attributes.get("aWaterPositionAndDepth");
	}

	return nullptr;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_terrain_vertex_colors_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aVertexColour");
	return fetcher;
}

std::size_t kvre::V1Profile::get_num_particle_vertices(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = get_particle_vertex_positions_fetcher(attributes);
	assert(fetcher != nullptr);

	return fetcher->get_num_vertices();
}

bool kvre::V1Profile::get_is_billboard(
	const kompvter::VertexAttributes& attributes) const
{
	return attributes.has("aBillboardMeta");
}

glm::vec3 kvre::V1Profile::get_particle_vertex_position(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	bool is_billboard) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec3(element.x, element.y, element.z);
	}

	return glm::vec3(0.0f);
}

glm::vec4 kvre::V1Profile::get_particle_vertex_color(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	bool is_billboard) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec4(element.x, element.y, element.z, element.w);
	}

	return glm::vec4(0.0f);
}

glm::vec2 kvre::V1Profile::get_particle_vertex_texture_coordinate(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	bool is_billboard) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		if (is_billboard)
		{
			return glm::vec2(element.x, element.y);
		}
		else
		{
			// 'x' is rotation
			return glm::vec2(element.y, element.z);
		}
	}

	return glm::vec2(0.0f);
}

glm::vec4 kvre::V1Profile::get_particle_vertex_texture_meta(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	bool is_billboard) const
{
	kompvter::FloatingPointVertexElement element;
	if (fetcher.fetch(index, element))
	{
		return glm::vec4(element.x, element.y, element.z, element.w);
	}

	return glm::vec4(0.0f);
}

const kompvter::VertexFetcher* kvre::V1Profile::get_particle_vertex_positions_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	if (attributes.has("aRotation_UV"))
	{
		return attributes.get("aRotation_UV");
	}
	else if (attributes.has("aVertexPositionDepthOffset"))
	{
		return attributes.get("aVertexPositionDepthOffset");
	}

	return nullptr;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_particle_vertex_colors_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	return attributes.get("aVertexColour");
}

const kompvter::VertexFetcher* kvre::V1Profile::get_particle_vertex_texture_coordinates_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	if (attributes.has("aRotation_UV"))
	{
		return attributes.get("aRotation_UV");
	}
	else if (attributes.has("aBillboardUV"))
	{
		return attributes.get("aBillboardUV");
	}

	return nullptr;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_particle_vertex_texture_metas_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	if (attributes.has("aParticleTextureMeta"))
	{
		return attributes.get("aParticleTextureMeta");
	}
	else if (attributes.has("aBillboardMeta"))
	{
		return attributes.get("aBillboardMeta");
	}

	return nullptr;
}

std::size_t kvre::V1Profile::get_num_flat_vertices(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = get_flat_vertex_positions_fetcher(attributes);
	assert(fetcher != nullptr);

	return fetcher->get_num_vertices();
}

void kvre::V1Profile::get_flat_vertex_positions(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	std::size_t count,
	glm::vec2* output_buffer,
	std::size_t output_stride) const
{
	fetcher.extract(index, count, &output_buffer->x, 2, output_stride);
}

void kvre::V1Profile::get_flat_vertex_colors(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	std::size_t count,
	glm::vec4* output_buffer,
	std::size_t output_stride) const
{
	fetcher.extract(index, count, &output_buffer->x, 4, output_stride);
	for (int i = 0; i < count; ++i)
	{
		// I guess it's a packed uint32_t internally. Endian-swap necessary.
		// The vertex shader has the same logic.
		std::swap(output_buffer->w, output_buffer->x);
		std::swap(output_buffer->z, output_buffer->y);
		output_buffer = (glm::vec4*)(((std::uint8_t*)output_buffer) + output_stride);
	}
}

void kvre::V1Profile::get_flat_vertex_texture_coordinates(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	std::size_t count,
	glm::vec2* output_buffer,
	std::size_t output_stride) const
{
	fetcher.extract(index, count, &output_buffer->x, 2, output_stride);
}

void kvre::V1Profile::get_flat_vertex_atlas_mins(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	std::size_t count,
	glm::vec2* output_buffer,
	std::size_t output_stride) const
{
	fetcher.extract(index, count, &output_buffer->x, 2, output_stride);
}

void kvre::V1Profile::get_flat_vertex_atlas_maxes(
	const kompvter::VertexFetcher& fetcher,
	std::size_t index,
	std::size_t count,
	glm::vec2* output_buffer,
	std::size_t output_stride) const
{
	fetcher.extract(index, count, &output_buffer->x, 2, output_stride);
}


const kompvter::VertexFetcher* kvre::V1Profile::get_flat_vertex_positions_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aVertexPosition2D");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_flat_vertex_colors_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aVertexColour");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_flat_vertex_texture_coordinates_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aTextureUV");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_flat_vertex_atlas_mins_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aTextureUVAtlasMin");
	return fetcher;
}

const kompvter::VertexFetcher* kvre::V1Profile::get_flat_vertex_atlas_maxes_fetcher(
	const kompvter::VertexAttributes& attributes) const
{
	auto fetcher = attributes.get("aTextureUVAtlasMax");
	return fetcher;
}

bool kvre::V1Profile::is_flat_vertex_textured(const glm::vec2& texture_coordinate) const
{
	static const float E = -60000.0f;
	return texture_coordinate.x >= E;
}
