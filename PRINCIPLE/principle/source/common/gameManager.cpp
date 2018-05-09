// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <allegro5/allegro.h>
#include "chomsky/io/gui/itemIconSilhouette.hpp"
#include "principle/common/jaquesFrameReader.hpp"
#include "principle/common/gameManager.hpp"
#include "touchstone/gl/glTrace.hpp"

principle::GameManager::GameManager() :
	buffer_processor(&context),
	texture_processor(&context),
	gui_processor(&context, &resource_manager.get_buffer_manager(), &resource_manager.get_texture_manager()),
	item_icon_processor(&context, &resource_manager.get_buffer_manager(), &resource_manager.get_texture_manager()),
	minimap_processor(&context, &resource_manager.get_buffer_manager(), &resource_manager.get_texture_manager()),
	model_processor(&context, &resource_manager.get_buffer_manager(), &resource_manager.get_texture_manager()),
	item_icon_resource_manager(texture_resource_manager),
	minimap_manager(resource_manager, minimap, true),
	utility_world_update_block(world_update_manager),
	map_world_update_block(world_update_manager, &map, utility_world_update_block.get_transform_state(), utility_world_update_block.get_bounds(), &runtime_map),
	static_world_update_block(world_update_manager, nullptr, utility_world_update_block.get_transform_state(), map_world_update_block.get_tile_elevation_state()),
	animated_world_update_block(world_update_manager, map_world_update_block.get_tile_elevation_state())
{
	this->buffer_processor.add(&this->resource_manager.get_buffer_manager());
	this->texture_processor.add(&this->resource_manager.get_texture_manager());
	this->texture_processor.add(&this->texture_resource_manager);
	this->texture_processor.add(&this->resource_manager.get_texture_fingerprint_manager());
	this->texture_processor.add(&this->resource_manager.get_texture_key_manager());
	this->texture_processor.add(&this->resource_manager.get_item_icon_manager());
	this->texture_processor.add(&this->item_icon_resource_manager);

	this->texture_processor.add(&this->minimap_manager);
	this->gui_processor.add(&this->minimap_manager);
	this->minimap_processor.add(&this->minimap_manager);
	this->frame_processor.add(&this->minimap_manager);

	this->world_update_manager.add_pass(&this->model_picker);
	this->world_update_manager.add_pass(&this->draw_manager);
	this->world_update_manager.add_pass(&this->model_manager);
	this->world_update_manager.add_pass(&this->terrain_manager);

	this->buffer_processor.add(&this->world_update_manager);
	this->model_processor.add(&this->world_update_manager);
	this->frame_processor.add(&this->world_update_manager);

	this->item_icon_processor.add(&this->resource_manager.get_item_icon_manager());
	this->item_icon_processor.add(&this->item_icon_resource_manager);

	auto path = std::getenv("NECRONOMICON_DATA_PATH");
	if (path != nullptr)
	{
		this->resource_manager.set_path(path);
	}

	load_texture_dictionary();
	load_special_items();
}

principle::GameManager::~GameManager()
{
	save_texture_dictionary();
}

kompvter::BufferManager&
principle::GameManager::get_buffer_manager()
{
	return this->resource_manager.get_buffer_manager();
}

const kompvter::BufferManager&
principle::GameManager::get_buffer_manager() const
{
	return this->resource_manager.get_buffer_manager();
}

kompvter::TextureManager&
principle::GameManager::get_texture_manager()
{
	return this->resource_manager.get_texture_manager();
}

const kompvter::TextureManager&
principle::GameManager::get_texture_manager() const
{
	return this->resource_manager.get_texture_manager();
}

touchstone::Context& principle::GameManager::get_touchstone_context()
{
	return this->context;
}

const touchstone::Context& principle::GameManager::get_touchstone_context() const
{
	return this->context;
}

rosalind::BufferProcessor& principle::GameManager::get_buffer_processor()
{
	return this->buffer_processor;
}

const rosalind::BufferProcessor& principle::GameManager::get_buffer_processor() const
{
	return this->buffer_processor;
}

rosalind::TextureProcessor& principle::GameManager::get_texture_processor()
{
	return this->texture_processor;
}

const rosalind::TextureProcessor& principle::GameManager::get_texture_processor() const
{
	return this->texture_processor;
}

rosalind::FrameProcessor& principle::GameManager::get_frame_processor()
{
	return this->frame_processor;
}

const rosalind::FrameProcessor& principle::GameManager::get_frame_processor() const
{
	return this->frame_processor;
}

rosalind::GUIProcessor& principle::GameManager::get_gui_processor()
{
	return this->gui_processor;
}

const rosalind::GUIProcessor& principle::GameManager::get_gui_processor() const
{
	return this->gui_processor;
}

rosalind::MinimapProcessor& principle::GameManager::get_minimap_processor()
{
	return this->minimap_processor;
}

const rosalind::MinimapProcessor& principle::GameManager::get_minimap_processor() const
{
	return this->minimap_processor;
}

rosalind::ModelProcessor& principle::GameManager::get_model_processor()
{
	return this->model_processor;
}

const rosalind::ModelProcessor& principle::GameManager::get_model_processor() const
{
	return this->model_processor;
}

const principle::GameManager::TransformState&
principle::GameManager::get_transform_state() const
{
	return *this->utility_world_update_block.get_transform_state();
}

const principle::GameManager::TileElevationState&
principle::GameManager::get_terrain_elevation_state() const
{
	return *this->map_world_update_block.get_tile_elevation_state();
}

const principle::GameManager::TilePointCloudState&
principle::GameManager::get_static_point_cloud_state() const
{
	return *this->static_world_update_block.get_point_cloud_state();
}

const principle::GameManager::SceneryMaterialsState&
principle::GameManager::get_scenery_materials_state() const
{
	return *this->static_world_update_block.get_materials_state();
}

const principle::GameManager::ActorBoundsState&
principle::GameManager::get_actor_bounds_state() const
{
	return *this->animated_world_update_block.get_actor_bounds_state();
}

const principle::GameManager::ActorDrawState&
principle::GameManager::get_actor_draw_state() const
{
	return *this->animated_world_update_block.get_actor_draw_state();
}

const principle::GameManager::ActorMaterialsState&
principle::GameManager::get_actor_materials_state() const
{
	return *this->animated_world_update_block.get_actor_materials_state();
}

const principle::GameManager::ActorPointCloudState&
principle::GameManager::get_actor_point_cloud_state() const
{
	return *this->animated_world_update_block.get_actor_point_cloud_state();
}

principle::ModelPicker&
principle::GameManager::get_model_picker()
{
	return this->model_picker;
}

const principle::ModelPicker&
principle::GameManager::get_model_picker() const
{
	return this->model_picker;
}

principle::DrawManager&
principle::GameManager::get_draw_manager()
{
	return this->draw_manager;
}

const principle::DrawManager&
principle::GameManager::get_draw_manager() const
{
	return this->draw_manager;
}

principle::ModelManager&
principle::GameManager::get_model_manager()
{
	return this->model_manager;
}

const principle::ModelManager&
principle::GameManager::get_model_manager() const
{
	return this->model_manager;
}

principle::TerrainManager&
principle::GameManager::get_terrain_manager()
{
	return this->terrain_manager;
}

const principle::TerrainManager&
principle::GameManager::get_terrain_manager() const
{
	return this->terrain_manager;
}

principle::TextureManager&
principle::GameManager::get_texture_resource_manager()
{
	return this->texture_resource_manager;
}

const principle::TextureManager&
principle::GameManager::get_texture_resource_manager() const
{
	return this->texture_resource_manager;
}

principle::ItemIconManager&
principle::GameManager::get_item_icon_resource_manager()
{
	return this->item_icon_resource_manager;
}

const principle::ItemIconManager&
principle::GameManager::get_item_icon_resource_manager() const
{
	return this->item_icon_resource_manager;
}

chomsky::TextureDictionary&
principle::GameManager::get_texture_dictionary()
{
	return this->resource_manager.get_texture_dictionary();
}

const chomsky::TextureDictionary&
principle::GameManager::get_texture_dictionary() const
{
	return this->resource_manager.get_texture_dictionary();
}

chomsky::WorldUpdateManager&
principle::GameManager::get_world_update_manager()
{
	return this->world_update_manager;
}

const chomsky::WorldUpdateManager&
principle::GameManager::get_world_update_manager() const
{
	return this->world_update_manager;
}

chomsky::TextureKeyManager&
principle::GameManager::get_texture_key_manager()
{
	return this->resource_manager.get_texture_key_manager();
}

chomsky::UtilityWorldUpdateBlock&
principle::GameManager::get_utility_block()
{
	return this->utility_world_update_block;
}

const glooper::MapWorldUpdateBlock&
principle::GameManager::get_map_block() const
{
	return this->map_world_update_block;
}

glooper::MapWorldUpdateBlock&
principle::GameManager::get_map_block()
{
	return this->map_world_update_block;
}

const chomsky::UtilityWorldUpdateBlock&
principle::GameManager::get_utility_block() const
{
	return this->utility_world_update_block;
}

chomsky::StaticWorldUpdateBlock&
principle::GameManager::get_static_block()
{
	return this->static_world_update_block;
}

const chomsky::StaticWorldUpdateBlock&
principle::GameManager::get_static_block() const
{
	return this->static_world_update_block;
}

chomsky::AnimatedWorldUpdateBlock&
principle::GameManager::get_animated_block()
{
	return this->animated_world_update_block;
}

const chomsky::AnimatedWorldUpdateBlock&
principle::GameManager::get_animated_block() const
{
	return this->animated_world_update_block;
}

const chomsky::TextureKeyManager&
principle::GameManager::get_texture_key_manager() const
{
	return this->resource_manager.get_texture_key_manager();
}

chomsky::TextureFingerprintManager&
principle::GameManager::get_texture_fingerprint_manager()
{
	return this->resource_manager.get_texture_fingerprint_manager();
}

const chomsky::TextureFingerprintManager&
principle::GameManager::get_texture_fingerprint_manager() const
{
	return this->resource_manager.get_texture_fingerprint_manager();
}

chomsky::ItemIconManager&
principle::GameManager::get_item_icon_manager()
{
	return this->resource_manager.get_item_icon_manager();
}

const chomsky::ItemIconManager&
principle::GameManager::get_item_icon_manager() const
{
	return this->resource_manager.get_item_icon_manager();
}

thoreau::Map& principle::GameManager::get_map()
{
	return this->map;
}

const thoreau::Map& principle::GameManager::get_map() const
{
	return this->map;
}

thoreau::Map& principle::GameManager::get_runtime_map()
{
	return this->runtime_map;
}

const thoreau::Map& principle::GameManager::get_runtime_map() const
{
	return this->runtime_map;
}

glooper::ResourceManager& principle::GameManager::get_resource_manager()
{
	return this->resource_manager;
}

const glooper::ResourceManager& principle::GameManager::get_resource_manager() const
{
	return this->resource_manager;
}

glooper::Minimap& principle::GameManager::get_minimap()
{
	return this->minimap;
}

const glooper::Minimap& principle::GameManager::get_minimap() const
{
	return this->minimap;
}

void principle::GameManager::load(
	LoadingScreen& loading_screen,
	JaquesFrameReader& reader,
	std::size_t frame_num, std::size_t count, std::size_t step)
{
	this->world_update_manager.reset();
	this->world_update_manager.set_loading_screen(loading_screen);

	this->resource_manager.get_buffer_manager().reset();
	this->resource_manager.get_texture_manager().reset();
	this->resource_manager.get_texture_key_manager().reset();

	this->context = touchstone::Context();

	this->draw_manager.reset();

	this->texture_resource_manager.reset();
	this->item_icon_resource_manager.reset();

	this->resource_manager.get_texture_key_manager().reset();
	this->resource_manager.get_texture_fingerprint_manager().reset();

	this->resource_manager.get_item_icon_manager().reset();

	float total = 0.0f;

	touchstone::GLTrace trace;
	for (std::size_t i = 0; i < frame_num; ++i)
	{
		reader.read(i, trace);

		auto start = al_get_time();
		this->frame_processor.on_frame(al_get_time());
		for (auto& call: trace)
		{
			this->buffer_processor.before_update(call);
			this->texture_processor.before_update(call);
			this->gui_processor.before_update(call);
			this->item_icon_processor.before_update(call);
			this->minimap_processor.before_update(call);

			this->context.update(call);

			this->buffer_processor.after_update(call);
			this->texture_processor.after_update(call);
			this->gui_processor.after_update(call);
			this->item_icon_processor.after_update(call);
			this->minimap_processor.after_update(call);
		}

		auto stop = al_get_time();
		auto difference = (stop - start) * 1000.0f;
		total += difference;

		trace.clear();
		loading_screen.next_frame(false);
		if (loading_screen.get_quit_pending())
		{
			return;
		}
	}

	std::size_t current_step = step;
	for (std::size_t i = 0; i < count; ++i)
	{
		reader.read(i + frame_num, trace);

		auto start = al_get_time();

		++current_step;
		this->frame_processor.on_frame(al_get_time());
		if (current_step >= step)
		{
			this->frame_processor.enable_draw(al_get_time());
		}

		for (auto& call: trace)
		{
			this->buffer_processor.before_update(call);
			this->texture_processor.before_update(call);
			this->gui_processor.before_update(call);
			this->item_icon_processor.before_update(call);
			this->minimap_processor.before_update(call);
			this->model_processor.before_update(call);

			this->context.update(call);

			this->buffer_processor.after_update(call);
			this->texture_processor.after_update(call);
			this->gui_processor.after_update(call);
			this->item_icon_processor.after_update(call);
			this->model_processor.after_update(call);
			this->minimap_processor.after_update(call);
		}

		if (current_step >= step)
		{
			this->frame_processor.disable_draw(al_get_time());
			this->world_update_manager.update();

			current_step = 0;
		}

		auto stop = al_get_time();;
		auto difference = (stop - start) * 1000.0f;
		total += difference;

		trace.clear();
		loading_screen.next_frame(true);
		if (loading_screen.get_quit_pending())
		{
			return;
		}
	}

	printf("total time: %.4f ms (%.4f ms average)\n", total, total / (frame_num + count));
}

void principle::GameManager::load_texture_dictionary()
{
	auto path = std::getenv("NECRONOMICON_DATA_PATH");
	if (path == nullptr)
	{
		return;
	}

	std::ifstream stream(std::string(path) + "/textures.dat");
	mantorok::DATReader reader(stream);

	mantorok::DATDeserializer<chomsky::TextureDictionary>::deserialize(
		this->resource_manager.get_texture_dictionary(), reader);
}

void principle::GameManager::save_texture_dictionary()
{
	auto path = std::getenv("NECRONOMICON_DATA_PATH");
	if (path == nullptr)
	{
		return;
	}

	std::ofstream stream(std::string(path) + "/textures.dat");
	mantorok::DATWriter writer(stream);

	auto sanitized_dictionary = this->resource_manager.get_texture_dictionary();
	bool is_clean = false;
	while (!is_clean)
	{
		is_clean = true;
		for (auto& definition: sanitized_dictionary)
		{
			if (definition.second.has_tag("runtime"))
			{
				sanitized_dictionary.remove(definition.first);
				is_clean = false;
				break;
			}
		}
	}

	mantorok::DATSerializer<chomsky::TextureDictionary>::serialize(
		sanitized_dictionary, writer);
}

void principle::GameManager::load_special_items()
{
	auto path = std::getenv("NECRONOMICON_DATA_PATH");
	if (path == nullptr)
	{
		return;
	}

	std::ifstream stream(std::string(path) + "/items/note.dat");
	mantorok::DATReader reader(stream);

	chomsky::ItemIconSilhouette note_silhouette;
	mantorok::DATDeserializer<chomsky::ItemIconSilhouette>::deserialize(
		note_silhouette, reader);

	if (stream)
	{
		this->resource_manager.get_item_icon_manager().add_special(
			note_silhouette.to_string(),
			note_silhouette);
	}
}

void principle::GameManager::ProxyWorldUpdateManager::set_loading_screen(LoadingScreen& loading_screen)
{
	this->loading_screen = &loading_screen;
}

void principle::GameManager::ProxyWorldUpdateManager::after_draw_terrain(
	chomsky::WorldUpdatePass* pass,
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	if (loading_screen != nullptr)
	{
		this->loading_screen->update();
	}
}

void principle::GameManager::ProxyWorldUpdateManager::after_draw_model(
	chomsky::WorldUpdatePass* pass,
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (loading_screen != nullptr)
	{
		this->loading_screen->update();
	}
}

void principle::GameManager::ProxyWorldUpdateManager::after_draw_water(
	chomsky::WorldUpdatePass* pass,
	int tag, const kvre::Terrain* water,
	const kompvter::ModelDrawEvent& event)
{
	if (loading_screen != nullptr)
	{
		this->loading_screen->update();
	}
}

void principle::GameManager::ProxyWorldUpdateManager::after_draw_particles(
	chomsky::WorldUpdatePass* pass,
	int tag, const kvre::Particles* particles,
	const kompvter::ModelDrawEvent& event)
{
	if (loading_screen != nullptr)
	{
		this->loading_screen->update();
	}
}

void principle::GameManager::ProxyWorldUpdateManager::after_start(chomsky::WorldUpdatePass* pass)
{
	if (loading_screen != nullptr)
	{
		this->loading_screen->update();
	}
}

void principle::GameManager::ProxyWorldUpdateManager::after_stop(chomsky::WorldUpdatePass* pass)
{
	if (loading_screen != nullptr)
	{
		this->loading_screen->update();
	}
}
