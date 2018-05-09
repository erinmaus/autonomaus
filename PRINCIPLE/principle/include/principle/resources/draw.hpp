// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_DRAW_HPP
#define PRINCIPLE_RESOURCES_DRAW_HPP

#include <glm/glm.hpp>
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/graphics/drawParameters.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/worldCoordinate.hpp"
#include "principle/scene/modelNode.hpp"

namespace principle
{
	class Draw
	{
	public:
		enum
		{
			draw_type_none = 0,
			draw_type_terrain,
			draw_type_water,
			draw_type_particles,
			draw_type_scenery,
			draw_type_prop,
			draw_type_actor,
			draw_type_unclassified,
			draw_type_max
		};

		Draw(
			std::size_t call_num,
			int tag,
			int draw_type,
			const kompvter::ModelDrawEvent& event);
		~Draw() = default;

		std::size_t get_call_num() const;
		int get_tag() const;
		int get_draw_type() const;
		glm::mat4 get_model_matrix() const;
		glm::vec3 get_translation() const;

		const kompvter::ModelDrawEvent& get_event() const;

		thoreau::TileCoordinate get_tile_coordinate() const;
		thoreau::WorldCoordinate get_world_coordinate() const;

		const kompvter::DrawParameters& get_draw_parameters() const;

		void copy_bone_transforms(principle::ModelNode* node) const;

	private:
		std::size_t call_num;
		int tag;

		int draw_type;

		glm::mat4 model_matrix;
		glm::vec3 translation;

		kompvter::ModelDrawEvent event;
	};
}

#endif
