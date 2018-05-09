// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "glooper/map/minimapManager.hpp"
#include "glooper/map/detail/wallRasterizer.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "thoreau/thoreau.hpp"

glooper::MinimapManager::MinimapManager(
	ResourceManager& resource_manager,
	Minimap& minimap,
	bool extract_walls_etc)
{
	this->resource_manager = &resource_manager;
	this->minimap = &minimap;
	this->extract_walls_etc = extract_walls_etc;

	this->resource_manager->add_listener(this);

	load_resources();
}

glooper::MinimapManager::~MinimapManager()
{
	this->resource_manager->remove_listener(this);
}

void glooper::MinimapManager::on_reload_resources()
{
	load_resources();
}

void glooper::MinimapManager::on_draw_frame(float timestamp)
{
	this->minimap->is_visible = false;
	this->finalized_minimap_element_streams.clear();
}

void glooper::MinimapManager::on_draw_enabled(float timestamp)
{
	this->draw_enabled = true;
}

void glooper::MinimapManager::on_draw_disabled(float timestamp)
{
	this->draw_enabled = false;
}

void glooper::MinimapManager::on_draw_gui(const kompvter::GUIDrawEvent& event)
{
	auto color_attachment_name = event.get_color_attachment_name();
	auto texture = event.get_atlas_name();

	if (color_attachment_name == 0)
	{
		if (this->finalized_minimap_textures.count(texture) != 0)
		{
			this->minimap->is_visible = true;

			assert(this->minimap_transfers.count(texture) != 0);
			auto working_texture = this->minimap_transfers[texture];
			assert(this->finalized_draw_properties.count(texture) != 0);
			auto properties = this->finalized_draw_properties[texture];

			this->minimap->size = properties.size;
			this->minimap->visible_size = properties.visible_size;
			this->minimap->rotation = properties.rotation;
			this->minimap->scale = properties.scale;
			this->minimap->center = properties.center;

			if (previous_minimap_texture != working_texture.get_name() ||
				this->dirty_working_minimap_textures.count(working_texture.get_name()) != 0)
			{
				build_working_map(working_texture);

				this->dirty_working_minimap_textures.erase(working_texture.get_name());
				previous_minimap_texture = working_texture.get_name();
			}

			if (!this->draw_enabled)
			{
				return;
			}

			auto iter = this->finalized_minimap_element_streams.find(texture);
			if (iter != this->finalized_minimap_element_streams.end())
			{
				extract_minimap_features(event.get_atlas());
				this->finalized_minimap_element_streams.erase(iter);
			}
		}
	}
	else if (this->working_minimap_textures.count(color_attachment_name) != 0)
	{
		if (this->extract_walls_etc)
		{
			this->working_minimap_draws.emplace(color_attachment_name, event);
		}
		this->dirty_working_minimap_textures.insert(color_attachment_name);
	}
	else
	{
		if (this->working_minimap_textures.count(texture) != 0)
		{
			transfer_minimap(event);
		}
		else if (this->finalized_minimap_textures.count(color_attachment_name) != 0)
		{
			auto iter = this->finalized_minimap_element_streams.find(color_attachment_name);
			if (iter == this->finalized_minimap_element_streams.end())
			{
				auto result = this->finalized_minimap_element_streams.emplace(
					color_attachment_name,
					chomsky::GUIElementStream(this->resource_manager->get_texture_manager(), this->resource_manager->get_texture_key_manager()));
				iter = result.first;
			}

			kvre::FlatDrawBuffer flat_draw_buffer(event);
			iter->second.update(
				event.get_atlas(),
				event.get_vertex_index_mapper(),
				event.get_draw_parameters(),
				flat_draw_buffer);
		}
	}
}

void glooper::MinimapManager::on_set_viewport(const kompvter::ViewportEvent& event)
{
	this->viewport = glm::ivec4(
		event.get_x(), event.get_y(),
		event.get_width(), event.get_height());
}

void glooper::MinimapManager::on_draw_minimap(
	const kompvter::MinimapDrawEvent& event)
{
	auto texture_name = event.get_color_attachment_name();

	auto view = event.get_view_matrix();
	auto projection = event.get_projection_matrix();
	auto viewport = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	auto min = glm::unProject(glm::vec3(0.0f, 0.0f, 0.0f), view, projection, viewport);
	auto max = glm::unProject(glm::vec3(1.0f, 1.0f, 0.0f), view, projection, viewport);
	if (this->working_minimap_textures.count(texture_name) != 0)
	{
		auto current_min = this->working_minimap_bounds[texture_name].min;
		thoreau::SectorCoordinate current_sector = thoreau::sector_from_world(current_min);
		thoreau::SectorCoordinate draw_sector = thoreau::sector_from_world(min);

		if (current_sector != draw_sector)
		{
			invalidate(texture_name);
		}
	}

	if (this->working_minimap_textures.count(texture_name) == 0)
	{
		this->working_minimap_textures.insert(texture_name);
		this->working_minimap_bounds[texture_name] = kvlc::Bounds(min, max);
	}
}

void glooper::MinimapManager::on_texture_delete(
	const kompvter::TextureEvent& event)
{
	invalidate(event.get_name());
}

void glooper::MinimapManager::on_texture_clear(
	const kompvter::TextureEvent& event)
{
	invalidate(event.get_name());
}

void glooper::MinimapManager::load_resources()
{
	// TODO.
}

void glooper::MinimapManager::invalidate(int texture)
{
	if (this->working_minimap_textures.count(texture) != 0)
	{
		this->minimap->working_map.clear();
	}

	this->working_minimap_textures.erase(texture);
	this->working_minimap_bounds.erase(texture);
	this->working_minimap_draws.erase(texture);
	this->dirty_working_minimap_textures.erase(texture);
	this->finalized_minimap_textures.erase(texture);
	this->finalized_minimap_element_streams.erase(texture);
	this->finalized_draw_properties.erase(texture);
	this->minimap_transfers.erase(texture);

	for (auto i = this->minimap_transfers.begin(); i != this->minimap_transfers.end(); /* Nil. */)
	{
		if (i->second.get_name() == texture)
		{
			i = this->minimap_transfers.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void glooper::MinimapManager::transfer_minimap(
	const kompvter::GUIDrawEvent& event)
{
	auto color_attachment_name = event.get_color_attachment_name();
	auto texture = event.get_atlas();

	this->finalized_minimap_textures.insert(color_attachment_name);
	this->minimap_transfers[color_attachment_name] = texture;

	chomsky::GUIElementStream element_stream(
		this->resource_manager->get_texture_manager(),
		this->resource_manager->get_texture_key_manager());

	kvre::FlatDrawBuffer flat_draw_buffer(event);
	element_stream.update(
		event.get_atlas(),
		event.get_vertex_index_mapper(),
		event.get_draw_parameters(),
		flat_draw_buffer);

	// There is an edge-case when only un-textured minimap GUI elements are
	// drawn (the player dot, maybe others): the current texture won't be
	// unbound. Verify only a single element is drawn, and that the element is
	// textured, to ensure proper behavior.
	if (element_stream.size() != 1)
	{
		return;
	}

	auto& element = element_stream.at(0);
	if (!element.get_is_textured())
	{
		return;
	}

	auto top_left = element.get_vertex(chomsky::GUIElement::TOP_LEFT);
	auto top_right = element.get_vertex(chomsky::GUIElement::TOP_RIGHT);
	auto bottom_left = element.get_vertex(chomsky::GUIElement::BOTTOM_LEFT);
	auto bottom_right = element.get_vertex(chomsky::GUIElement::BOTTOM_RIGHT);

	float rotation = element.get_rotation();
	float width = glm::length(glm::vec2(top_left.position - top_right.position));
	float height = glm::length(glm::vec2(top_right.position - bottom_right.position));

	auto sector_bounds = get_sector_bounds(texture);
	float width_in_tiles = sector_bounds.x * thoreau::SECTOR_SIZE;
	float height_in_tiles = sector_bounds.y * thoreau::SECTOR_SIZE;
	float minimap_scale_x = width / width_in_tiles;
	float minimap_scale_y = height / height_in_tiles;
	float tile_scale_x = viewport.z / width_in_tiles;
	float tile_scale_y = viewport.w / height_in_tiles;

	auto dx = event.get_color_attachment().get_width() / 2.0f - bottom_left.position.x;
	auto dy = event.get_color_attachment().get_height() / 2.0f - bottom_left.position.y;
	auto local_x = dx * std::cos(-rotation) - dy * std::sin(-rotation);
	local_x /= minimap_scale_x;
	auto local_y = dy * std::cos(-rotation) + dx * std::sin(-rotation);
	local_y /= minimap_scale_y;

	auto bottom_left_tile = glm::vec2(get_bottom_left_tile(texture));

	auto& properties = this->finalized_draw_properties[color_attachment_name];
	properties.rotation = rotation;
	properties.size = glm::vec2(width * tile_scale_x, height * tile_scale_y);
	properties.visible_size = glm::vec2(viewport.z, viewport.w);
	properties.scale = glm::vec2(minimap_scale_x, minimap_scale_y);
	properties.center = glm::vec2(
		bottom_left_tile.x + local_x,
		bottom_left_tile.y + local_y);
}

void glooper::MinimapManager::build_working_map(
	const kompvter::Texture& working_texture)
{
	if (this->extract_walls_etc)
	{
		extract_ladders(working_texture);
		extract_walls(working_texture);
	}

	this->working_minimap_draws.erase(working_texture.get_name());
}

void glooper::MinimapManager::extract_ladders(
	const kompvter::Texture& working_texture)
{
	chomsky::GUIElementStream element_stream(
		this->resource_manager->get_texture_manager(),
		this->resource_manager->get_texture_key_manager());

	auto range = working_minimap_draws.equal_range(working_texture.get_name());
	for (auto i = range.first; i != range.second; ++i)
	{
		auto& event = i->second;
		kvre::FlatDrawBuffer flat_draw_buffer(event);
		element_stream.update(
			event.get_atlas(),
			event.get_vertex_index_mapper(),
			event.get_draw_parameters(),
			flat_draw_buffer);
	}

	auto bottom_left_tile = get_bottom_left_tile(working_texture);
	auto bounds_in_tiles = get_sector_bounds(working_texture);
	bounds_in_tiles *= thoreau::SECTOR_SIZE;

	int x_scale = working_texture.get_width() / bounds_in_tiles.x;
	int y_scale = working_texture.get_height() / bounds_in_tiles.y;

	auto& texture_dictionary = this->resource_manager->get_texture_dictionary();
	for (auto& element: element_stream)
	{
		if (!element.get_is_textured())
		{
			continue;
		}

		if (!texture_dictionary.has(element.get_key()))
		{
			continue;
		}

		auto& definition = texture_dictionary.get(element.get_key());
		if (definition.get_value() != "minimap-feature")
		{
			continue;
		}

		auto feature_type = definition.get_data("minimap-feature-type");
		if (feature_type == "ladder")
		{
			int ladder_tile_x = (element.get_x() + element.get_width() / 2.0f) / x_scale + bottom_left_tile.x;
			int ladder_tile_y = (element.get_y() + element.get_height() / 2.0f) / y_scale + bottom_left_tile.y;

			auto& tile = this->minimap->working_map.add_tile(
				ladder_tile_x, ladder_tile_y, 0);
			tile.append_flags(thoreau::Tile::flag_portal);
			tile.append_flags(thoreau::Tile::flag_impassable);
		}
	}
}

void glooper::MinimapManager::extract_walls(
	const kompvter::Texture& working_texture)
{
	auto bottom_left_tile = get_bottom_left_tile(working_texture);
	auto bounds_in_tiles = get_sector_bounds(working_texture);
	bounds_in_tiles *= thoreau::SECTOR_SIZE;

	detail::WallRasterizer wall_rasterizer(bounds_in_tiles.x, bounds_in_tiles.y);
	glm::vec2 scale;
	scale.x = bounds_in_tiles.x;
	scale.x /= working_texture.get_width();
	scale.y = bounds_in_tiles.y;
	scale.y /= working_texture.get_height();

	auto range = working_minimap_draws.equal_range(working_texture.get_name());
	for (auto i = range.first; i != range.second; ++i)
	{
		auto& event = i->second;
		kvre::FlatDrawBuffer flat_draw_buffer(event.get_vertex_attributes());
		auto begin = kvre::IndexedVertexIterator::begin(
			event.get_vertex_index_mapper(),
			event.get_draw_parameters());
		auto end = kvre::IndexedVertexIterator::end(
			event.get_vertex_index_mapper(),
			event.get_draw_parameters());

		wall_rasterizer.rasterize(begin, end, flat_draw_buffer, scale);
	}

	wall_rasterizer.finalize(this->minimap->working_map, bottom_left_tile);
}

void glooper::MinimapManager::extract_minimap_features(
	const kompvter::Texture& finalized_texture)
{
	this->minimap->features.clear();

	auto name = finalized_texture.get_name();
	auto iter = this->finalized_minimap_element_streams.find(name);
	if (iter == this->finalized_minimap_element_streams.end())
	{
		return;
	}

	auto& element_stream = iter->second;auto& texture_dictionary = this->resource_manager->get_texture_dictionary();
	for (auto& element: element_stream)
	{
		if (!element.get_is_textured())
		{
			continue;
		}

		if (!texture_dictionary.has(element.get_key()))
		{
			continue;
		}

		auto& definition = texture_dictionary.get(element.get_key());
		if (definition.get_value() != "minimap-feature")
		{
			continue;
		}

		if (!definition.has_data("minimap-feature-type"))
		{
			continue;
		}

		auto feature_type = definition.get_data("minimap-feature-type");
		if (!feature_type.empty())
		{
			auto x = element.get_x() + element.get_width() / 2.0f;
			auto y = element.get_y();
			if (definition.has_tag("minimap-dot"))
			{
				y += element.get_height() / 2.0f;
			}

			this->minimap->features.emplace(feature_type, glm::vec2(x, y));
		}
	}
}

glm::ivec2 glooper::MinimapManager::get_bottom_left_tile(
	const kompvter::Texture& working_texture)
{
	assert(this->working_minimap_bounds.count(working_texture.get_name()) != 0);
	auto world_bounds = this->working_minimap_bounds[working_texture.get_name()];
	float tile_left = world_bounds.min.x / thoreau::TILE_SIZE;
	float tile_bottom = world_bounds.min.z / thoreau::TILE_SIZE;

	return glm::ivec2(tile_left, tile_bottom);
}

glm::ivec2 glooper::MinimapManager::get_sector_bounds(
	const kompvter::Texture& working_texture)
{
	assert(this->working_minimap_bounds.count(working_texture.get_name()) != 0);
	auto world_bounds = this->working_minimap_bounds[working_texture.get_name()];

	const float SECTOR_WIDTH = thoreau::TILE_SIZE * thoreau::SECTOR_SIZE;
	const float SECTOR_HEIGHT = thoreau::TILE_SIZE * thoreau::SECTOR_SIZE;
	float sector_left = world_bounds.min.x / SECTOR_WIDTH;
	float sector_right = world_bounds.max.x / SECTOR_WIDTH;
	float sector_top = world_bounds.max.z / SECTOR_HEIGHT;
	float sector_bottom = world_bounds.min.z / SECTOR_HEIGHT;

	return glm::ivec2(
		sector_right - sector_left,
		sector_top - sector_bottom);
}
