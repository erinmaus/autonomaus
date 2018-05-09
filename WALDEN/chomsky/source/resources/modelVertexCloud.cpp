// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <map>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "chomsky/resources/modelVertexCloud.hpp"
#include "chomsky/resources/silhouette.hpp"
#include "kompvter/graphics/vertexFetcher.hpp"
#include "kvre/profile/profile.hpp"
#include "kvre/profile/profile.hpp"

void chomsky::ModelVertexCloud::add(const Vertex& vertex)
{
	if (empty())
	{
		this->bounds.min = vertex.position;
		this->bounds.max = vertex.position;
	}
	else
	{
		this->bounds.expand(vertex.position);
	}

	this->vertices.push_back(vertex);
}

std::size_t chomsky::ModelVertexCloud::project(
	Silhouette& silhoutte,
	const glm::vec3& translation,
	const kvlc::Bounds& clip,
	int blend_mode,
	const std::function<float (const glm::vec3&, const Vertex&)>& process) const
{
	std::size_t num_points = 0;

	glm::vec3 one_over_dimensions;
	{
		auto clip_dimensions = clip.compute_size();

		if (clip_dimensions.x > 0)
		{
			one_over_dimensions.x = 1.0f / clip_dimensions.x;
		}
		else
		{
			one_over_dimensions.x = 0.0f;
		}

		if (clip_dimensions.y > 0)
		{
			one_over_dimensions.y = 1.0f / clip_dimensions.y;
		}
		else
		{
			one_over_dimensions.y = 0.0f;
		}

		if (clip_dimensions.z > 0)
		{
			one_over_dimensions.z = 1.0f / clip_dimensions.z;
		}
		else
		{
			one_over_dimensions.z = 0.0f;
		}
	}

	auto silhoutte_dimensions = glm::vec3(
		silhoutte.get_width() - 1,
		silhoutte.get_height() - 1,
		silhoutte.get_depth() - 1);
	for (auto& vertex: this->vertices)
	{
		auto position = vertex.position + translation;
		if (!clip.contains(position))
		{
			continue;
		}

		// Project the vertex position to silhouette space.
		auto position_clip_relative = (position - clip.min);
		position_clip_relative *= one_over_dimensions;

		auto fragment_position = position_clip_relative * silhoutte_dimensions;
		float source_fragment = process(fragment_position, vertex);
		float destination_fragment = silhoutte.get_fragment(
			fragment_position.x, fragment_position.y, fragment_position.z);

		float blended_fragment;
		switch (blend_mode)
		{
			case blend_mode_additive:
				blended_fragment = source_fragment + destination_fragment;
				break;
			case blend_mode_subtractive:
				blended_fragment = source_fragment - destination_fragment;
				break;
			case blend_mode_multiplicative:
				blended_fragment = source_fragment * destination_fragment;
				break;
			case blend_mode_min:
				blended_fragment = glm::min(source_fragment, destination_fragment);
				break;
			case blend_mode_max:
				blended_fragment = glm::max(source_fragment, destination_fragment);
				break;
			default:
				assert(false);
			case blend_mode_overwrite:
				blended_fragment = source_fragment;
				break;
		}

		++num_points;
		silhoutte.set_fragment(
			fragment_position.x, fragment_position.y, fragment_position.z,
			blended_fragment);
	}

	return num_points;
}

std::size_t chomsky::ModelVertexCloud::project_weight(
	chomsky::Silhouette& silhoutte,
	const glm::vec3& translation,
	const kvlc::Bounds& clip) const
{
	return project(
		silhoutte,
		translation, clip,
		blend_mode_additive,
		[](const glm::vec3&, const Vertex& vertex)
		{
			return 1.0f;
		});
}

void chomsky::ModelVertexCloud::project_color_component(
	Silhouette& silhoutte,
	const glm::vec3& translation,
	const kvlc::Bounds& clip,
	int component_index) const
{
	assert(component_index >= 0);
	assert(component_index < color_component_max);

	project(
		silhoutte,
		translation, clip,
		blend_mode_additive,
		[component_index](const glm::vec3&, const Vertex& vertex)
		{
			return glm::value_ptr(vertex.color)[component_index];
		});
}

chomsky::ModelVertexCloud::iterator chomsky::ModelVertexCloud::begin()
{
	return this->vertices.begin();
}

chomsky::ModelVertexCloud::const_iterator chomsky::ModelVertexCloud::begin() const
{
	return this->vertices.begin();
}

chomsky::ModelVertexCloud::iterator chomsky::ModelVertexCloud::end()
{
	return this->vertices.end();
}

chomsky::ModelVertexCloud::const_iterator chomsky::ModelVertexCloud::end() const
{
	return this->vertices.end();
}

bool chomsky::ModelVertexCloud::empty() const
{
	return this->vertices.empty();
}

std::size_t chomsky::ModelVertexCloud::size() const
{
	return this->vertices.size();
}

void chomsky::ModelVertexCloud::clear()
{
	this->vertices.clear();
}

const chomsky::ModelVertexCloud::Vertex& chomsky::ModelVertexCloud::at(
	std::size_t index) const
{
	return this->vertices.at(index);
}

kvlc::Bounds chomsky::ModelVertexCloud::get_bounds() const
{
	return this->bounds;
}

kvlc::Bounds chomsky::ModelVertexCloud::get_transformed_bounds(
	const glm::mat4& model) const
{
	if (empty())
	{
		return kvlc::Bounds();
	}

	kvlc::Bounds bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	for (auto& vertex: vertices)
	{
		glm::vec4 p = model * glm::vec4(vertex.position, 1.0f);
		bounds.expand(glm::vec3(p.x, p.y, p.z));
	}

	return bounds;
}
