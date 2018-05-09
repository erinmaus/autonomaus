// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_MODEL_PICKER_HPP
#define PRINCIPLE_RESOURCES_MODEL_PICKER_HPP

#include <map>
#include <set>
#include <vector>
#include "chomsky/world/worldUpdatePass.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvlc/geometry/ray.hpp"
#include "kvlc/collision/scene.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

namespace principle
{
	class ModelPicker : public chomsky::WorldUpdatePass
	{
	public:
		ModelPicker() = default;
		~ModelPicker() = default;

		std::vector<int> pick(
			const glm::mat4& view,
			const glm::mat4& projection,
			const glm::vec4& viewport,
			const glm::vec2& mouse);
		std::vector<int> pick(
			const glm::mat4& view,
			const glm::mat4& projection,
			const glm::vec4& viewport,
			const glm::vec2& mouse,
			std::map<int, glm::vec3>& positions);
		std::vector<int> pick(
			const kvlc::Ray& ray, float length,
			std::map<int, glm::vec3>& positions);
		std::vector<int> pick(
			const kvlc::Bounds& bounds);

		void draw_model(
			int tag, const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;
		void draw_terrain(
			int tag, const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;
		void draw_water(
			int tag, const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;

	private:
		std::vector<glm::vec3> transform_skinned_model(
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event);
		std::vector<glm::vec3> transform_unskinned_model(
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event);
		std::vector<glm::vec3> transform_terrain(
			const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event);
		void add_model(
			int tag,
			const std::vector<glm::vec3>& points,
			const kompvter::ModelDrawEvent& event);

		float scale = 1 / 2048.0f;

		kvlc::Scene collision_scene;
		std::map<int /* draw_tag */, int /* scene_tag */> tags;
		std::map<int /* scene_tag */, int /* draw_tag */> scene_tags;
	};
}

#endif
