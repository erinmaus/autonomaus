// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_TERRAIN_MANAGER_HPP
#define PRINCIPLE_RESOURCES_TERRAIN_MANAGER_HPP

#include "chomsky/world/worldUpdatePass.hpp"
#include "kvre/terrain/terrain.hpp"

namespace principle
{
	class TerrainManager : public chomsky::WorldUpdatePass
	{
	public:
		TerrainManager() = default;
		~TerrainManager() = default;

		bool has(int tag);
		const kvre::Terrain* get(int tag);

		void draw_terrain(
			int tag,
			const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;
		void invalidate_terrain(const kvre::Terrain* terrain) override;

	private:
		std::unordered_map<int, const kvre::Terrain*> tags;
		std::unordered_map<const kvre::Terrain*, kvre::Terrain> terrains;
	};
}

#endif
