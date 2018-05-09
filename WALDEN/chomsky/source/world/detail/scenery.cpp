// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/detail/scenery.hpp"
#include "thoreau/thoreau.hpp"

bool chomsky::is_scenery_draw_event(const kompvter::ModelDrawEvent& event)
{
	if (!event.are_bone_transforms_identity())
	{
		return false;
	}

	auto translation =
		event.get_model_matrix() * glm::vec4(glm::vec3(0.0f), 1.0f);

	// It actually apears scenery is baked in 16 x 16 tile blocks.
	//
	// Ergo, we should be checking to see if it's a multiple of 16 * TILE_SIZE,
	// but this should work for now.
	int tile_x = (int)translation.x % thoreau::TILE_SIZE;
	int tile_y = (int)translation.z % thoreau::TILE_SIZE;
	float elevation = glm::floor(translation.y);

	return tile_x == 0 && tile_y == 0 && elevation == 0.0f;
}