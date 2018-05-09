// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/isaac.hpp"
#include "autonomaus/graphics/worldDrawHook.hpp"
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "autonomaus/state/view.hpp"
#include "thoreau/sectorCoordinate.hpp"
#include "thoreau/thoreau.hpp"

namespace autonomaus
{
	class SectorBorder : public Isaac
	{
	public:
		SectorBorder(Autonomaus& autonomaus, LibraryHandle handle);
		~SectorBorder();

		void update() override;

	private:
		WorldOverlay::Tag tag;
		View view;
	};
}

autonomaus::SectorBorder::SectorBorder(Autonomaus& autonomaus, LibraryHandle handle) :
	Isaac(autonomaus, handle),
	view(*autonomaus.get_world_state(), *autonomaus.get_map_state())
{
	// Nothing.
}

autonomaus::SectorBorder::~SectorBorder()
{
	// Nothing.
}

void autonomaus::SectorBorder::update()
{
	if (!get_autonomaus().get_map_state()->get_minimap().get_is_visible())
	{
		return;
	}

	this->view.update(Camera(get_autonomaus().get_draw_state()));
	auto sector = thoreau::sector_from_tile(this->view.get_player_tile_coordinate());
	auto y = this->view.get_player_position().y;

	auto& overlay = get_autonomaus().get_world_draw_hook().after();
	overlay.start(this->tag, true, 5.0f);
	for (auto i = 0; i <= thoreau::SECTOR_SIZE; ++i)
	{
		for (auto j = 0; j <= thoreau::SECTOR_SIZE; ++j)
		{
			if (!(i == 0 || i == thoreau::SECTOR_SIZE || j == 0 || j == thoreau::SECTOR_SIZE))
			{
				continue;
			}

			glm::vec3 position;
			position.x = ((sector.x * thoreau::SECTOR_SIZE + i) + 0.5f) * thoreau::TILE_SIZE;
			position.y = y + 512.0f;
			position.z = ((sector.y * thoreau::SECTOR_SIZE + j) + 0.5f) * thoreau::TILE_SIZE;

			overlay.draw_box(position, glm::vec3(512.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
		}
	}

	glm::vec3 center_position;
	center_position.x = (sector.x + 0.5f) * thoreau::SECTOR_SIZE * thoreau::TILE_SIZE;
	center_position.y = y;
	center_position.z = (sector.y + 0.5f) * thoreau::SECTOR_SIZE * thoreau::TILE_SIZE;
	overlay.draw_box(center_position, glm::vec3(2048, 1024*16, 2048), glm::vec4(1.0f, 1.0f, 1.0f, 0.25f));
	overlay.stop();
}

extern "C"
autonomaus::Isaac* create_isaac_module(
	autonomaus::Autonomaus& autonomaus,
	autonomaus::Isaac::LibraryHandle handle)
{
	return new autonomaus::SectorBorder(autonomaus, handle);
}

extern "C"
void destroy_isaac_module(autonomaus::Isaac* isaac)
{
	delete isaac;
}
