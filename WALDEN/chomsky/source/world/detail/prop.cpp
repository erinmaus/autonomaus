// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/detail/prop.hpp"
#include "thoreau/thoreau.hpp"

bool chomsky::is_prop_draw_event(const kompvter::ModelDrawEvent& event)
{
	if (!event.are_bone_transforms_identity())
	{
		return false;
	}

	auto translation =
		event.get_model_matrix() * glm::vec4(glm::vec3(0.0f), 1.0f);
	int tile_x = (int)translation.x % thoreau::TILE_SIZE;
	int tile_y = (int)translation.z % thoreau::TILE_SIZE;
	float elevation = glm::floor(translation.y);

	return elevation != 0.0f && tile_x == thoreau::TILE_SIZE / 2 && tile_y == thoreau::TILE_SIZE / 2;
}
