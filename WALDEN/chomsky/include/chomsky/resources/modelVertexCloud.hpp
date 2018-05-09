// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_MODEL_VERTEX_CLOUD_HPP
#define CHOMSKY_RESOURCES_MODEL_VERTEX_CLOUD_HPP

#include <cstddef>
#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <glm/glm.hpp>
#include "kvre/model/model.hpp"
#include "kvlc/geometry/bounds.hpp"

namespace chomsky
{
	class Silhouette;

	class ModelVertexCloud
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
		};

		typedef std::vector<Vertex>::iterator iterator;
		typedef std::vector<Vertex>::const_iterator const_iterator;

		ModelVertexCloud() = default;
		~ModelVertexCloud() = default;

		template <typename IndexIter>
		void extract(
			IndexIter iter_begin, IndexIter iter_end,
			const kvre::Model& model);

		void add(const Vertex& vertex);

		enum
		{
			blend_mode_additive = 0,
			blend_mode_subtractive,
			blend_mode_multiplicative,
			blend_mode_min,
			blend_mode_max,
			blend_mode_overwrite
		};
		std::size_t project(
			Silhouette& silhoutte,
			const glm::vec3& translation,
			const kvlc::Bounds& clip,
			int blend_mode,
			const std::function<float(const glm::vec3& position, const Vertex& vertex)>& process) const;

		std::size_t project_weight(
			Silhouette& silhoutte,
			const glm::vec3& translation,
			const kvlc::Bounds& clip) const;

		enum
		{
			color_component_red = 0,
			color_component_green = 1,
			color_component_blue = 2,
			color_component_max
		};
		void project_color_component(
			Silhouette& silhoutte,
			const glm::vec3& translation,
			const kvlc::Bounds& clip,
			int component_index) const;

		iterator begin();
		const_iterator begin() const;

		iterator end();
		const_iterator end() const;

		bool empty() const;
		std::size_t size() const;

		const Vertex& at(std::size_t index) const;
		void clear();

		kvlc::Bounds get_bounds() const;
		kvlc::Bounds get_transformed_bounds(const glm::mat4& model) const;

	private:
		std::vector<Vertex> vertices;
		kvlc::Bounds bounds;
	};
}

template <typename IndexIter>
void chomsky::ModelVertexCloud::extract(
	IndexIter iter_begin, IndexIter iter_end,
	const kvre::Model& model)
{
	for (auto& i = iter_begin; i != iter_end; ++i)
	{
		auto index = *i;
		Vertex vertex;
		vertex.position = model.get_position(index);
		vertex.color = glm::vec3(model.get_color(index));
		add(vertex);
	}
}

#endif
